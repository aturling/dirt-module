(function ($) {
  // Define location of table source json for ajax calls
  var DIRT_DATATABLE_SOURCE_URL = window.location.pathname + '/src';
  var DIRT_DATATABLE_METADATA_URL = window.location.pathname + '/cols';

  // Define cut offs between regular computer view and
  // tablet/mobile view
  var MAX_TABLET_WIDTH = 1024;
  var MIN_TABLET_WIDTH = 768;
  var MAX_MOBILE_WIDTH = 720;

  var surveyColGroups = []; // Survey column groups

  var table = '';         // Table object
  var tableCols = '';     // Table column data
  var tableTop = '';      // Top of table position (updated upon window resize)
  var tableFixed = false; // Whether table is fixed (updated upon window resize)
  var dateColIdx = '';    // Date column index (changes frequently, depends on logged in user, etc.) and is slow so do this once

  var map = '';              // Map
  var prevInfoWindow = '';   // Currently open InfoWindow (none open initially)
  var mapMarkers = [];       // Array of all map markers
  var mapMarkerIDs = [];     // Array of site ids for markers
  var onlySiteSearchID = ''; // Search on one site ID only

  // Keep track of current screen width so that page layout is changed
  // upon resize
  var currentWinWidth = '';

  // Go to top of page on load/refresh
  window.onbeforeunload = function(){ window.scrollTo(0,0); }

  Drupal.dirtDataTable = {};

  /**
   * Sets up page elements (table and map, if enabled).
   */
  Drupal.behaviors.dirtDatatable = {
    attach: function (context, settings) {
      Drupal.dirtDataTable.getTableColumnData();

      // Set current window width as initial value for currentWinWidth
      currentWinWidth = window.innerWidth;

      // We'll reset the search after the table loads, but
      // go ahead and set the default search type now because
      // it looks weird to have the radio button unchecked initially
      $("#search-type-site-id").prop("checked", true);
      Drupal.dirtDataTable.toggleSearchType();

      // Get survey column classes from variable checkbox filter
      // Special case: make group-info first item (no cols, only rows assigned to it)
      surveyColGroups.push('group-info');
      $('#variable-checkboxes-container input[type=checkbox]:not([value=group-info])').each(function () {
        surveyColGroups.push($(this).val());
      });

      // Change Site ID select filter to chosen format
      $('#site-id-select').chosen({width: "120px"});

      // Change habitat type select filter to chosen format
      $('#habitat-type-select').chosen({width: "150px"});

      // Change county select filter to chosen format
      $('#county-select').chosen({width: "120px"});

      // Hide the disabled button
      $('.button-disabled').hide();

      // Add map
      Drupal.dirtDataTable.initMap();

      // At this point, we want to set the page layout to go ahead and get the
      // table and filter menu in position ASAP.
      // Note that this function is called here at first page load and
      // potentially multiple times, whenever the browser window is resized.
      Drupal.dirtDataTable.setPageLayout();

      // Add map markers from ajax source (unless using map module)
      Drupal.dirtDataTable.addMapMarkers();

      // Next we need to add a bunch of handlers for all of the filters, page
      // resizing, etc. These first handlers can be set right here.

      // Add handler to table filters side menu to make it appear to collapse to
      // the left
      $('#hide-show-filters-link').click(function(e) {
        Drupal.dirtDataTable.toggleFilterMenu();
        e.preventDefault();
        return false;
      });

      // Add resize handler that resets page if resized from small to large or
      // vice versa
      var resizeTimer;
      $(window).on('resize', function (e) {
        clearTimeout(resizeTimer);
        resizeTimer = setTimeout(function () {
          // Update page layout
          Drupal.dirtDataTable.setPageLayout();

          // Update table height
          Drupal.dirtDataTable.resetTableHeight();

          // If changing from desktop<->tablet or tablet<->mobile, reset all page
          // elements (because filters change, etc.)
          // Additionally, destroy and rebuild table (to go between large table and
          // small table, also to create/destroy mobile-friendly table headers)
          if ( ((window.innerWidth >  MAX_TABLET_WIDTH) && (currentWinWidth <= MAX_TABLET_WIDTH))
               || ((window.innerWidth <= MAX_TABLET_WIDTH) && (currentWinWidth >  MAX_TABLET_WIDTH))
               || ((window.innerWidth <= MAX_MOBILE_WIDTH) && (currentWinWidth > MAX_MOBILE_WIDTH))
               || ((window.innerWidth > MAX_MOBILE_WIDTH) && (currentWinWidth <= MAX_MOBILE_WIDTH))
             ) {
            Drupal.dirtDataTable.resetPage();
            table.destroy();
            Drupal.dirtDataTable.createDataTable();
          }

          // Update current window width
          currentWinWidth = window.innerWidth;
        }, 250);
      });

      // Add scroll handler to fix table position
      $(window).scroll(function() {
        Drupal.dirtDataTable.fixTablePosition();
      });

      // Add handlers for all of the filters (desktop and mobile)
      Drupal.dirtDataTable.addFilterHandlers();
    }
  };


  /*-----------------------------------------------------------*/
  /* Functions to handle page layout and positions of page     */
  /* elements (table, filter menu, etc.)                       */
  /*-----------------------------------------------------------*/

  /**
   * Sets the layout of the search table and filters on the page, according to
   * the current browser window width.
   */
  Drupal.dirtDataTable.setPageLayout = function () {
    // Fit table to window size (minus page margins 40px each, left margin 20px,
    // and left menu
    // 226px - 2 extra pixels because Safari)

    // First update table width:
    var winWidth = $(window, ).width() - 328;
    if ( (window.innerWidth <= MAX_TABLET_WIDTH) && (window.innerWidth > MIN_TABLET_WIDTH)) {
      // Too small to have filter to the left, move filters to the top and
      // expand table to full width
      // Also add about 40px of padding for tablet size
      winWidth = $(window, ).width() - 80;
      $('#table-filters', ).css('width', '100%');
    }
    else if (window.innerWidth <= MIN_TABLET_WIDTH) {
      // Same as above but no extra padding needed
      winWidth = $(window, ).width() - 20;
      $('#table-filters', ).css('width', '100%');
    }
    else {
      // i.e. window.innerWidth > MAX_TABLET_WIDTH; set left hand filter menu width
      $('#table-filters', ).css('width', '224px');
    }

    var tableContainer = $('#table-container', );
    tableContainer.css('width', winWidth);

    // Update vertical positions unless table is already locked into position
    if (!tableFixed) {
      tableTop = tableContainer.offset().top - 26;
    }
  };

  /**
   * Resets the table height based on the current browser window height.
   */
  Drupal.dirtDataTable.resetTableHeight = function () {
    var tableHeight = window.innerHeight - 300;
    if (window.innerWidth <= MAX_TABLET_WIDTH) {
      tableHeight = 475;  // default
    }
    $('.dataTables_scrollBody', ).css('height', tableHeight + 'px');
  };

  /**
   * Sets the table position so that it stays at the top of the page when
   * scrolling vertically downward.
   * Also un-stickies the table to the top of the page when scrolling
   * vertically upward beyond the top of the table.
   */
  Drupal.dirtDataTable.fixTablePosition = function () {
    var tableContainer = $('#table-container', );

    // Get current window top position:
    var windowTop = $(window, ).scrollTop();

    if ( !(tableFixed) && (windowTop > tableTop) ) {
      tableFixed = true;
      tableContainer.css({
        'position' : 'fixed',
        'top'      : '30px',
        'left'     : '264px',
      });
    }
    else if ( tableFixed && (windowTop <= tableTop) ) {
      tableFixed = false;
      tableContainer.css({
        'position' : 'relative',
        'top'      : '0px',
        'left'     : '0px',
      });
    }
  };

  /**
   * Hides/shows filter menu.
   */
  Drupal.dirtDataTable.toggleFilterMenu = function () {
    if ($('#table-filters', ).hasClass('hidden')) {
      // The menu has been expanded.
      // Return the data table back to the right side of the page
      var winWidth = $(window, ).width() - 328;
      $('#table-container', ).css('margin-left', '20px');
      $('#table-container', ).css('width', winWidth);
      //$('#table-and-filters-container #table-container', ).css('position', 'relative');
      $('#table-filters', ).removeClass('hidden');
      $('#hide-show-filters-link', ).text('Hide filter menu');
    }
    else {
      // The menu has been collapsed.
      // Give the data table the full width of the page
      var winWidth = $(window, ).width() - 84;
      $('#table-container', ).css('margin-left', '0px');
      $('#table-container', ).css('width', winWidth);
      //$('#table-and-filters-container #table-container', ).css('position', 'absolute');
      $('#table-filters', ).addClass('hidden');
      $('#hide-show-filters-link', ).text('Show filter menu');
    }
  };

  /**
   * Resets the entire page as a result of a browser width change from
   * desktop <-> tablet or tablet <-> mobile.
   * Resets the page layout and search.
   */
  Drupal.dirtDataTable.resetPage = function () {
    /**
     * This function gets called whenever switching from mobile to desktop or
     * vice versa
     *
     * Note that this function is NOT called upon page load - it doesn't make
     * sense to because the table object may not even exist yet when page is
     * first loaded. This function can ONLY be triggered by a window resizing.
     */
    
    /**
     * Disable draw event handler (if enabled)
     */
     if (table) { table.off('draw.dt'); }
     
    /**
     * We need to set the page layout which sets the table and filter widths
     * and positions (in theory this was just recently called but in practice we
     * need to call it again to get the table height exactly correct for the
     * fixed table position when scrolling).
     */
    Drupal.dirtDataTable.setPageLayout();

    // Then we need to switch the filters - desktop -> mobile or vice versa
    // but this should be handled by the resetSearch() function which should
    // check current browser width and hide/display filters according to width.
    Drupal.dirtDataTable.resetSearch(true);
  };


  /*-----------------------------------------------------------*/
  /* Functions for handling filters and table search           */
  /* (applying filters, etc.)                                  */
  /*-----------------------------------------------------------*/

  /**
   * Restores the filters to their default selections.
   * Only filters are changed (checkboxes, drop-downs, etc.); DOES NOT apply
   * the filters to the table.
   *
   * Also displays filters according to browser window width (shows checkboxes
   * if in desktop mode, switches to drop-downs if mobile/tablet, etc.).
   *
   * Does not handle the search type (desktop only); that must be handled
   * separately!
   */
  Drupal.dirtDataTable.setFilterDefaults = function () {
    // Note that since this function gets called every time there's a browser
    // width change or the search is reset, we only have to set defaults for
    // filters that are actually displayed
    if (window.innerWidth > MAX_TABLET_WIDTH) {
      // Clear site id filter
      $('#site-id-select option:selected', ).prop("selected", false);
      $('#site-id-select').trigger('chosen:updated');

      // Clear habitat type filter
      $('#habitat-type-select option:selected', ).prop("selected", false);
      $('#habitat-type-select').trigger('chosen:updated');

      // Clear county filter
      $('#county-select option:selected', ).prop("selected", false);
      $('#county-select').trigger('chosen:updated');

      // Check all checkboxes:
      $('#table-filters input[type=checkbox]').prop("checked", true);

      // Show all desktop filters
      $('.large-width-filter').show();

      // Hide all mobile dropdown filters
      $('.small-width-filter').hide();
    }
    else {
      // Default dropdowns to first selections
      $('#habitat-type-dropdown-container select').val('All Habitats');
      $('#variable-dropdown-container select').val('soil-core');

      // Hide desktop filters
      $(".large-width-filter").hide();

      // Show all mobile dropdown filters
      $(".small-width-filter").show();
    }

    // Date filter is for all window widths:
    // Reset date filters to first option (select year/month prompts)
    $('#date-range-filter select').val($("#target option:first").val());
  };

  /**
   * Applies the mobile (drop-down) filters to the search table.
   * When called, it is assumed that the mobile dropdown filters are currently
   * visible on the page.
   */
  Drupal.dirtDataTable.applyMobileFilters = function () {
    // Apply the dropdown searches - habitat type and variable
    // Since this is a special case and is called from resetSearch, we can call
    // the functions directly rather than going through the generic
    // triggerTableFilter() function.
    Drupal.dirtDataTable.toggleTableColsSelect();
    Drupal.dirtDataTable.toggleHabitatTypeSelect();

    // Then update survey links
    Drupal.dirtDataTable.toggleSurveyLinks();
  };

  /**
   * Resets the search table; i.e., clears all existing filters and returns
   * filters to their default selections.
   *
   * Input: resetSearchTypeBool (boolean true/false)
   *   - If true, additionally set the default search type (Site ID)
   *   - Otherwise, do not set the default search type (assumed it will be set
   *     elsewhere)
   */
  Drupal.dirtDataTable.resetSearch = function (resetSearchTypeBool) {
    // Housekeeping - clear warnings, etc.
    Drupal.dirtDataTable.doHousekeeping();

    // Clear out single site search ID
    onlySiteSearchID = '';

    // Clear date search if present - have to do this separately
    // (Check first if table object exists just in case)
    if ( table && ($.fn.dataTableExt.afnFiltering.length > 0) ) {
      $.fn.dataTableExt.afnFiltering.pop();
    }

    // Set the filters to their defaults and show/hide filters according to current browser window width
    Drupal.dirtDataTable.setFilterDefaults();

    // Default to showing all columns
    Drupal.dirtDataTable.showAllSurveyCols();

    // Redraw the table, clearing all filters
    if (table) {
      table.search('').columns().search('').draw();
    }

    // Now that all rows, all columns are visible, make all survey links visible (admin-only column):
    Drupal.dirtDataTable.showAllSurveyLinks();

    // Finally, we need to apply the default filters
    // For desktop width, the default is to show everything in the table so
    // there isn't much to do after we've redrawn the table above except to make
    // sure that all map markers are visible and possibly reset the search type.
    // For mobile, we need to filter the table according to the first option in
    // the variable and habitat drop-downs.
    if (window.innerWidth > MAX_TABLET_WIDTH) {
      Drupal.dirtDataTable.showAllMapMarkers();

      // If resetSearchTypeBool is true, set the search type to the default "search by site id"
      if (resetSearchTypeBool) {
        $("#search-type-site-id", ).prop("checked", true);
        Drupal.dirtDataTable.toggleSearchType();
      }
    }
    else {
      Drupal.dirtDataTable.applyMobileFilters();
    }
  };

  /**
   * This Refreshes the table data (without reloading entire page).
   */
  Drupal.dirtDataTable.refreshTable = function () {
    if (table) {
      // Add refreshing throbber and status message
      $('#refresh-table-loading-status').addClass('ajax-progress ajax-progress-throbber');
      $('#refresh-table-loading-status-text').html('refreshing . . .');

      // Reload table from ajax source
      table.ajax.reload(Drupal.dirtDataTable.tableRefreshed);
    }
  };

  /**
   * Callback function after the table has been successfully refreshed.
   */
  Drupal.dirtDataTable.tableRefreshed = function () {
    // Clear refreshing loading throbber and status message
    $('#refresh-table-loading-status').removeClass('ajax-progress ajax-progress-throbber');
    $('#refresh-table-loading-status-text').html('');

    // Since table is completely redrawn, all survey links become visible again.
    // Need to call the function that toggles the survey link visibility based on the selected variable(s):
    Drupal.dirtDataTable.toggleSurveyLinks();

    // Additionally, while the table filters are still applied to any new content created between the initial table load
    // and the table refresh (so the new content will appear in the table if & only if it meets the current filter criteria),
    // the map markers aren't automatically refreshed, so we need to run through this again in case the table refresh has caused
    // a site to be added to or removed from the table.
    if (window.innerWidth > MAX_TABLET_WIDTH) {
      Drupal.dirtDataTable.showFilteredMapMarkers();
    }
  };

  /**
   * Shows/hides the filters according to the selected search type from the
   * radio button selection.
   * Current search is reset to begin a new search with the new search type.
   */
  Drupal.dirtDataTable.toggleSearchType = function () {
    // Get the selected search type from the radio buttons
    var searchTypeVal = $('#search-type-radios input[type=radio]:checked').val();

    // This function should really only be called when the search type radio
    // buttons are present, but just in case, check the window width first:
    if (window.innerWidth > MAX_TABLET_WIDTH) {
      // Show/hide filters according to the search type that was selected.
      if (searchTypeVal == 'habitat') {
        $("#site-id-filter").hide();
        $("#habitat-type-filter .large-width-filter").show();
      }
      else if (searchTypeVal == 'site-id') {
        $("#site-id-filter").show();
        $("#habitat-type-filter .large-width-filter").hide();
      }
      else {
        $("#site-id-filter").hide();
        $("#habitat-type-filter .large-width-filter").hide();
      }
    }
  };

  /**
   * Applies a filter to the data table.
   * Handles all of the things that filters have in common, then, based on the
   * filter type param string that was passed in, applies the specific filter
   * to the table.
   */
  Drupal.dirtDataTable.triggerTableFilter = function (filterTypeStr) {
    if (table) {
      // First do housekeeping (clear warnings, etc.)
      Drupal.dirtDataTable.doHousekeeping();

      // Apply the actual filter to the table according to filterTypeStr
      // For desktop filters, return value will be whether the filter is valid
      // (so true = filter applied, false = filter was not applied and warning
      // message is displayed, which means user needs to do take action to fix
      // it).
      var filterApplied = true;

      switch(filterTypeStr) {
        case "site-id":
          // Site ID select filter
          Drupal.dirtDataTable.toggleSiteIDChosenSearch();
          break;

        case "habitat-chosen":
          // Habitat type chosen select filter
          Drupal.dirtDataTable.toggleHabitatChosenSearch();
          break;

        case "habitat-select":
          // Habitat type select dropdown filter
          Drupal.dirtDataTable.toggleHabitatTypeSelect();
          break;

        case "date":
          // Date filter
          // Potential for errors, so check that filter was applied:
          filterApplied = Drupal.dirtDataTable.searchByDateRange();
          break;

        case "variable-checkboxes":
          // Variable checkboxes filter
          Drupal.dirtDataTable.toggleVariableCheckboxSearch();
          break;

        case "variable-select":
          // Variable select dropdown filter
          Drupal.dirtDataTable.toggleTableColsSelect();
          break;

        case "county":
          // County select filter
          Drupal.dirtDataTable.toggleChosenSearch('county-select', 'dirt-site-county');
          break;
      }

      // After applying the filter (whether it was successful or not), update the
      // map markers (possibly hiding all if there's a problem with the filter choice)
      if (window.innerWidth > MAX_TABLET_WIDTH) {
        Drupal.dirtDataTable.showFilteredMapMarkers();
      }

      // If filter was successfully applied, check if the table is empty (no
      // results found.
      // Otherwise, display the appropriate warning message to indicate to the
      // user that action is needed to fix the issue with the filter.
      if (filterApplied) {
        // Check if table empty which will display message to user if so
        // If not empty, additionally hide or show survey nids appearing in row
        // after table has been filtered
        // (Of course if the table is empty, there are no rows...)
        if (!Drupal.dirtDataTable.checkTableEmpty()) {
          Drupal.dirtDataTable.toggleSurveyLinks();
        }
      }
      // Otherwise, there was an error: the filter function itself takes care of
      // displaying the error and disabling other filters.
    }
  };

  /**
   * Clears any visible warnings, closes any open info windows, etc. to tidy up
   * the page, usually called before making a change to the search table.
   */
  Drupal.dirtDataTable.doHousekeeping = function () {
    // Clear any table warnings
    Drupal.dirtDataTable.clearWarning();

    // Enable any filters that may have been disabled
    Drupal.dirtDataTable.enableAllFilters();

    // Close any open map marker info windows
    if ((window.innerWidth > MAX_TABLET_WIDTH) && prevInfoWindow) {
      prevInfoWindow.close();
    }
  };

  /**
   * Clears any search warnings that may be present.
   */
  Drupal.dirtDataTable.clearWarning = function () {
    // Clear the state search that emptied table
    if (table) {
      table.columns('.dirt-user-collection-state').search('',  true, false, true).draw();
    }
  };

  /**
   * Displays warning text given as input param.
   */
  Drupal.dirtDataTable.displayWarning = function (text) {
    // Empty the table by searching for Collection Status = 0 
    // (never happens; doesn't interfere with other table filters,
    // generally can rely on this field being in the table)
    if (table) {
      table.columns('.dirt-user-collection-state').search('^0$',  true, false, true).draw();
    }

    // Display custom table empty message
    $('span.empty-table-message', ).hide().html(text).fadeIn();
  };

  /**
   * Enables all filters to undo the disabling of some filters.
   */
  Drupal.dirtDataTable.enableAllFilters = function () {
    // Enable checkboxes:
    $('input[type=checkbox]').prop('disabled', false);

    // Show check/uncheck alls:
    $('.check-all').show();

    // Enable small-width drop-downs:
    $('.small-width-filter select').prop('disabled', false);

    // Enable chosen select filters:
    $('.chosen-select').prop('disabled', false).trigger("chosen:updated");

    // Enable search text box
    // Commented out - disabled for now
    // $('#dirt-data-table_filter input').prop('disabled', false);

    // Enable refresh button
    $('.button-disabled').hide();
    $('.button-enabled').show();
  };

  /**
   * Disables all filters other than the date dropdowns, in case there's an 
   * error with the date selection that needs to be corrected before messing
   * with the other filters.
   */
  Drupal.dirtDataTable.disableNonDateFilters = function () {
    // Disable chosen select filters:
    $('.chosen-select').prop('disabled', true).trigger("chosen:updated");

    // Disable checkboxes:
    $('input[type=checkbox]').prop('disabled', true);

    // Hide check/uncheck alls:
    $('.check-all').hide();

    // Disable small-width drop-downs:
    $('.small-width-filter select').prop('disabled', true);

    // Disable search text box
    // Commented out - disabled for now
    // $('#dirt-data-table_filter input').prop('disabled', true);

    // Disable refresh button by hiding the actual button and replacing with
    // disabled button
    $('.button-enabled').hide();
    $('.button-disabled').show();
  };

  /**
   * Checks if the table is empty as a result of the filter(s) applied, and if
   * so, report to user.
   * Returns true if the table is empty and false otherwise.
   */
  Drupal.dirtDataTable.checkTableEmpty = function () {
    var retVal = false;

    var msg = 'No results found for the selected filters. Expand the ' +
              'search to include more habitat types, Site IDs, and/or a less ' +
              'restrictive date range.';

    if (window.innerWidth <= MAX_MOBILE_WIDTH) {
      msg = 'No results found for the selected filters.';  // shorten for mobile
    }

    if (table && (! table.rows({'filter':'applied'}).data().count() )  ) {
      retVal = true;
      Drupal.dirtDataTable.displayWarning(msg);
    }

    return retVal;
  };

  /**
   * Adds the handlers for all of the table filters (desktop and mobile
   * dropdowns).
   */
  Drupal.dirtDataTable.addFilterHandlers = function () {
    // Add reset button handler
    $('#reset-search-button a').click(function(e) {
      Drupal.dirtDataTable.resetSearch(true);
      e.preventDefault();
      return false;
    });

    // Add table refresh button handler
    $('#refresh-table-button a.button-enabled').click(function(e) {
      Drupal.dirtDataTable.refreshTable();
      e.preventDefault();
      return false;
    });

    // Add handler for "disabled" buttons (that does nothing)
    $('a.button-disabled').click(function(e) {
      e.preventDefault();
      return false;
    });

    // Add the search type handler (radio buttons)
    $("#search-type-radios input[type=radio]").change(function() {
      Drupal.dirtDataTable.changeSearchType();
    });

    // Add check all / uncheck all handlers for variable checkboxes
    // Check All:
    $('#check-all-variable').click(function(e) {
      Drupal.dirtDataTable.toggleCheckAll('variable', true);
      e.preventDefault();
      return false;
    });
    // Uncheck All:
    $('#uncheck-all-variable').click(function(e) {
      Drupal.dirtDataTable.toggleCheckAll('variable', false);
      e.preventDefault();
      return false;
    });

    // Rest of handlers:

    // For each of these, we want to call a generic triggerTableFilter()
    // function which is a blueprint for what every filter that alters the table
    // needs to do additionally pass in a string param that tells which search
    // filter was triggered so that the specific filter can be applied to the
    // table (stacking with other filters).

    // Desktop width filters (habitat type chosen select, Site ID select,
    // variable checkboxes):

    // Add handler to habitat type chosen select
    $('#habitat-type-select').chosen().change(function() {
      Drupal.dirtDataTable.triggerTableFilter("habitat-chosen");
    });

    // Add handler to Site ID chosen select
    $('#site-id-select').chosen().change(function() {
      Drupal.dirtDataTable.triggerTableFilter("site-id");
    });

    // Add handler to the county chosen select
    $('#county-select').chosen().change(function() {
      Drupal.dirtDataTable.triggerTableFilter("county");
    });

    // Add handlers for variable checkboxes
    for (i=0; i<surveyColGroups.length; i++) {
      $("#variable-checkboxes-container input[type=checkbox][value=" + surveyColGroups[i] + "]").change(function() {
        Drupal.dirtDataTable.triggerTableFilter("variable-checkboxes");
      });
    }

    // Desktop and tablet width filters (date search):

    // Add handlers to date dropdowns
    $('#date-range-filter select').change(function() {
      Drupal.dirtDataTable.triggerTableFilter("date");
    });

    // Mobile and tablet width filters (habitat type select, variable select):

    // Add handler to variable select
    $('#variable-filter select').change(function() {
      Drupal.dirtDataTable.triggerTableFilter("variable-select");
    });

    // Add handler to habitat type select
    $('#habitat-type-dropdown-container select').change(function() {
      Drupal.dirtDataTable.triggerTableFilter("habitat-select");
    });
  };

  /**
   * Handles changing the search type.
   * Gets the desired search type from the selected radio button, resets the
   * search, and hides/shows filters as appropriate for the search type.
   */
  Drupal.dirtDataTable.changeSearchType = function () {
    // Changing the search type means we need to reset the search.
    // Call resetSearch() which will clear the current search, do housekeeping,
    // etc. We're going to do a soft reset by passing in "false" for
    // resetSearchTypeBool to indicate that we don't need to set the search type
    // to the default selection (since it's about to change anyway).
    Drupal.dirtDataTable.resetSearch(false);

    // Now change the search type to what is now selected:
    Drupal.dirtDataTable.toggleSearchType();

    // Since the search was reset and the default is to search on all
    // sites/all habitats, shouldn't actually need to apply any filter here
    // as we did before, should be good to go.
  };

  /**
   * Handles searching on a single Site ID when triggered from something other
   * than simply entering a single Site ID into the Site ID filter (e.g.,
   * initiate search from map marker).
   * It is assumed that the search type has already been set to search by Site
   * ID before this function is called, so all we need to do is update the Site
   * ID filter to reflect the Site ID we're searching on, and then trigger the
   * Site ID search.
   */
  Drupal.dirtDataTable.searchSingleSiteID = function (siteId) {
    // Clear out Site ID filter first
    $('#site-id-select option:selected').prop("selected", false);

    // Set the Site ID filter to show this Site ID
    $('#site-id-select option[value=' + siteId + ']').prop("selected", true);

    // Redo Site ID search
    $('#site-id-select').trigger('chosen:updated'); // Updates filter GUI element
    Drupal.dirtDataTable.triggerTableFilter("site-id"); // Runs Site ID search
  };

  /**
   * Displays the table columns with the class colClass given as input.
   */
  Drupal.dirtDataTable.showSurveyCols = function (colClass) {
    table.columns('th.var-filter-' + colClass).visible(true, false);
  };

  /**
   * Hides the table columns with the class colClass given as input.
   */
  Drupal.dirtDataTable.hideSurveyCols = function (colClass) {
    table.columns('th.var-filter-' + colClass).visible(false, false);
  };

  /**
   * Sets all table columns to be visible.
   */
  Drupal.dirtDataTable.showAllSurveyCols = function () {
    table.columns().visible(true, false);
  };

  /**
   * Hides all survey-related data columns (leaving common columns, such as
   * Site ID).
   */
  Drupal.dirtDataTable.hideAllSurveyCols = function () {
    // We don't want to hide ALL cols, just the survey ones
    var i;
    for (i=1; i<surveyColGroups.length; i++) {
      Drupal.dirtDataTable.hideSurveyCols(surveyColGroups[i]);
    }
  };

  /**
   * Callback function when check all / uncheck all links are clicked
   *
   * Input:
   * type = filter type whose check/uncheck all link was clicked
   * checked = true if check all was clicked on, false if uncheck all
   */
  Drupal.dirtDataTable.toggleCheckAll = function (type, checked) {
    // First check or uncheck the checkboxes of filter type according to
    // "checked" boolean value
    $('#' + type + '-checkboxes-container' + ' input[type=checkbox]').prop("checked", checked);

    if (type == 'variable') {
      // Redo variable checkbox search
      Drupal.dirtDataTable.triggerTableFilter("variable-checkboxes");
    }
  };

  /**
   * Checks if siteId passed in as a param (from URL param) is a valid Site ID,
   * and if so, triggers a search on this single ID.
   * Otherwise, does nothing, which should default to showing all sites (as
   * this function is only called after the search has been reset).
   */
  Drupal.dirtDataTable.doSiteIdParamSearch = function (siteId) {
    // Check if siteId was passed in to default to, and if so, make sure it's a
    // valid Site ID
    var siteIds = $("#site-id-select option").map(function(){
      return $(this).val();
    }).get();

    if ((typeof siteId !== 'undefined') && ($.inArray(siteId, siteIds) !== -1)) {
      // Only search on Site ID with value equal to siteId passed in
      Drupal.dirtDataTable.searchSingleSiteID(siteId);
    }
  };

  /**
   * Gets the URL parameter (site ID).
   */
  Drupal.dirtDataTable.getQueryParameter = function (param) {
    var values = [];
    var params; 
    var searchStr = window.location.search.substring(1);
    var searchVars = searchStr.split('&');

    var i;
    for (i = 0; i < searchVars.length; i++) {
      params = searchVars[i].split('=');
      if (Drupal.checkPlain(params[0]) == param) {
        return Drupal.checkPlain(params[1]);
      }
    }

    return null;
  };

  /**
   * Clears any parameters in the URL.
   */
  Drupal.dirtDataTable.updateWindowURL = function () {
    var pagePath = Drupal.settings.dirt_datatable.page_path;
    window.history.replaceState({}, document.title, pagePath);
  };


  /*-----------------------------------------------------------*/
  /* Table filter functions - called from triggerTableFilter   */
  /* (Note all of these assume table object exists already)    */
  /*-----------------------------------------------------------*/

  /**
   * Applies the variable checkbox filter.
   * Updates both table columns (variables shown) and rows (by survey type).
   */
  Drupal.dirtDataTable.toggleVariableCheckboxSearch = function () {
    // First filter the columns according to the selected checkboxes
    var i;
    for (i=1; i<surveyColGroups.length; i++) {
      if ( $('#variable-checkboxes-container input[type=checkbox][value=' + surveyColGroups[i] + ']').is(':checked') ) {
        Drupal.dirtDataTable.showSurveyCols(surveyColGroups[i]);
      }
      else {
        Drupal.dirtDataTable.hideSurveyCols(surveyColGroups[i]);
      }
    }

    // Additionally, filter rows according to survey type
    // (e.g., don't show monthly surveys if no monthly variables are selected to be shown)
    Drupal.dirtDataTable.toggleTableRowsBySurveyType();
  };

 /**
  * Checks which variables are selected and returns an array of survey types
  * corresponding to those variables (when they're collected).
  *
  * The returned array contains some subset of the numbers 1-12 (# times per
  * year collected) or the string 'one-time' to indicate that the variable is
  * collected once per site.
  */
  Drupal.dirtDataTable.getSurveyTypesFromVariables = function () {
    var surveyTypes = [];  // initialize return array

    // If in desktop mode, get the variables from checkboxes. Otherwise, get variables from dropdown.
    if (window.innerWidth > MAX_TABLET_WIDTH) {

      $('#variable-filter input[type=checkbox]').each(function () {
        if ($(this).is(':checked')) {
          var checkboxClass = this.className.match(/survey-type-([a-z_]+)/);
          if (checkboxClass) {
            surveyTypes.push(checkboxClass[1]);
          }
        }
      });
    }
    else {
      var dropDownVal = $('#variable-dropdown-container select').val();
      $('#variable-filter input[type=checkbox]').each(function () {
        var checkboxClass = this.className.match(/survey-type-([a-z_]+)/);
        var className = checkboxClass[1];
        var checkboxVal = $(this).val();
        if (checkboxVal == dropDownVal) {
          surveyTypes.push(checkboxClass[1]);
        }
      });
    }

    return surveyTypes;
  };

  /**
   * Shows table rows with survey types matching variables checked and hides all
   * other rows.
   */
  Drupal.dirtDataTable.toggleTableRowsBySurveyType = function () {
    var searchTerms = Drupal.dirtDataTable.getSurveyTypesFromVariables();

    // Build search term
    var searchTerm = '';
    var numTerms = searchTerms.length;

    if (numTerms == 1) {
      searchTerm = searchTerms[0];
    }
    else if (numTerms > 1) {
      searchTerm = '(' + searchTerms[0];
      var i;
      for (i=1; i<numTerms; i++) {
        searchTerm += '|';
        searchTerm += searchTerms[i];
      }
      searchTerm += ')';
    }

    // Toggle row visibility in table

    table.columns('.row-types').search(searchTerm,  true, false, true).draw();
  };

  /**
   * Controls the visibility of each column group based on select dropdown
   * (mobile and tablet widths).
   */
  Drupal.dirtDataTable.toggleTableColsSelect = function () {
    var dropDownVal = $('#variable-dropdown-container select').val();

    $('#variable-dropdown-container select option').each(function() {
      if (this.value == dropDownVal) {
        Drupal.dirtDataTable.showSurveyCols(this.value);
      }
      else {
        Drupal.dirtDataTable.hideSurveyCols(this.value);
      }
    });

    // Additionally, auto search collection periods
    Drupal.dirtDataTable.toggleTableRowsBySurveyType();
  };

  /**
   * Controls the visibility of the survey links by type.
   */
  Drupal.dirtDataTable.toggleSurveyLinks = function () {
    // Get survey types from the variables selected
    var searchTerms = Drupal.dirtDataTable.getSurveyTypesFromVariables();

    // Initially hide all survey links
    Drupal.dirtDataTable.hideAllSurveyLinks();

    $.each(searchTerms, function(idx, term) {
      $('span.link-type-' + term).show();
    });
  };

  /**
   * Shows all survey links in the (admin-only) survey links column.
   */
  Drupal.dirtDataTable.showAllSurveyLinks = function () {
    $('#dirt-data-table td.survey-link span').show();
  };

  /**
   * Hides all survey links in the (admin-only) survey links column.
   */
  Drupal.dirtDataTable.hideAllSurveyLinks = function () {
     $('#dirt-data-table td.survey-link span').hide();
  };

  /**
   * Applies the habitat type select (dropdown) filter to the table.
   */
  Drupal.dirtDataTable.toggleHabitatTypeSelect = function () {
    var searchTerm = '';
    var dropDownVal = $('#habitat-type-dropdown-container select').val();

    if (dropDownVal == 'All Habitats') {
      // Search all habitat types
      searchTerm += '(';
      prevChecked = false;
      $('#habitat-type-dropdown-container select option').each(function() {
        if (prevChecked) {
          searchTerm += '|';
        }
        searchTerm += this.value;
        prevChecked = true;
      });
      searchTerm += ')';
    }
    else {
      searchTerm = dropDownVal;
    }

    table.columns('.dirt-site-habitat-type').search(searchTerm,  true, false, true).draw();
  };

  /**
   * Applies a chosen select search to the table with the selector ID given as
   * input. Also the table column that the filter is to be applied to is given
   * as input colClass.
   */
  Drupal.dirtDataTable.toggleChosenSearch = function (id, colClass) {
    var searchTerm = '';
    var searchValues = $('#' + id).val();

    var numValues = 0;
    if (searchValues != null) {
      numValues = searchValues.length;
    }

    if (numValues == 1) {
      searchTerm = '^' + searchValues[0] + '$';
    }
    else if (numValues > 1) {
      searchTerm = '(^' + searchValues[0] + '$';

      for (i=1; i<numValues;++i) {
        // Build search term
        searchTerm += '|';
        searchTerm += '^' + searchValues[i] + '$';
      }
      searchTerm += ')';
    }

    // Otherwise, filter is empty, so search on all values (leave searchTerm blank)
    // Update table
    table.columns('.' + colClass).search(searchTerm,  true, false, true).draw();
  };

  /**
   * Applies the habitat type chosen select search to the table.
   * (Special case of the toggleChosenSearch() function because the "Other"
   * habitat type case must be handled separately.)
   */
  Drupal.dirtDataTable.toggleHabitatChosenSearch = function () {
    var searchTerm = '';

    var habitatTypes = $('#habitat-type-select').val();
    var numHabitats = 0;
    if (habitatTypes != null) {
      numHabitats = habitatTypes.length;
    }

    if (numHabitats > 0) {
      searchTerm = '(^' + habitatTypes[0];

      if (habitatTypes[0] !== 'Other') {
        searchTerm += '$';
      }

      var i;
      for (i=1; i<numHabitats; ++i) {
        // Build search term
        searchTerm += '|';
        searchTerm += '^' + habitatTypes[i];
        if (habitatTypes[i] !== 'Other') {
          searchTerm += '$';
        }
      }

      searchTerm += ')';
    }

    // Otherwise, filter is empty, so search on all values (leave searchTerm
    // blank)
    // Update table
    table.columns('.dirt-site-habitat-type').search(searchTerm,  true, false, true).draw();
  };

  /**
   * Applies the Site ID chosen select search to the table.
   * (Special case of the toggleChosenSearch() function because the global Site
   * ID search must be handled too.)
   */
  Drupal.dirtDataTable.toggleSiteIDChosenSearch = function () {
    var searchTerm = '';

    // Clear out single site search ID
    onlySiteSearchID = '';

    var siteIds = $('#site-id-select').val();
    var numIds = 0;
    if (siteIds != null) {
      numIds = siteIds.length;
    }

    if (numIds == 1) {
      // Searching on one site ID only, update global
      onlySiteSearchID = siteIds[0];
      searchTerm = '^' + siteIds[0] + '$';
    }
    else if (numIds > 1) {
      searchTerm = '(^' + siteIds[0] + '$';

      for (i=1; i<numIds;++i) {
        // Build search term
        searchTerm += '|';
        searchTerm += '^' + siteIds[i] + '$';
      }

      searchTerm += ')';
    }

    // Otherwise, filter is empty, so search on all sites (leave searchTerm
    // blank)
    // Update table
    table.columns('.site-id').search(searchTerm,  true, false, true).draw();
  };

  /**
   * Applies the selected date range search to the table.
   */
  Drupal.dirtDataTable.searchByDateRange = function () {
    var startYear  = $('#date-range-filter select#date-start-year').val();
    var startMonth = $('#date-range-filter select#date-start-month').val();
    var endYear    = $('#date-range-filter select#date-end-year').val();
    var endMonth   = $('#date-range-filter select#date-end-month').val();

    var startEmpty = (!startYear && !startMonth);
    var endEmpty = (!endYear && !endMonth);

    var startDateStr = '';
    var endDateStr = '';
    var monthSearch = '';

    var errorsPresent = false;

    // Behavior:
    // If month selected and no year, show surveys from only that month
    // If month+year selected, show surveys after that year
    // Similar to end date, but if only start and end months selected, display
    // warning

    if (startYear) {
      // Create start date string:
      startDateStr = startYear;

      if (startMonth) {
        var startMonthInt = parseInt(startMonth, 10);
        startMonthInt += 1;
        startDateStr = startDateStr + '-' + startMonthInt;
      }
    }
    else if (startMonth) {
      // No start year entered. If no end date entered, search on month only.
      if (endEmpty) {
        monthSearch = startMonth;
      }
      else {
        // Need to enter a start year
        Drupal.dirtDataTable.displayWarning('Enter a start year.');
        Drupal.dirtDataTable.disableNonDateFilters();
        errorsPresent = true;
      }
    }

    if (endYear) {
      // Create end date string:
      endDateStr = endYear;

      if (endMonth) {
        var endMonthInt = parseInt(endMonth, 10);
        endMonthInt += 1;
        endDateStr = endDateStr + '-' + endMonthInt;
      }
    }
    else if (endMonth) {
      // No end year entered. If no start date entered, search on month only.
      if (startEmpty) {
        monthSearch = endMonth;
      }
      else {
        // Need to enter an end year
        Drupal.dirtDataTable.displayWarning('Enter an end year.');
        Drupal.dirtDataTable.disableNonDateFilters();
        errorsPresent = true;
      }
    }

    // Verify that start date <= end date
    if ( startYear && endYear && (Drupal.dirtDataTable.parseDate(startDateStr, 0) > Drupal.dirtDataTable.parseDate(endDateStr, 1)) ) {
      errorsPresent = true;
      Drupal.dirtDataTable.displayWarning('Start date cannot be after end date. Enter different dates.');
      Drupal.dirtDataTable.disableNonDateFilters();
    }

    if (!errorsPresent) {
      // Clear the last search, if there is one
      if ($.fn.dataTableExt.afnFiltering.length > 0) {
        $.fn.dataTableExt.afnFiltering.pop();
      }
      // Add this search
      $.fn.dataTableExt.afnFiltering.push(
        function(oSettings, aData, iDataIndex) {
          var surveyDate = Drupal.dirtDataTable.parseDate(aData[dateColIdx], 0);

          if (monthSearch) {
            // Just search on month (already zero-indexed)
            if (surveyDate.getUTCMonth() == monthSearch) {
              return true;
            }
            else {
              return false;
            }
          }
          else if (startDateStr && endDateStr) {
            // Search on date range
            if ( (surveyDate >= Drupal.dirtDataTable.parseDate(startDateStr, 0)) && (surveyDate <= Drupal.dirtDataTable.parseDate(endDateStr, 1)) ) {
              return true;
            }
            else {
              return false;
            }
          }
          else if (startDateStr) {
            // Search on start date only
            if (surveyDate >= Drupal.dirtDataTable.parseDate(startDateStr, 0)) {
              return true;
            }
            else {
              return false;
            }
          }
          else if (endDateStr) {
            // Search on end date only
            if (surveyDate <= Drupal.dirtDataTable.parseDate(endDateStr, 1)) {
              return true;
            }
            else {
              return false;
            }
          }
          else {
            // Nothing to search on, so return true
            return true;
          }
        }
      );

      table.draw();
    }

    // The filter is valid if there are no errors present, so return the
    // negation of that boolean.
    return !(errorsPresent);
  };

  /**
   * Helper function to parse dates in form YYYY or YYYY-MM.
   *
   * Input:
   * typeIdx: 0 or 1
   * if 0, create date at start of the month
   * if 1, create date at end of the month
   */
  Drupal.dirtDataTable.parseDate = function (dateStr, typeIdx) {
    var dateObj = new Date();

    if (typeIdx == 0) {
      dateObj.setUTCMonth(0, 1);
      dateObj.setUTCHours(6, 0, 0, 0);
      dateObj.setUTCFullYear(2015); // default start year to 2015
    }
    else {
      dateObj.setUTCHours(23, 59, 59, 999);
      dateObj.setUTCMonth(11, 28);
      dateObj.setUTCFullYear(2050); // default end year sometime way in future
    }

    // Set actual end year if nonempty string
    if (dateStr) {
      var dateArr = dateStr.split('-');
      dateObj.setUTCFullYear(dateArr[0]);

      if (dateArr.length > 1) {
        dateArr[1] = parseInt(dateArr[1], 10);
        dateArr[1] -= 1;  // months are zero-indexed
        dateObj.setUTCMonth(dateArr[1]);
      }
    }

    return dateObj;
  };


  /*-----------------------------------------------------------*/
  /* Functions to create table and load table with data        */
  /*-----------------------------------------------------------*/

  /**
   * Loads the table column data from JSON source.
   */
  Drupal.dirtDataTable.getTableColumnData = function () {
    $.getJSON(DIRT_DATATABLE_METADATA_URL, {
      format: "json"
    })
      .done(function(data) {
        // Create data table from column data
        tableCols = data;
        Drupal.dirtDataTable.createDataTable();
      })
      .fail(function() {
        Drupal.dirtDataTable.tableLoadError();
      }); 
  };

  /**
   * Displays an error message if table data cannot be loaded for some reason.
   */
  Drupal.dirtDataTable.tableLoadError = function () {
    // Fail as gracefully as possible... hide all table elements and report error
    var errorMsg = '<p>Table cannot be loaded at this time. Contact the site administrator.</p>';
    $('#table-and-filters-container').html(errorMsg);
  };

  /**
   * Creates the data table depending on the current browser window
   * width (mobile/tablet vs. desktop).
   */
  Drupal.dirtDataTable.createDataTable = function () {
    // Change "processing" message to "loading data" to better explain delay
    $.extend( $.fn.dataTable.defaults, {
      language: {
        "processing": "Loading data "
      },
    });

    if (window.innerWidth > MAX_TABLET_WIDTH) {
      Drupal.dirtDataTable.createDataTableLarge();
    }
    else {
      Drupal.dirtDataTable.createDataTableSmall();
    }

    // Initially hide the empty table message while data is loading.
    // Since the element is redrawn each time, this will only hide it
    // during the initial load. If the table really is empty, the
    // message will still be shown after the initial default filters
    // are applied.
    $('.empty-table-message').hide();

    Drupal.dirtDataTable.animateLoadScreen(0);
  };

  /**
   * Animates "loading" message
   */
  Drupal.dirtDataTable.animateLoadScreen = function (idx) {
    $('#dirt-data-table_processing').append(' . ');
    setTimeout(function() {
      if (idx < 8) {
        Drupal.dirtDataTable.animateLoadScreen(idx+1);
      }
    }, 400);
  }

  /**
   * Creates the large version of the data table for desktop/tablet widths.
   * It does use fixedColumn to fix the first column (Site ID) when scrolling
   * horizontally.
   */
  Drupal.dirtDataTable.createDataTableLarge = function () {
    // Default to 25 rows per page
    var pageLength = 25;

    // Set table height based on window height
    //var tableHeight = window.innerHeight - 300;
    var tableHeight = window.innerHeight - 320;

    // Initialize DataTable
    $('#dirt-data-table').DataTable( {
      dom: 'Brtlip',
      "ajax": {
        'url': DIRT_DATATABLE_SOURCE_URL,
      },
      "columns": tableCols,
      "createdRow": function(row, data, dataIndex) {
         $(row).addClass(($(data)[0])['row_types']);
       },
       buttons: [
         {
           extend: 'csv',
           exportOptions: {
             columns: ':visible',
           }
         },
         {
           extend: 'excelHtml5',
           exportOptions: {
             columns: ':visible',
           }
         }
     ],
     "scrollX": true,
     "scrollY": tableHeight,
     "processing": true,
     "fixedColumns": {
       leftColumns: 1,
       heightMatch: 'none'
     },
     "lengthMenu": [ [10, 25, 50, 100, 200, -1], [10, 25, 50, 100, 200, "All"] ],
     "pageLength": pageLength,
     "search": {
       "regex": true
      },
      "language": {
        "zeroRecords": "<span class='empty-table-message'>No matching records found.</span>"
      },
     "initComplete": function(settings, json) {
        Drupal.dirtDataTable.tableLoaded();
      }
    });
  };

  /**
   * Creates the small version of the data table for mobile and tablet browser
   * widths. Does not use fixedColumn since that doesn't work well on anything
   * other than desktop width.
   */
  Drupal.dirtDataTable.createDataTableSmall = function () {
    var tableHeight = window.innerHeight - 320;

    // Initialize DataTable
    $('#dirt-data-table').DataTable( {
      dom: 'rtip',
      'ajax': {
        'url': DIRT_DATATABLE_SOURCE_URL,
      },
      'columns': tableCols,
      'createdRow': function(row, data, dataIndex) {
        $(row).addClass(($(data)[0])['row_types']);
      },
      "scrollX": true,
      "scrollY": tableHeight,
      "ordering": false,
      "processing": true,
      "pageLength": 10,
      "search": {
        "regex": true
      },
      "language": {
        "zeroRecords": "<span class='empty-table-message'>No matching records found.</span>"
      },
      "initComplete": function(settings, json) {
        Drupal.dirtDataTable.tableLoaded();
      }
    });
  };

  /**
   * Callback function once the table has been loaded.
   *
   * Once the table has been loaded, we can apply the default filters.
   * Note we need to call this here and not when page is first loaded because
   * we need the table object to exist before applying filters.
   */
  Drupal.dirtDataTable.tableLoaded = function () {
    var headerClasses = [];
    var firstClass = '';
    var i;

    // Store data table object
    table = $('#dirt-data-table').DataTable();

    // ------------------------------------------
    // TOO SLOW on large tables - commenting out:
    // ------------------------------------------
    // Propagate header classes to their cells
    //Drupal.dirtDataTable.propagateHeaderClasses();
    // Add table page change handler
    //table.on('page.dt', function () {
    //  // Propagate header classes to their cells
    //  Drupal.dirtDataTable.propagateHeaderClasses();
    //});

    // Find index of survey date column and store it
    dateColIdx = table.column('.survey-date-year-month', ).index();

    // Set the default filters and apply them to the table
    Drupal.dirtDataTable.resetSearch(true);

    // If a particular Site ID was passed in as a URL parameter, search on this Site ID only:
    var siteId = Drupal.dirtDataTable.getQueryParameter('id');
    Drupal.dirtDataTable.updateWindowURL();

    Drupal.dirtDataTable.doSiteIdParamSearch(siteId);

    // After table drawn, add handler for mobile table draw event
    if (window.innerWidth <= MAX_MOBILE_WIDTH) {
      // Call once now after table created
      Drupal.dirtDataTable.setTableHeaderLabels();

      // Add event handler for future table draws:
      table.on('draw.dt', function (e) {
        e.preventDefault();
        // Set mobile-friendly table header labels
        Drupal.dirtDataTable.setTableHeaderLabels();
      });
    }
  };

  /**
   * Propagates table header classes to children.
   */
  Drupal.dirtDataTable.propagateHeaderClasses = function () {
    $('td').each(function(idx) {
      if ($(this).prop('colspan') <= 1) {
        // First class (that isn't 'admin-only' or 'column-hidden') is the
        // column name and can be used to identify all cells belonging to this
        // column:
        firstClass = ($(this).attr('class')).trim().split(' ')[0];
        if ((firstClass != 'admin-only') && (firstClass != 'column-hidden')) {
          headerClasses = ($('th.' + firstClass).attr('class')).trim().split(' ');
          for(i = 0; i < headerClasses.length; ++i) {
            // No need to propagate 'sorting' class as that's only for <th> elements
            if ((headerClasses[i] != '') && !(headerClasses[i].includes('sorting'))) {
              $(this).addClass(headerClasses[i]);
            }
          }
        }
      }
    });
  }

  /**
   * Reformats the mobile-friendly table to display the column header to the
   * left of the column value itself (since the header row is hidden).
   * Gives the table a vertical orientation rather than the usual horizontal.
   */
  Drupal.dirtDataTable.setTableHeaderLabels = function () {
    var colLabels = [];

    $('#dirt-data-table th').each(function () {
      if ($(this).prop('colspan') <= 1) {
        colLabels.push($(this).text());
      }
    });

    var idx;
    var colValue = '';
    for (idx = 1; idx < colLabels.length; ++idx) {
      $('#dirt-data-table td:nth-of-type(' + idx + ')').each(function () {
        // Grab the column value
        colValue = $(this).text();

        // In case called multiple times:
        if($(this).find('.table-right-col').length) {
          colValue = $(this).find('.table-right-col').text();
        }

        // Clear the column
        $(this).empty();
     
        // Add left and right divs for column label : column value in same line
        $(this).append('<div class="table-left-col">' + colLabels[idx-1] + ': ' + '</div>');
        $(this).append('<div class="table-right-col">' + colValue + '</div>');
      });
    }
  };


  /*-----------------------------------------------------------*/
  /* Functions to handle map and map markers                   */
  /*-----------------------------------------------------------*/

  /**
   * Adds the map (but does not place any markers at this time).
   */
  Drupal.dirtDataTable.initMap = function () {
    // Check if map div exists first
    if ($('#map-placement').length > 0) {
      var latValue = parseFloat(Drupal.settings.dirt_datatable.lat);
      var lonValue = parseFloat(Drupal.settings.dirt_datatable.lon);
      var zoomLevel = parseInt(Drupal.settings.dirt_datatable.zoom, 10);

      var mapCenter = new google.maps.LatLng(latValue, lonValue);

      map = new google.maps.Map(
        document.getElementById('map-placement'), {
          zoom: zoomLevel,
          minZoom: 2,
          center: mapCenter,
          streetViewControl: false,
          fullscreenControl: false,
          scrollwheel: false
        }
      );
    }
  };

  /**
   * Gets the map marker info from table source JSON and adds map markers, one
   * for each site. Also adds an info window for each marker and click handlers
   * for the info window popups.
   */
  Drupal.dirtDataTable.addMapMarkers = function () {
    if (map != '') {
      $.getJSON(DIRT_DATATABLE_SOURCE_URL, function(data) {
        var latitude;
        var longitude;
        var habitatType;
        var county;
        var state;
        var zipCode;

        // Loop through each table row and add a marker for each Site ID
        $.each(data['data'], function(idx, obj) {
          siteId = obj.site_id;

          // Only process this Site ID if we haven't seen it already
          if($.inArray(siteId, mapMarkerIDs) == -1) {
            latitude = obj.dirt_site_geo_coords_lat;
            longitude = obj.dirt_site_geo_coords_lon;

            habitatType = obj.dirt_site_habitat_type;
            county = obj.dirt_site_county;
            state = obj.dirt_site_state;
            zipCode = obj.dirt_site_zip_code;

            if (latitude != null && longitude != null && siteId != null) {
              // Add marker
              var marker = new google.maps.Marker({position: {lat: parseFloat(latitude), lng: parseFloat(longitude)}, map: map, clickable: true});

              // Add infowindow
              var infoMarkup = '<div class="map-marker-info">';
              infoMarkup += '<h4><b>Site Information</b></h4>';
              infoMarkup += '<b>Site ID</b>: ' + siteId + '<br><br>';

              if (habitatType != null && habitatType != '') {
                infoMarkup += '<b>Habitat Type</b>: ' + habitatType + '<br><br>';
              }

              if (county != null && state != null && zipCode != null) {
                infoMarkup += '<b>Location</b>: ' + county + ' County, ' + state + ', ' + zipCode + '<br><br>';
              }

              // TODO: habitat photos
              infoMarkup += '<div class="map-search-by-uid-container">';
              infoMarkup += '<span class="map-search-by-uid-status">Searching on this site only</span><br>';
              infoMarkup += '<a class="map-search-by-uid-link" href="#' + siteId + '">Search on this Site ID only</a>';
              infoMarkup += '</div>';

              marker.info = new google.maps.InfoWindow({
                content: infoMarkup
              });

              // Add onclick listener for markers
              google.maps.event.addListener(marker, 'click', function() {
                // Close previously opened InfoWindow
                if (prevInfoWindow) {
                  prevInfoWindow.close();
                }
                prevInfoWindow = marker.info;
                // Open this InfoWindow
                marker.info.open(map, marker);
              });

              // Add domready listener to info window
              google.maps.event.addListener(marker.info, 'domready', function() {
                var siteIdLink = $('a.map-search-by-uid-link');
                var siteId = siteIdLink.attr('href').split('#')[1];
                var siteIdStatus = $('span.map-search-by-uid-status');

                // Initially hide included in search status and hide "search on
                // this site" link
                siteIdStatus.hide();
                siteIdLink.hide();

                // Show Site ID search option only when Search by Site ID
                // currently selected
                if ($('#search-type-radios input[type=radio]:checked').val() == 'site-id') {
                  // Additionally check if we're searching on this site only - if
                  // so, hide "search this site only" link and show "Included in
                  // site" status
                  if (onlySiteSearchID == siteId) {
                    siteIdStatus.show();
                    siteIdLink.hide();
                  }
                  else {
                    // Otherwise leave status hidden and show search link
                    siteIdLink.show();
                  }
                }
              });

              mapMarkers.push(marker);
              mapMarkerIDs.push(siteId);
            } //  end if valid geo coord and site id
          } // end if in array
        }); // end each
      }); // end get json

      // Add handler for each "search for this site only" info window link (dynamic elements)
      $('#map-search').on('click', '.map-search-by-uid-link', function(e) {
        // Close the info window otherwise stuff gets messed up
        if (prevInfoWindow) {
          prevInfoWindow.close();
        }
        var siteId = $(this).attr('href').split('#')[1];

        // Now run search on this site ID only
        Drupal.dirtDataTable.searchSingleSiteID(siteId);

        e.preventDefault();
        return false;
      });
    } // end if
  };

  /**
   * Sets all of the map markers to be visible.
   */
  Drupal.dirtDataTable.showAllMapMarkers = function () {
    var i;
    for (i=0; i<mapMarkers.length; i++) {
       mapMarkers[i].setMap(map);
    }
  };

  /**
   * Sets the map marker corresponding to the siteId passed in as param to be
   * visible.
   */
  Drupal.dirtDataTable.showMapMarker = function (siteId) {
    var siteIdIdx = mapMarkerIDs.indexOf(siteId);
    if (siteIdIdx != -1) {
      mapMarkers[siteIdIdx].setMap(map);
    }
  };

  /**
   * Shows only map markers corresponding to Site IDs present in the data table.
   */
  Drupal.dirtDataTable.showFilteredMapMarkers = function () {
    // First hide all markers so we can only display the filtered ones
    Drupal.dirtDataTable.hideAllMapMarkers();

    // Now get all of the Site IDs remaining in the table (in column 0) after the
    // filter has been applied, and show those markers (leaving the rest hidden).
    table.column(0, { search:'applied' } ).data().unique().each(function(value, index) {
      Drupal.dirtDataTable.showMapMarker(value);
    });
  };

  /**
   * Hides all of the markers on the map.
   */
  Drupal.dirtDataTable.hideAllMapMarkers = function () {
    var i;
    for (i=0; i<mapMarkers.length; i++) {
      mapMarkers[i].setMap(null);
    }
  };


  /*-----------------------------------------------------------*/
  /* Functions to get object values for use in other modules.  */
  /*-----------------------------------------------------------*/

  /**
   * Returns data table object.
   */
  Drupal.behaviors.dirtDatatable.getDataTable = function () {
    return table;
  };

  /**
   * Returns map object.
   */
  Drupal.behaviors.dirtDatatable.getMap = function () {
    return map;
  };

  /**
   * Returns array of map marker objects.
   */
  Drupal.behaviors.dirtDatatable.getMapMarkers = function () {
    return mapMarkers;
  };

  /**
   * Returns array of Site IDs corresponding to map markers.
   */
  Drupal.behaviors.dirtDatatable.getMapMarkerIDs = function () {
    return mapMarkerIDs;
  };

})(jQuery);
