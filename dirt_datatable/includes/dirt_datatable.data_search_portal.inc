<?php


/**
 * @file
 * This file specifies functions for the data search portal
 */


/**
 * Page callback for main data search portal page
 *
 * @retval Render array.
 */
function dirt_datatable_page() {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  // Set the page title
  $title = t('Data Search Portal');
  drupal_set_title($title);

  // Add necessary CSS and JS files
  drupal_add_css('https://cdn.datatables.net/1.10.14/css/jquery.dataTables.min.css', 'external');
  drupal_add_css('https://cdn.datatables.net/buttons/1.2.4/css/buttons.dataTables.min.css', 'external');
  drupal_add_css('https://cdn.datatables.net/fixedcolumns/3.2.3/css/fixedColumns.dataTables.min.css', 'external');
  drupal_add_css(drupal_get_path('module', 'dirt_datatable') . '/css/dirt_datatable.css');
  drupal_add_js(drupal_get_path('module', 'dirt_datatable') . '/js/dirt_datatable.js');
  drupal_add_js('https://cdn.datatables.net/1.10.15/js/jquery.dataTables.min.js', 'external');
  drupal_add_js('https://cdn.datatables.net/buttons/1.3.1/js/dataTables.buttons.min.js', 'external');
  drupal_add_js('https://cdn.datatables.net/buttons/1.3.1/js/buttons.flash.min.js', 'external');
  drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/jszip/3.1.3/jszip.min.js', 'external');
  drupal_add_js('https://cdn.datatables.net/buttons/1.3.1/js/buttons.html5.min.js', 'external');
  drupal_add_js('https://cdn.datatables.net/fixedcolumns/3.2.3/js/dataTables.fixedColumns.min.js', 'external');

  if (variable_get('dirt_datatable_config_display_map', '')) {  
    $options = array(
      'type' => 'external',
      'weight' => 99,
      'defer' => TRUE,
    );
    // Add defaults in case something went wrong and these aren't set,
    // because JS expects integer values.
    $lat = variable_get('dirt_datatable_config_map_lat', '39.8283');
    $lon = variable_get('dirt_datatable_config_map_lon', '-98.5795');
    $zoom = variable_get('dirt_datatable_config_map_zoom', '5');

    // API key should be set if display_map is true
    $api_key = variable_get('dirt_datatable_config_map_api_key', '');

    drupal_add_js('https://maps.googleapis.com/maps/api/js?key=' . $api_key, $options);
    drupal_add_js(array('dirt_datatable' => array('lat' => $lat, 'lon' => $lon, 'zoom' => $zoom)), 
      array('type' => 'setting'));
  }

  // Search portal page path including base_url (so relative to "/")
  global $base_path; // includes trailing "/"
  $page_path = $base_path . variable_get('dirt_data_search_portal_path', DIRT_DATA_SEARCH_PATH_DEFAULT);
  drupal_add_js(array('dirt_datatable' => array('page_path' => $page_path)),
    array('type' => 'setting'));

  // Add ChosenJS library files
  $library = libraries_detect('chosen');
  if ($library && !empty($library['installed'])) {
    libraries_load_files($library);
  }
  else {
    $status_link = l(t('Status report'), 'admin/reports/status');
    drupal_set_message(t('Warning: Chosen jQuery plugin not found. See @status for more details.',
      array('@status' => $status_link)), 'warning');

    watchdog('dirt_datatable', 'Error: Chosen jQuery plugin  not found. See %status for more details.',
      array('%status' => $status_link), WATCHDOG_ERROR);
  }

  $build = array();

  // Load the data search portal overview
  dirt_datatable_add_data_search_overview($build);

  // Add map search if applicable
  if (variable_get('dirt_datatable_config_display_map', '')) 
    dirt_datatable_add_map_search($build);

  // Load the table and filters
  dirt_datatable_add_table_and_filters($build);

  // Override menu breadcrumbs
  // This should be the last thing to do because some of the embedded nodes may
  // change the breadcrumb. By doing this last, we override any other changes.
  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Citizen Science Soil Health Surveys'), $citizen_sci_path);
  $breadcrumb[] = $title;
  drupal_set_breadcrumb($breadcrumb);
  unset($breadcrumb);

  return $build;
}


