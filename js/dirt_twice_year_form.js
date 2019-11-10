(function ($) {
  Drupal.dirtTwiceYearSurvey = {};

  /**
   * Adds listeners to twice a year survey elements and sets up page.
   */
  Drupal.behaviors.dirtTwiceYearSurvey = {
    attach: function (context, settings) {
      /** Note that JQuery version packaged with Drupal 7 is very old so this   **/
      /** script has to use outdated things like 'attr' instead of 'prop', etc. **/

      // When form loaded, attempt to (re-)calculate N, P, K, pH, and active carbon fields
      Drupal.dirtTwiceYearSurvey.setNPKValues('n');
      Drupal.dirtTwiceYearSurvey.setNPKValues('p');
      Drupal.dirtTwiceYearSurvey.setNPKValues('k');
      Drupal.dirtTwiceYearSurvey.setPHValues();
      Drupal.dirtTwiceYearSurvey.setACFields();

      // Go ahead and lock all fields to be automatically calculated now so that it only happens once
      var lockFieldIds = ['edit-field-dirt-soil-n-value-und-0-value',
                          'edit-field-dirt-soil-n-ppm-und-0-value',
                          'edit-field-dirt-soil-n-category-und-0-value',
                          'edit-field-dirt-soil-p-value-und-0-value',
                          'edit-field-dirt-soil-p-ppm-und-0-value',
                          'edit-field-dirt-soil-p-category-und-0-value',
                          'edit-field-dirt-soil-k-value-und-0-value',
                          'edit-field-dirt-soil-k-ppm-und-0-value',
                          'edit-field-dirt-soil-k-category-und-0-value',
                          'edit-field-dirt-soil-ph-value-und-0-value',
                          'edit-field-dirt-soil-ph-category-und-0-value',
                          'edit-field-dirt-soil-ac-range-und-0-value',
                          'edit-field-dirt-soil-quality-und-0-value',
                          'edit-field-dirt-soil-organic-matter-und'];

      var idx;
      for (idx = 0; idx < lockFieldIds.length; ++idx) {
        $('#' + lockFieldIds[idx]).attr('readonly', true);
        $('#' + lockFieldIds[idx]).addClass('lock-field');
      }

      // Add listeners

      // N Listener
      $('#edit-field-dirt-soil-n-und').change(function() {
        Drupal.dirtTwiceYearSurvey.setNPKValues('n');
      });

      // P Listener
      $('#edit-field-dirt-soil-p-und').change(function() {
        Drupal.dirtTwiceYearSurvey.setNPKValues('p');
      });

      // K Listener
      $('#edit-field-dirt-soil-k-und').change(function() {
        Drupal.dirtTwiceYearSurvey.setNPKValues('k');
      });

      // pH Listener
      $('#edit-field-dirt-soil-ph-und').change(function() {
        Drupal.dirtTwiceYearSurvey.setPHValues();
      });

      // Active Carbon Listener
      $('#edit-field-dirt-soil-active-carbon-und').change(function() {
        Drupal.dirtTwiceYearSurvey.setACFields();
      });
    }
  };

  /**
   * Sets field values for the N, P, or K field given as input.
   */
  Drupal.dirtTwiceYearSurvey.setNPKValues = function (npk) {
    var npkStr;   // N/P/K string value from field
    var npkParts; // N/P/K string components (array of value, ppm, and category)

    if ($('#edit-field-dirt-soil-' + npk + '-und').val() >= 0) {
      npkStr = $('#edit-field-dirt-soil-' + npk + '-und option:selected').text();
      npkParts = npkStr.split(' - ');
      $('#edit-field-dirt-soil-' + npk + '-value-und-0-value').val(npkParts[0]);
      $('#edit-field-dirt-soil-' + npk + '-ppm-und-0-value').val(npkParts[1]);
      $('#edit-field-dirt-soil-' + npk + '-category-und-0-value').val(npkParts[2]);
    }
    else {
      // "none" selected, clear NPK fields
      $('#edit-field-dirt-soil-' + npk + '-value-und-0-value').val('');
      $('#edit-field-dirt-soil-' + npk + '-ppm-und-0-value').val('');
      $('#edit-field-dirt-soil-' + npk + '-category-und-0-value').val('');
    }
  };

  /**
   * Sets value for pH fields.
   */
  Drupal.dirtTwiceYearSurvey.setPHValues = function () {
    var pHStr;  // pH string (includes value and category)
    var pH;     // pH string components
    var pHVal;  // pH value extracted from string
    var pHCat;  // pH category extracted from string

    if ($('#edit-field-dirt-soil-ph-und').val() >= 0) {
      pHStr = $('#edit-field-dirt-soil-ph-und option:selected').text();
      pH = pHStr.match(/[0-9\.]+/);
      // first will be match
      pHVal = pH[0];
      pH = pHStr.match(/\(([a-zA-z ]+)\)/);
      // first is match with ()s, second is match inside ()s
      pHCat = pH[1];

      // Set value and category
      $('#edit-field-dirt-soil-ph-value-und-0-value').val(pHVal);
      $('#edit-field-dirt-soil-ph-category-und-0-value').val(pHCat);
    }
    else {
      // clear pH fields
      $('#edit-field-dirt-soil-ph-value-und-0-value').val('');
      $('#edit-field-dirt-soil-ph-category-und-0-value').val('');
    }
  };

  /**
   * Sets value for active carbon, soil quality, and active organic matter
   * fields (all derived from active carbon value).
   */
  Drupal.dirtTwiceYearSurvey.setACFields = function () {
    var ACStr;          // Active carbon range string
    var AC;             // Active carbon range components
    var ACVal;          // Active carbon value
    var soilQuality;    // Soil quality string comonents
    var soilQualityVal; // Soil quality value

    if ($('#edit-field-dirt-soil-active-carbon-und').val() >= 0) {
      // Set active carbon range (lbs/A):
      ACStr = $('#edit-field-dirt-soil-active-carbon-und option:selected').text();
      AC = ACStr.match(/[^(]+/);  // everything before the first "("
      ACVal = AC[0].trim();
      $('#edit-field-dirt-soil-ac-range-und-0-value').val(ACVal);

      // Set soil quality:
      soilQuality = ACStr.match(/\(([a-zA-z ]+)\)/);
      // first match is with ()s, second is match inside ()s
      soilQualityVal = soilQuality[1];
      $('#edit-field-dirt-soil-quality-und-0-value').val(soilQualityVal);

      // Set active organic matter:
      // Since the values for active carbon and active organic matter sync up,
      // can just set the value of one to the other
      // e.g., value = 0 => active carbon value is also 0 (first in select options)
      ACVal = $('#edit-field-dirt-soil-active-carbon-und').val();
      $('#edit-field-dirt-soil-organic-matter-und').val(ACVal);      
    }
    else {
      // Clear AC range field
      $('#edit-field-dirt-soil-ac-range-und-0-value').val('');

      // Clear soil quality field
      $('#edit-field-dirt-soil-quality-und-0-value').val('');

      // Clear AOM field
      $('#edit-field-dirt-soil-organic-matter-und').val('_none');
    }
  }

})(jQuery);