/**
 * Adds the data search overview node to the data search
 * portal page.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_data_search_overview(&$build) {
  // Create fieldset container

  $build['overview'] = array(
    '#type' => 'fieldset',
    '#title' => t('About this Data'),
    '#attributes' => array(
      'class' => array('open'),
      'id' => array('about-this-data'),
    ),
  );

  // Add block for user login (if not already logged in) 
  // and data access request message:

  // Initialize already logged-in markup (will remain empty if not logged in)
  $user_logged_in_markup = '';

  // Add login block if user isn't logged in already
  $login_block = module_invoke('user', 'block_view', 'login');

  $user_logged_in = (empty($login_block));
  $block_class = ($user_logged_in) ? 'logged-in' : 'not-logged-in';

  // Add block container
  $build['overview']['block_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'about-login',
      'class' => array($block_class),
    ),
  );

  if (!($user_logged_in)) {
    // User is not logged in:

    // Add container for login header and login form
    $build['overview']['block_container']['login_container'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('login-item'),
      ),
    );

    // Add block header
    $build['overview']['block_container']['login_container']['header'] = array(
      '#type' => 'markup',
      '#markup' => t('Participant login'),
      '#prefix' => '<h4 class="login-header">',
      '#suffix' => '</h4>',
    );

    // Add block renderable content
    // Clear the create new account/request new password links
    $login_block['content']['links']['#markup'] = '';
    $build['overview']['block_container']['login_container']['login'] = $login_block['content'];
  }
  else {
    // User is logged in:

    global $user;
    $user_logged_in_markup = t('Welcome, @name', array('@name' => $user->name));
  }

  // Add welcome (if logged in) or data access request message if not
  // (requests allowed or disallowed via admin config settings)
  $data_access_requests_allowed = variable_get('dirt_datatable_config_access_requests_allowed', 0);
  if ($user_logged_in_markup || $data_access_requests_allowed) {
    // Open fieldset
    $build['overview']['block_container']['data_access'] = array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('open', 'login-item', 'data-access'),
      ),
    );

    if ($user_logged_in_markup) {
      // Add welcome message
      $build['overview']['block_container']['data_access']['welcome'] = array(
        '#type' => 'markup',
        '#markup' => $user_logged_in_markup,
        '#prefix' => '<div id="welcome-username">',
        '#suffix' => '</div>',
      );
    }
    else {
      // Otherwise add data request message
      $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);

      $build['overview']['block_container']['data_access']['request_link'] = array(
        '#type' => 'markup',
        '#markup' => t('Scientists may request full access to the data by registering') .
                       ' ' . l(t('here'), $citizen_sci_path . '/account-request-form',
                       array('attributes' => array(
                         'target'=>'_blank',
                         'title'=>t('Request a data viewing account'),
                       ))
                     ) . '.',
      );
    }
  }

  // Get overview node from its path
  $search_data_path = variable_get('dirt_data_search_portal_path', DIRT_DATA_SEARCH_PATH_DEFAULT);
  $path = drupal_lookup_path('source', $search_data_path . '/overview');

  if ($path) {
    $node = menu_get_object('node', 1, $path);
    $build['overview']['overview_text'] = node_view($node);
    unset($node);
  }
}


/**
 * Adds the map search (via google maps) to the data search portal page.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_map_search(&$build) {
  // Add the fieldset
  $build['site_map_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map Site Search'),
    '#attributes' => array(
      'class' => array('open'),
      'id' => array('map-search'),
    ),
  );

  // Add the instructions
  $build['site_map_search']['map_search_instructions'] = array(
    '#type' => 'markup',
    '#markup' => t('Each marker on the map represents a collection site. ' .
                   'Click on a marker to view information about that site ' .
                   'and/or search for that site only in the table.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  // Add the map placeholder
  $build['site_map_search']['map_placeholder'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('map-placement'),
    ),
  );
}



/**
 * Adds the table and filters to the data search portal page.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_table_and_filters(&$build) {
  // Add the table and filters container
  $build['table_and_filters'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('table-and-filters-container'),
    ),
  );

  // Add hide/show filters toggle
  $build['table_and_filters']['hide_show_filters'] = array(
    '#type' => 'markup',
    '#markup' => t('<a id="@id" href="#">Hide filter menu</a>',
      array('@id' => 'hide-show-filters-link')),
    '#prefix' => '<div id="hide-show-filters">',
    '#suffix' => '</div>',
  );

  // Add container for table filters
  $build['table_and_filters']['table_filters'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('table-filters'),
    ),
  );

  // Add fieldset for table search type filter
  dirt_datatable_add_search_type_filter($build['table_and_filters']['table_filters']);

  // Add fieldset for table search filters
  dirt_datatable_add_search_filters($build['table_and_filters']['table_filters']);

  // Add container for search table
  $build['table_and_filters']['search_table'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('table-container'),
    ),
  );

  // Add search table

  // Drupal doesn't allow for multi-row headers, so we have to do this the
  // old-fashioned way... it will be themed by jQuery DataTable.js anyway.

  $build['table_and_filters']['search_table']['table_content'] = array(
    '#type' => 'markup',
    '#markup' => dirt_datatable_get_search_table_markup(),
  );
}


/**
 * Add the search type filter to the filters section of the data search
 * portal page.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_search_type_filter(&$build) {
  // Add the search type fieldset
  // Enclose in large width filter class div to hide on smaller screens
  $build['search_type_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search Type'),
    '#attributes' => array(
      'class' => array('open'),
      'id' => array('search-type-fieldset'),
    ),
    '#prefix' => '<div class="width-filter large-width-filter">',
    '#suffix' => '</div>', 
  );

  // Add a container for the radio button search type filter
  $build['search_type_fieldset']['search_type_radios'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('search-type-radios'),
    ),
  );

  // Add the search type explanation text
  $build['search_type_fieldset']['search_type_radios']['search_type_explanation'] = array(
    '#type' => 'markup',
    '#markup' => t('Select one of the following options to begin your search:'),
    '#prefix' => '<div id="search-type-explanation">',
    '#suffix' => '</div>',
  );

  // Add search type radio option
  // This feels hack-y but form API can't be used without a call to
  // drupal_get_form().
  // See: https://drupal.stackexchange.com/questions/146898/how-do-you-render-a-set-of-checkboxes-without-using-drupal-get-form

  $habitat_field = field_info_field('field_dirt_site_habitat_type');

  // Info for the search type radio options
  $search_type_info = array();
  $idx = 0;

  // Search by site ID
  $search_type_info[$idx] = array(
    'id' => 'site-id',
    'label' => t('Search by Site ID'),
    'sublabel' => '',
  );
  $idx += 1;

  // Search by habitat type
  if (!empty($habitat_field)) {
    $search_type_info[$idx] = array(
      'id' => 'habitat',
      'label' => t('Search by Habitat Type'),
      'sublabel' => t('(e.g. forest, cropland, etc.)'),
    );
    $idx += 1;
  }

  // Include all sites
  $search_type_info[$idx] = array(
    'id' => 'all',
    'label' => t('Include All Sites'),
    'sublabel' => '',
  );

  $markup = '';
  foreach ($search_type_info as $info_arr) {
    $id = $info_arr['id'];
    $markup .= '<div class="radio-container">';
    $markup .= '<input type="radio" name="search-type" id="search-type-' . $id . '" value="' . $id . '">';
    $markup .= '<label for="search-type-' . $id . '">' . $info_arr['label'] . '</label>';
    $markup .= '<div class="search-type-label-div">' . $info_arr['sublabel'] . '</div></label>';
    $markup .= '</div>';
  }

  $build['search_type_fieldset']['search_type_radios']['search_type'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="checkbox-container">',
    '#suffix' => '</div>',
  );

  unset($markup);
  unset($search_type_info);
}


/**
 * Add the search filters to the data search portal page.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_search_filters(&$build) {
  // Add the search filters fieldset
  $build['table_filters_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Table Filters'),
    '#attributes' => array(
      'class' => array('open'),
      'id' => array('search-filters-fieldset'),
    ),
  );

  // Add instructions for tablet users
  dirt_datatable_add_tablet_instructions($build['table_filters_fieldset']);

  // Add reset filters button
  dirt_datatable_add_reset_filters_button($build['table_filters_fieldset']);

  // Add refresh table button
  dirt_datatable_add_refresh_table_button($build['table_filters_fieldset']);
  
  // Add habitat type filter
  $habitat_field = field_info_field('field_dirt_site_habitat_type');
  if (!empty($habitat_field))
    dirt_datatable_add_habitat_type_filter($build['table_filters_fieldset']);

  // Add site ID filter
  dirt_datatable_add_site_id_filter($build['table_filters_fieldset']);

  // Add variable filter
  dirt_datatable_add_variable_filter($build['table_filters_fieldset']);

  // Add county filter
  $county_field = field_info_field('field_dirt_site_county');
  if (!empty($county_field))
    dirt_datatable_add_county_filter($build['table_filters_fieldset']);

  // Add date range filter
  dirt_datatable_add_date_range_filter($build['table_filters_fieldset']);
}


/**
 * Returns the search table markup (html).
 *
 * @retval String HTML markup for table. Since table rows will be populated
 * via JSON, this sets up a table skeleton with the correct headers but
 * an otherwise empty table.
 */
function dirt_datatable_get_search_table_markup() {
  $header = '<thead><tr>';

  // Open table
  $markup = '<table id="dirt-data-table" class="display" width="100%" cellspacing="0">';

  // Get column data
  $table_col_data = dirt_datatable_get_table_column_data();

  // For each column header group, add a property called "headers" that
  // will contain the header markup.
  // Also add a lookup for filter group that column group belongs to (if any).
  $header_groups = array();
  $group_class = array();
  foreach ($table_col_data->colgroups as $col_obj) {
    $colgroup = $col_obj->gid;
    $header_groups[$colgroup] = $col_obj;
    $header_groups[$colgroup]->headers = array();

    // set filter group classes
    $element_id = dirt_datatable_get_filter_element_id($col_obj->filter_group);
    $group_class[$colgroup] = $col_obj->class . ' ' . 'var-filter-' . $element_id;
  }

  // Get common group ID
  $common_gid = variable_get('dirt_datatable_colgroup_gid_common', 0);
  // Populate headers for each group
  // These are already sorted by weight so we can add them in one by one
  // to their appropriate column group
  foreach ($table_col_data->tablecols as $col_obj) {
    $title = $col_obj->label;
    if ($col_obj->admin_only)
      $title .= ' ' . t('(This column is only visible to site admins.)');

    if (dirt_datatable_is_rendered_data_table_column($col_obj)) {
      $colgroup = $col_obj->colgroup;
      // Special case: first column group of common fields
      if ($colgroup == $common_gid) {
        $class = $col_obj->class;
        $header_groups[$colgroup]->headers[] = array(
          'html' => '<th rowspan="2" class="' . $class . '" title="' . $title . '">' . $col_obj->label . '</th>',
          'weight' => $col_obj->weight,
        );
      }
      elseif (!empty($colgroup)) {
        $classes = explode(' ', $col_obj->class);
        $classes[] = $group_class[$colgroup];
        $class = implode(' ', $classes);
        $header_groups[$colgroup]->headers[] = array(
          'html' => '<th class="' . $class . '" title="' . $title . '">' . $col_obj->label . '</th>',
          'weight' => $col_obj->weight,
        );
      }
    }
  }

  unset($table_col_data);

  // Sort the header groups by weight so that we add them in the right order
  // Important: this re-indexes to begin with 0 (common group)!!
  usort($header_groups, "_dirt_datatable_row_weight_cmp");

  // Add common field headers (special case)
  $header_html = array();
  foreach($header_groups[0]->headers as $info_arr) {
    $header_html[] = $info_arr['html'];
  }
  $header .= implode("", $header_html);

  unset($header_groups[0]); // no colspan for the common header group

  // Add the colspans for the rest of the header groups.
  // Note that one of the header groups will be the "empty" group (fields
  // that don't belong to any column group).
  // But under normal circumstances, this group will be empty, so only
  // add <th> tag if group is not empty (count > 0).
  foreach ($header_groups as $idx => $group_info) {
    $colspan_count = count($group_info->headers);
    if ($colspan_count > 0)
      $header .= '<th colspan="' . $colspan_count . '" class="' . $group_info->class . '">' . $group_info->label . '</th>';
  }

  $header .= '</tr><tr>'; // end colspan row and begin next row with actual headers

  // Add the rest of the column headers:
  foreach ($header_groups as $idx => $group_info) {
    $col_header_info = $group_info->headers;
    $header_html = array();
    foreach($col_header_info as $info_arr) {
      $header_html[] = $info_arr['html'];
    }
    $header .= implode("", $header_html);
  }
  // Close header
  $header .= '</tr></thead>';

  // Add header to table
  $markup .= $header;

  // Close table
  $markup .= '</table>';

  return $markup;
}


/**
 * Add the instructions for tablet users to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_tablet_instructions(&$build) {
  // Set up container
  $build['tablet_instructions'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('tablet-width'),
    ),
  );

  // Get tablet instructions node from its path
  $search_data_path = variable_get('dirt_data_search_portal_path', DIRT_DATA_SEARCH_PATH_DEFAULT);
  $path = drupal_lookup_path('source', $search_data_path . '/instructions-tablet');
  if ($path) {
    $node = menu_get_object('node', 1, $path);
    $build['tablet_instructions']['instructions_text'] = node_view($node);
  }

  unset($node);
  unset($path);
}


/**
 * Adds the reset filters button to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_reset_filters_button(&$build) {
  $markup  = '<div title="' . t('Begin a new search') . '" class="full-screen-width dt-buttons">';
  $markup .= '<a class="dt-button" href="#"><span>';
  $markup .= t('Reset Search Filters');
  $markup .= '</span></a></div>';

  $build['reset_filters_button'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div id="reset-search-button">',
    '#suffix' => '</div>',
  );

  unset($markup);
}


/**
 * Adds the refresh table button to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_refresh_table_button(&$build) {
  $markup  = '<div title="' . t('Refresh table without reloading page') . '" class="full-screen-width dt-buttons">';
  // Add the button twice: one enabled, one disabled; will toggle between the two with JS
  $button_classes = array('button-enabled', 'button-disabled');
  foreach($button_classes as $class) {
    $markup .= '<a class="dt-button ' . $class . '" href="#"><span>';
    $markup .= t('Refresh table');
    $markup .= '</span></a>';
  }
  $markup .= '</div>';

  // Add the button markup
  $build['refresh_table_button'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div id="refresh-table-button">',
    '#suffix' => '</div>',
  );

  // Add the loading status div
  $markup  = '<div class="throbber">&nbsp;</div>';
  $markup .= '<div id="refresh-table-loading-status-text"></div>';
  $build['refresh_loading_status'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div id="refresh-table-loading-status">',
    '#suffix' => '</div>',
  );

  unset($markup);
}


/**
 * Adds the habitat type filter to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_habitat_type_filter(&$build) {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  // We'll add two types of filters: chosen select (for desktops) and drop-downs (for tablets and mobile)
  // Use JS to toggle visibility of filters depending on screen width

  // Get max access level for logged-in user
  $max_access_level = dirt_datatable_get_max_access_level();

  // Get all habitat type options
  $habitat_type_options = dirt_get_habitat_type_options();

  // Get habitat types that actually appear in table
  $habitat_types_in_table = dirt_datatable_get_all_habitat_types_from_data_table($max_access_level);

  // Create array of habitat type options based on what appears in table
  $habitat_types = array();
  // Boolean whether to include "Other" as an option. If no one has
  // selected "Other" for habitat type, don't include it.
  $include_other = FALSE;

  foreach ($habitat_types_in_table as $habitat_type) {
    if (in_array($habitat_type, $habitat_type_options)) {
      // Create short name for habitat type using last/only word in name
      // TODO: maybe should change to whole name, spaces turned into hyphens
      $words = explode(' ', $habitat_type);
      $habitat_short_name = array_pop($words);
      $habitat_types[$habitat_short_name] = $habitat_type;
    }
    else {
      $include_other = TRUE;
    }
  }
  if ($include_other) {
    $habitat_types['other'] = 'Other';
  }

  // Add a container for all habitat type filters
  $build['habitat_type_filter'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('habitat-type-filter'),
    ),
  );

  // Add habitat type chosen select filter
  $markup  = t('<h5>Habitat Types</h5>');
  $markup .= '<div id="habitat-type-chosen-select-container">';
  $markup .= '<select id="habitat-type-select" data-placeholder="';
  $markup .= t('Choose Habitat Type(s)');
  $markup .= '" multiple class="chosen-select select">';
  // Add blank option
  $markup .= '<option value></option>';

  foreach ($habitat_types as $name => $label) {
    $markup .= '<option value="'. $label . '">' . $label . '</option>';
  }

  $markup .= '</select></div>';

  $build['habitat_type_filter']['habitat_chosen_select'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter large-width-filter">',
    '#suffix' => '</div>',
  );

  // For drop-down filters where only one habitat may be selected at a time,
  // add an "All Habitats" option
  $habitat_types['all'] = t('All Habitats');

  // Add habitat type drop-down filter
  $markup  = t('<b>Habitat Type</b><br>');
  $markup .= '<div id="habitat-type-dropdown-container">';
  $markup .= '<select class="form-select">';
  foreach($habitat_types as $name => $label) {
    $markup .= '<option name="habitat-type" value="' . $label . '">' . $label . '</option>';
  }
  $markup .= '</select></div>';

  $build['habitat_type_filter']['habitat_dropdown_select'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter small-width-filter">',
    '#suffix' => '</div>',
  );

  unset($markup);
  unset($habitat_types);
  unset($habitat_types_in_table);  
  unset($habitat_type_options);
  unset($instance);
}


/**
 * Adds the site ID filter to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_site_id_filter(&$build) {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  // Search by Site ID is a large width only filter because it doesn't work well
  // for small screens once there are many collection sites in the system.

  // Get max access level for logged-in user
  $max_access_level = dirt_datatable_get_max_access_level();

  // Get all site ids that appear in table subject to logged in user's access level
  $site_ids = dirt_datatable_get_all_site_ids_from_data_table($max_access_level);

  // Add a container for site ID filter
  $build['site_id_filter'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('site-id-filter'),
    ),
  );

  // Add site ID chosen select filter
  $markup  = t('<h5>Site IDs</h5>');
  $markup .= '<div id="site-id-chosen-select-container">';
  $markup .= '<select id="site-id-select" data-placeholder="';
  $markup .= t('Choose Site ID(s)');
  $markup .= '" multiple class="chosen-select select">';
  // Add blank option
  $markup .= '<option value></option>';

  foreach($site_ids as $idx => $site_id) {
    $markup .= '<option value="'. $site_id . '">' . $site_id . '</option>';
  }

  $markup .= '</select></div>';

  $build['site_id_filter']['site_id_chosen_select'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter large-width-filter">',
    '#suffix' => '</div>',
  );

  unset($markup);
  unset($site_ids);
}


/**
 * Adds the variable filter to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_variable_filter(&$build) {
  // We'll add two types of filters: checkboxes (for desktops) and drop-downs (for tablets and mobile)
  // Use JS to toggle visibility of filters depending on screen width

  // Get variables
  $variables = dirt_datatable_get_variables();

  // Special case: group info is admin-only 
  // Add if logged-in user is admin
  $admin_only_vars = array();
  $group_type = 'dirt_group_information_survey';
  if (dirt_node_type_is_survey_type($group_type)) {
    if (user_access('administer nodes')) {
      $admin_only_vars['group-info'] = array(
        'fid' => 0,
        'label' => t('Group Information'),
        'survey_type' => $group_type,
      );
    }
  }

  $variables = $admin_only_vars + $variables;

  // Add a container for variable filter
  $build['variable_filter'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('variable-filter'),
    ),
  );
  
  // Add variable checkboxes for larger width screens
  $markup = dirt_datatable_get_variable_filter_checkboxes($variables);

  $build['variable_filter']['variable_checkboxes'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter large-width-filter">',
    '#suffix' => '</div>',
  );

  // Add variable dropdown for smaller width screens (phone, etc.)
  $markup = dirt_datatable_get_variable_filter_dropdown($variables);

  $build['variable_filter']['variable_dropdown'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter small-width-filter">',
    '#suffix' => '</div>',
  );
 
  unset($markup);
  unset($variables);
}


/**
 * Get variable filters.
 * 
 * @retval Array of variable class => label.
 */
function dirt_datatable_get_variables() {
  $variables = array();

  $groups = dirt_datatable_get_filter_groups();

  // Sort the filter groups by weight
  usort($groups, "_dirt_datatable_row_weight_cmp");

  // Index by element ID
  foreach($groups as $group) {
    $variables[$group->element_id] = array(
      'fid' => $group->fid,
      'label' => $group->label,
      'survey_type' => $group->survey_type,
    );
  } 

  return $variables;
}


/**
 * Get markup for variable checkboxes filter (for large screens).
 *
 * @param[in] $variables Associative array where keys are option values
 * and values are human-readable option labels.
 *
 * @retval String Markup (HTML) for variable checkboxes filter.
 */
function dirt_datatable_get_variable_filter_checkboxes($variables) {
  // Add markup
  $markup  = t('<h5>Variables</h5>');
  $markup .= '<div id="variable-checkboxes-container">';  

  // Add check all / uncheck all boxes
  $markup .= '<div class="check-all">';
  $markup .= '<a id="check-all-variable" href="#">';
  $markup .= t('Check all');
  $markup .= '</a> | <a id="uncheck-all-variable" href="#">';
  $markup .= t('Uncheck all');
  $markup .= '</a></div>';

  // Iterate over variables and add the checkboxes
  foreach($variables as $id => $var_info) {
    // First check if filter group is empty - if so, skip
    $fid = $var_info['fid'];
    $cols_in_group = dirt_datatable_get_columns_in_filter_group($fid, user_access('administer nodes'), FALSE);

    if (!empty($cols_in_group)) {
      // Get the collection frequency from the survey type
      $frequency_num = dirt_get_collection_frequency_of_survey_type($var_info['survey_type']);
      $frequency = _dirt_datatable_get_formatted_collection_frequency($frequency_num);
    
      // Get the list of variables belonging to this filter group
      $var_fields = _dirt_datatable_get_fields_in_filter_group($fid);

      // Add checkbox container div
      $admin_only = ($id == 'group-info') ? ' admin-only' : '';
      $markup .= '<div class="checkbox-container' . $admin_only . '">';
    
      // Add checkbox
      // Add the survey type as a class, prefixed by 'survey-type-'
      $markup .= '<input type="checkbox" class="form-checkbox survey-type-' . $var_info['survey_type'] . '" id="col-visibility-';
      $markup .= $id;
      $markup .= '" name="col-visibility" value="' . $id . '">';
      $markup .= '<label for="col-visibility-' . $id . '">' . $var_info['label'] . '<br>';
      $markup .= '<span class="variable-description">';
      $markup .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
      $markup .= '(' . $frequency . ')';

      // Add list of fields
      $markup .= '<ul>';
      foreach($var_fields as $field => $label) {
        $markup .= '<li class="variable-description">' . $label . '</li>';
      }
      $markup .= '</ul>';
      $markup .= '</span></label></div>';
    }
  }

  $markup .= '</div>';

  return $markup;
}


/**
 * Get markup for variable dropdown filter (for small screens).
 *
 * @param[in] $variables Associative array where keys are option values
 * and values are human-readable option labels.
 *
 * @retval String Markup (HTML) for variable dropdown filter.
 */
function dirt_datatable_get_variable_filter_dropdown($variables) { 
  // Remove site description since it will always appear
  unset($variables['site-desc']);

  $markup  = t('<b>Variable</b>');
  $markup .= '<br><div id="variable-dropdown-container">';
  $markup .= '<select class="form-select">';
  foreach($variables as $id => $var_info) {
    $markup .= '<option name="col-visibility" value="' . $id . '">' . $var_info['label'] . '</option>';
  }
  $markup .= '</select></div>';

  return $markup;
}


/**
 * Adds the county filter to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_county_filter(&$build) {
  // Get max access level for logged-in user
  $max_access_level = dirt_datatable_get_max_access_level();

  // Add a container for county filter
  $build['county_filter'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('county-filter'),
    ),
  );

  // Add county filter for larger width screens
  $markup = dirt_datatable_get_county_filter_select($max_access_level);

  $build['county_filter']['county_select'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter large-width-filter">',
    '#suffix' => '</div>',
  );
}


/**
 * Get select filter markup for county filter.
 *
 * @param[in] $max_access_level Max access level integer (0, 1, or 2) of
 *   logged-in user.
 */
function dirt_datatable_get_county_filter_select($max_access_level) {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  $markup  = t('<h5>Counties</h5>');
  $markup .= '<div id="county-chosen-select-container">';

  // Get all counties that appear in table subject to logged in user's access
  // level
  $counties =  dirt_datatable_get_all_counties_from_data_table($max_access_level);

  $markup .= '<select id="county-select" data-placeholder="';
  $markup .= t('Choose counties');
  $markup .= '" multiple class="chosen-select select">';
  // Add blank option
  $markup .= '<option value></option>';

  foreach ($counties as $idx => $county) {
    $markup .= '<option value="'. $county . '">' . $county . '</option>';
  }

  $markup .= '</select></div>';

  return $markup;
}


/**
 * Adds date range filter to the search portal filters fieldset.
 *
 * @param[in,out] $build Render array.
 */
function dirt_datatable_add_date_range_filter(&$build) {
  // Add a container for date range filter
  $build['date_range_filter'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => array('date-range-filter'),
    ),
  );

  // Add date range filter
  $markup = dirt_datatable_get_date_range_filter_dropdowns();

  $build['date_range_filter']['date_range_dropdowns'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => '<div class="width-filter">',
    '#suffix' => '</div>',
  );
}


/**
 * Gets markup for the date range filter dropdowns.
 */
function dirt_datatable_get_date_range_filter_dropdowns() {
  // Define months and years for filter range
  $months = array('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');
  $current_year = date('Y');
  $start_year = variable_get('dirt_project_start_year', $current_year);

  $years = array();
  for ($year = $start_year; $year <= $current_year; ++$year) {
    $years[] = $year;
  }

  $markup  = t('<h5>Date Range</h5>');

  // Add date dropdowns
  $date_dropdowns = array('date-start' => t('Survey date on or after:'),
                          'date-end'   => t('Survey date on or before:') );

  foreach($date_dropdowns as $name => $label) {
    $markup .= '<label for="' . $name . '-year">' . $label . ' </label>';
    $markup .= '<select class="form-select" id="' . $name . '-year" name="' . $name . '">';

    // Year dropdown
    $markup .= '<option value="" selected="selected">-' . t('Year') . '</option>';
    foreach ($years as $year) {
      $markup .= '<option value="' . $year . '">' . $year. '</option>';
    }
    $markup .= '</select>';

    // Month dropdown
    $markup .= '<select class="form-select" id="' . $name . '-month" name="' . $name . '">';
    $markup .= '<option value="" selected="selected">-' . t('Month') . '</option>';
    foreach ($months as $idx => $month) {
      $markup .= '<option value="' . $idx . '">' . $month . '</option>';
    }
    $markup .= '</select>';
  }

  unset($months);
  unset($current_year);
  unset($start_year);
  unset($years);
  unset($date_dropdowns);

  return $markup;
}


/**
 * Get JSON table source.
 */
function dirt_datatable_get_table_source() {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

 // Get max access level based on logged in user
 $max_access_level = dirt_datatable_get_max_access_level();

 // Get table content from database query
 $source = dirt_datatable_get_all_data_from_data_table($max_access_level);

 // Convert to JSON
 drupal_json_output($source);

 unset($max_access_level);
 unset($source);

 drupal_exit();
}


/**
 * Get JSON table column metadata.
 */
function dirt_datatable_get_table_metadata_json() {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  $col_data = dirt_datatable_get_table_column_metadata();

  $table_cols = array();

  foreach($col_data as $col_obj) {
    if (dirt_datatable_is_rendered_data_table_column($col_obj)) {
      $table_cols[] = array(
        'data' => $col_obj->column_name,
        'className' => $col_obj->class,
      );
    }
  }

  unset($col_data);

  // Convert to JSON
  drupal_json_output($table_cols);

  unset($table_cols);

  drupal_exit();
}


/**
 * Returns whether table column should actually be rendered in the data search
 * table. Some columns in the database table are not actually rendered in the
 * search table on the data search portal page because they are only used for
 * populating the table rows (e.g., sharing permissions column) or are
 * grouped together in a new column (e.g., survey types).
 *
 *
 * @param[in] $col_obj Object containing column metadata (column name, etc.)
 *
 * @retval Boolean True if column is to be rendered, FALSE otherwise.
 */
function dirt_datatable_is_rendered_data_table_column($col_obj) {
  // Render column in table if:
  // 1) it's not an admin-only column, unless logged-in user is admin
  // 2) it's not one of the columns that has their fields grouped together,
  //    i.e., nid or survey_type (which get grouped into nids and row_types)
  // 3) it's not user sharing perms column, which is used in the query to
  //    populate the table rows but isn't actually rendered in the table
  //    itself.

  $render_admin_only_col = (!($col_obj->admin_only) || user_access('administer nodes'));
  $excluded_cols = array('nid', 'survey_type', 'dirt_user_sharing_perms');
  $not_excluded_col = (!in_array($col_obj->column_name, $excluded_cols));

  return ($render_admin_only_col && $not_excluded_col);
}


/**
 * Helper function to get a formatted collection frequency string for the
 * variables filters from the collection frequency number.
 *
 * @param[in] $frequency_num Integer Collection frequency number (1-12, or
 *   DIRT_ONE_TIME_COLLECTION for one-time collection).
 *
 * @retval String Formatted collection frequency string (e.g., "Collected once a year").
 */
function _dirt_datatable_get_formatted_collection_frequency($frequency_num) {
  // Default to frequency string:
  $frequency = t('Collected') . ' ' . dirt_get_collection_frequency_str_of_survey_type($frequency_num);

  // Special cases for data search portal:
  switch($frequency_num) {
    case DIRT_ONE_TIME_COLLECTION:
      // Use "once" instead of "one-time"
      $frequency = t('Collected once');
      break;

    case '12':
      // Use "once a month" instead of "monthly"
      $frequency = t('Collected once a month');
      break;
  }

  return $frequency;
}


/**
 * Helper function to get list of all fields contained in a
 * variable filter group. Only includes admin-only fields
 * if logged-in user is a site admin.
 *
 * @param[in] $fid Filter group ID.
 *
 * @retval Array of field labels belonging to filter group
 *  (indexed by field name).
 */
function _dirt_datatable_get_fields_in_filter_group($fid) {
  // Initialize return array
  $var_fields = array();

  // Begin database query
  $query = db_select(DIRT_SEARCH_TABLE_COLUMNS, 't');

  // Join in column group data
  $query->leftJoin(DIRT_SEARCH_TABLE_COLUMN_GROUPS, 'c', 't.colgroup = c.gid');

  // Join in filter group data
  $query->leftJoin(DIRT_SEARCH_TABLE_FILTER_GROUPS, 'f', 'c.filter_group = f.fid');

  // Limit to the filter group passed in as parameter
  $query->condition('f.fid', $fid);

  // Unless logged-in user is admin, exclude admin-only fields
  if (!user_access('administer nodes')) {
    $query->condition('t.admin_only', '0');
  }

  // Get the field label and field name and whether admin-only
  $query->fields('t', array('field_label', 'field_name', 'admin_only'));

  // Group by field label to remove duplicates (in case of field collections)
  $query->groupBy('t.field_label');

  // Order by column group first, then weight within column group
  $query->orderBy('c.gid');
  $query->orderBy('t.weight');

  // Get results
  $result = $query->execute()->fetchAll();

  foreach($result as $result_obj) {
    $label = $result_obj->field_label;

    if ($result_obj->admin_only)
      $var_fields[$result_obj->field_name] = '<span class="admin-only">' . $label . '</span>';
    else
      $var_fields[$result_obj->field_name] = $label;
  }

  // Some special cases for pre-installed DIRT fields
  // (makes filter menu easier to read)

  // Combine county, state, and zip code in one line
  $fields = array('field_dirt_site_county', 'field_dirt_site_state', 'field_dirt_site_zip_code');
  _dirt_datatable_combine_field_labels($fields, $var_fields);

  // Combine geo coords into one line with label "Coordinates"
  $fields = array('field_dirt_site_geo_coords_lat', 'field_dirt_site_geo_coords_lon');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Coordinates'));

  // Combine mean temp and precip in one line with a shorter label
  $fields = array('field_dirt_site_mean_annual_temp', 'field_dirt_site_annual_precip');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Mean Temperature & Precipitation'));

  // Combine map unit name & symbol into one line with label "Soil Map Unit"
  $fields = array('field_dirt_site_map_unit_name', 'field_dirt_site_map_unit_symbol');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Soil Map Unit'));

  // Combine present and past land management into one with shorter label
  $fields = array('field_dirt_site_present_manage', 'field_dirt_site_past_manage');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Past & Present Land Management'));

  // Combine texture and color
  $fields = array('field_dirt_site_layer_texture', 'field_dirt_site_layer_color');
  _dirt_datatable_combine_field_labels($fields, $var_fields);

  // Change time and color into something more descriptive
  $fields = array('field_dirt_soil_resp_time');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Time of Data Collection'));
  $fields = array('field_dirt_soil_resp_color');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Color Number'));

  // Combine emissions fields
  $fields = array('field_dirt_co2c_room_temp_range', 'field_dirt_co2c_room_temp_upper', 'field_dirt_co2c_room_temp_lower', 'field_dirt_co2c_room_temp_avg');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('CO<sub>2</sub>-C Emissions at Room Temp'));
  $fields = array('field_dirt_co2c_field_temp_range', 'field_dirt_co2c_field_temp_upper', 'field_dirt_co2c_field_temp_lower', 'field_dirt_co2c_field_temp_avg');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('CO<sub>2</sub>-C Emissions at Field Soil Temp'));

  // Combine N, P, K
  $fields = array('field_dirt_soil_n_value', 'field_dirt_soil_n_ppm', 'field_dirt_soil_n_category');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Nitrogen as Nitrate (NO<sub>3</sub> ppm)'));
  $fields = array('field_dirt_soil_p_value', 'field_dirt_soil_p_ppm', 'field_dirt_soil_p_category');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Phosphorus as Phosphoric Acid (P<sub>2</sub>O<sub>5</sub> ppm)'));
  $fields = array('field_dirt_soil_k_value', 'field_dirt_soil_k_ppm', 'field_dirt_soil_k_category');
  _dirt_datatable_combine_field_labels($fields, $var_fields, t('Potassium as Potassium Oxide (K<sub>2</sub>O ppm)'));

  return $var_fields;
}


/**
 * Helper function that collapses multiple variable labels into 
 * one line (if possible).
 *
 * @param[in] $fields Array of fields to collapse into one line
 * @param[in,out] $var_fields Array of all fields => labels.
 * @param[in] $custom_label *Optional* String label to use if different than combining the
 *   existing labels.
 */
function _dirt_datatable_combine_field_labels($fields, &$var_fields, $custom_label = '') {
  $num = 0;
  $labels = array();
  $first_field = ''; // Key to keep
  $other_fields = array(); // Keys to discard
  foreach ($fields as $field) {
    if (array_key_exists($field, $var_fields)) {
      $num += 1;
      $labels[] = $var_fields[$field];
      if (empty($first_field)) $first_field = $field;
      else $other_fields[] = $field;
    }
  }

  $admin_only = FALSE;
  foreach($labels as $label) {
    if (strpos($label, 'admin-only') !== FALSE)
      $admin_only = TRUE;
  }

  // If none found, move on 
  if ($num > 0) {
    if ($custom_label) {
      // Apply custom label
      if ($admin_only)
        $custom_label = '<span class="admin-only">' . $custom_label . '</span>';
 
      $var_fields[$first_field] = $custom_label;
    }
    else {
      // Format depends on number of fields being combined
      if ($num == 2) {
        // Two fields: format A & B
        $var_fields[$first_field] = $labels[0] . ' & ' . $labels[1];
      }
      elseif ($num == 3) {
        // Three fields: format A, B, & C
        $var_fields[$first_field] = $labels[0] . ', ' . $labels[1] . ', & ' . $labels[2];
      }
    }

    // Unset other fields
    foreach ($other_fields as $other_field) {
      unset($var_fields[$other_field]);
    }
  }

  // Otherwise, no changes to $var_fields
}


/**
 * Helper function used in comparing table row weights.
 * See: https://www.php.net/manual/en/function.usort.php
 *
 * @param[in] $a Object (representing table row) with property named 'weight'.
 * @param[in] $b Object (representing table row) with property named 'weight'.
 *
 * @retval Integer less than, equal to, or greater than zero depending on
 *   whether the weight of $a is less than, equal to, or greater than the 
 *   weight of $b.
 */
function _dirt_datatable_row_weight_cmp($a, $b) {
  if ($a->weight == $b->weight) {
    return 0;
  }

  return ($a->weight < $b->weight) ? -1 : 1;
}


