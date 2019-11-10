(function ($) {
  Drupal.dirtMonthlySurvey = {};

  /**
   * Adds listeners to monthly survey elements, sets up page, and runs first
   * pass of calculations.
   */
  Drupal.behaviors.dirtMonthlySurvey = {
    attach: function (context, settings) {
      /** Note that JQuery version packaged with Drupal 7 is very old so this   **/
      /** script has to use outdated things like 'attr' instead of 'prop', etc. **/

      // Add warning divs for average soil temp and water-filled pores
      $('.form-item-field-dirt-co2c-field-temp-range-und-0-value .description').append("<p class='avg-temp-warning'></p>");
      $('.field-name-field-dirt-bulk-density-calcs').append("<p class='water-filled-pores-warning'></p>");

      // When form loaded, attempt to (re-)calculate CO2-C fields. If can't be calculated this will display error message.
      Drupal.dirtMonthlySurvey.updateCO2Fields();

      // Similarly, attempt to (re-)calculate average temp at 5 C.
      Drupal.dirtMonthlySurvey.setAverage();

      // Go ahead and lock all fields to be automatically calculated now so that it only happens once
      var lockFieldIds = ['edit-field-dirt-avg-soil-temp-5cm-und-0-value',
                          'edit-field-dirt-co2c-room-temp-range-und-0-value',
                          'edit-field-dirt-co2c-room-temp-lower-und-0-value',
                          'edit-field-dirt-co2c-room-temp-upper-und-0-value',
                          'edit-field-dirt-co2c-room-temp-avg-und-0-value',
                          'edit-field-dirt-co2c-field-temp-range-und-0-value',
                          'edit-field-dirt-co2c-field-temp-lower-und-0-value',
                          'edit-field-dirt-co2c-field-temp-upper-und-0-value',
                          'edit-field-dirt-co2c-field-temp-avg-und-0-value',
                          'edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-wet-soil-und-0-value',
                          'edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-und-0-value',
                          'edit-field-dirt-bulk-density-calcs-und-0-field-dirt-soil-bulk-density-und-0-value',
                          'edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-gravimetric-und-0-value',
                          'edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-volumetric-und-0-value',
                          'edit-field-dirt-bulk-density-calcs-und-0-field-dirt-total-soil-porosity-und-0-value',
                          'edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-filled-pores-und-0-value'];
      var idx;
      for (idx = 0; idx < lockFieldIds.length; ++idx) {
        $('#' + lockFieldIds[idx]).attr('readonly', true);
        $('#' + lockFieldIds[idx]).addClass('lock-field');
      }

      // Add listeners to soil temp at 5 cm depth fields to find average soil temp at 5 cm depth
      // Also check if the CO2-C calculations can be done yet because the user might enter the color/hour info first then
      // go back up and enter the soil temperatures (or make a correction, which affects the CO2-C calculations)

      // Replicate 1 Listener:
      $('#edit-field-dirt-temp-cloud-cover-und-0-field-dirt-soil-temp-5cm-und-0-value', context).once('rep-1-temp-listener', function () {
        $(this).bind('input', function(event) {        
          Drupal.dirtMonthlySurvey.setAverage();
          Drupal.dirtMonthlySurvey.updateCO2Fields();
          event.stopPropagation();
        });
      });

      // Replicate 2 Listener:
      $('#edit-field-dirt-temp-cloud-cover-und-1-field-dirt-soil-temp-5cm-und-0-value', context).once('rep-2-temp-listener', function () {
        $(this).bind('input', function(event) {
          Drupal.dirtMonthlySurvey.setAverage();
          Drupal.dirtMonthlySurvey.updateCO2Fields();
          event.stopPropagation();
        });
      });

      // Replicate 3 Listener:
      $('#edit-field-dirt-temp-cloud-cover-und-2-field-dirt-soil-temp-5cm-und-0-value', context).once('rep-3-temp-listener', function () {
        $(this).change(function (event) {
          Drupal.dirtMonthlySurvey.setAverage();
          Drupal.dirtMonthlySurvey.updateCO2Fields();
          event.stopPropagation();
        });
      });

      // Replicate 4 Listener:
      $('#edit-field-dirt-temp-cloud-cover-und-3-field-dirt-soil-temp-5cm-und-0-value', context).once('rep-4-temp-listener', function () {
        $(this).bind('input', function(event) {
          Drupal.dirtMonthlySurvey.setAverage();
          Drupal.dirtMonthlySurvey.updateCO2Fields();
          event.stopPropagation();
        });
      });

      // Replicate 5 Listener:
      $('#edit-field-dirt-temp-cloud-cover-und-4-field-dirt-soil-temp-5cm-und-0-value', context).once('rep-5-temp-listener', function () {
        $(this).bind('input', function(event) {
          Drupal.dirtMonthlySurvey.setAverage();
          Drupal.dirtMonthlySurvey.updateCO2Fields();
          event.stopPropagation();
        });
      });

      // Add listeners for color hours:

      // Color at 5h listener:
      $('#edit-field-dirt-color-number-by-hour-und-5-field-dirt-soil-resp-color-und', context).once('color-5h-listener', function () {
        $(this).change(function (event) {
           Drupal.dirtMonthlySurvey.updateCO2Fields();
        });
      });

      // Color at 24h listener:
      $('#edit-field-dirt-color-number-by-hour-und-7-field-dirt-soil-resp-color-und', context).once('color-24h-listener', function () {
        $(this).change(function (event) {
          Drupal.dirtMonthlySurvey.updateCO2Fields();
        });
      });

      // Sometimes color at 5h will be empty because color number is large in previous hours
      // Add listener to 4h, 3h, etc.:
      // Color at 4h listener:
      $('#edit-field-dirt-color-number-by-hour-und-4-field-dirt-soil-resp-color-und', context).once('color-4h-listener', function () {
        $(this).change(function (event) {
          Drupal.dirtMonthlySurvey.updateCO2Fields();
        });
      });

      // Color at 3h listener:
      $('#edit-field-dirt-color-number-by-hour-und-3-field-dirt-soil-resp-color-und', context).once('color-3h-listener', function () {
        $(this).change(function (event) {
          Drupal.dirtMonthlySurvey.updateCO2Fields();
        });
      });
    
      // Similarly, in the non-fast-riser case, sometimes color at 24h will be empty and the calculation
      // should go ahead with the color at 10h
      // Color at 10h listener:
      $('#edit-field-dirt-color-number-by-hour-und-6-field-dirt-soil-resp-color-und', context).once('color-10h-listener', function () {
        $(this).change(function (event) {
          Drupal.dirtMonthlySurvey.updateCO2Fields();
        });
      });

      // Soil water content fields:

      // A Listener:
      $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-wet-soil-bag-und-0-value', context).once('column-a-listener', function () {
        $(this).bind('input', function(event) {
          // Value in Column A added. Attempt to set C, I, J, and L, which all depend on A.
          Drupal.dirtMonthlySurvey.setC();
          Drupal.dirtMonthlySurvey.setI();
          Drupal.dirtMonthlySurvey.setJ();
          Drupal.dirtMonthlySurvey.setL();
          event.stopPropagation();
        });
      });

      // B Listener:
      $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-bag-und-0-value', context).once('column-b-listener', function () {
        $(this).bind('input', function(event) {
          // Value in Column B added. Attempt to set C, I, J, and L, which all depend on B.
          Drupal.dirtMonthlySurvey.setC();
          Drupal.dirtMonthlySurvey.setI();
          Drupal.dirtMonthlySurvey.setJ();
          Drupal.dirtMonthlySurvey.setL();
          event.stopPropagation();
        });
      });

      // D Listener:
      $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-bowl-und-0-value', context).once('column-d-listener', function () {
        $(this).bind('input', function(event) {
          // Value in Column D added. Attempt to set F, H, I, J, K, and L, which all depend on D.
          Drupal.dirtMonthlySurvey.setF();
          Drupal.dirtMonthlySurvey.setH();
          Drupal.dirtMonthlySurvey.setI();
          Drupal.dirtMonthlySurvey.setJ();
          Drupal.dirtMonthlySurvey.setK();
          Drupal.dirtMonthlySurvey.setL();
          event.stopPropagation();
        });
      });

      // E Listener:
      $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-bowl-und-0-value', context).once('column-e-listener', function () {
        $(this).bind('input', function(event) {
          // Value in Column E added. Attempt to set F, H, I, J, K, and L, which all depend on E.
          Drupal.dirtMonthlySurvey.setF();
          Drupal.dirtMonthlySurvey.setH();
          Drupal.dirtMonthlySurvey.setI();
          Drupal.dirtMonthlySurvey.setJ();
          Drupal.dirtMonthlySurvey.setK();
          Drupal.dirtMonthlySurvey.setL();
          event.stopPropagation();
        });
      }); 
    
      // G Listener:
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-volume-soil-sample-und-0-value', context).once('column-g-listener', function () {
        $(this).bind('input', function(event) {
          // Value in Column G added. Attempt to set H, J, K, and L, which all depend on G.
          Drupal.dirtMonthlySurvey.setH();
          Drupal.dirtMonthlySurvey.setJ();
          Drupal.dirtMonthlySurvey.setK();
          Drupal.dirtMonthlySurvey.setL(); 
          event.stopPropagation();
        });
      });

      // Since C, F, H, I, J, K, and L are automatically calculated, no listeners needed.
    }
  };

  /**
   * Calculates the average soil temp at 5 cm depth.
   */
  Drupal.dirtMonthlySurvey.setAverage = function () {
    var sum;
    var average;

    var rep1 = $('#edit-field-dirt-temp-cloud-cover-und-0-field-dirt-soil-temp-5cm-und-0-value').val();
    var rep2 = $('#edit-field-dirt-temp-cloud-cover-und-1-field-dirt-soil-temp-5cm-und-0-value').val();
    var rep3 = $('#edit-field-dirt-temp-cloud-cover-und-2-field-dirt-soil-temp-5cm-und-0-value').val();
    var rep4 = $('#edit-field-dirt-temp-cloud-cover-und-3-field-dirt-soil-temp-5cm-und-0-value').val();
    var rep5 = $('#edit-field-dirt-temp-cloud-cover-und-4-field-dirt-soil-temp-5cm-und-0-value').val();

    // At least one rep should be filled in before calculating average. Count the number of reps
    // currently filled in:
    var numNonempty = $('input[id*="field-dirt-soil-temp-5cm-und-0-value"]').filter(function() {
      return $(this).val();
    }).length;

    // Compute the sum
    // Use + in front of variables to get values as integers
    sum = +rep1 + +rep2 + +rep3 + +rep4 + +rep5;

    if (numNonempty > 0) {
      average = sum / numNonempty;
      average = Math.round(average * 100) / 100;
      $('#edit-field-dirt-avg-soil-temp-5cm-und-0-value').val(average);
    }
    else {
      // Clear average field if all of the numbers have been deleted
      $('#edit-field-dirt-avg-soil-temp-5cm-und-0-value').val('');
      // Also need to clear the CO2-C fields because they depend on this average value
      Drupal.dirtMonthlySurvey.clearCO2Fields(); 
    }
  };

  /**
   * Update CO2-C field calculations.
   *
   * Note this function isn't the most efficient; it could be optimized to
   * handle the cases where the CO2-C fields don't need to be recalculated, but
   * it would make the code much more complicated and harder to follow.
   * Dec 19 2018 change: new threshold for fast-riser is >4 (so =5 or 6). Old
   * threshold was >=3.
   */
  Drupal.dirtMonthlySurvey.updateCO2Fields = function () {
    // First determine if we are in the fast-riser case. Check if 5h, 4h, 3h (in this order) color is > 4 (strict inequality).
    // If so, do fast riser calculations
    if ( $('#edit-field-dirt-color-number-by-hour-und-5-field-dirt-soil-resp-color-und').val() > 4 ) {
      // Do fast riser calculations with hour 5:
      Drupal.dirtMonthlySurvey.doFastRiserCO2Calcs($('#edit-field-dirt-color-number-by-hour-und-5-field-dirt-soil-resp-color-und').val());
    }
    else if ( $('#edit-field-dirt-color-number-by-hour-und-4-field-dirt-soil-resp-color-und').val() > 4 ) {
      // Do fast riser calculations with hour 4:
      Drupal.dirtMonthlySurvey.doFastRiserCO2Calcs($('#edit-field-dirt-color-number-by-hour-und-4-field-dirt-soil-resp-color-und').val());
    }
    else if ( $('#edit-field-dirt-color-number-by-hour-und-3-field-dirt-soil-resp-color-und').val() > 4 ) {
      // Do fast riser calculations with hour 3:
      Drupal.dirtMonthlySurvey.doFastRiserCO2Calcs($('#edit-field-dirt-color-number-by-hour-und-3-field-dirt-soil-resp-color-und').val());
    }
    else {
      // We're NOT in a fast-riser situation.
      // Check if the color at 10h is >= 6, if so then do the non-fast-riser
      // calculations with that number.
      // Otherwise continue to check 24h and do the calculation with that number
      // if it is present, or else clear the CO2-C fields.
      if ($('#edit-field-dirt-color-number-by-hour-und-6-field-dirt-soil-resp-color-und').val() >= 6) {
        // Do non-fast-riser calculations with hour 10:
        Drupal.dirtMonthlySurvey.doStandardCO2Calcs($('#edit-field-dirt-color-number-by-hour-und-6-field-dirt-soil-resp-color-und').val());
      }
      else if ($('#edit-field-dirt-color-number-by-hour-und-7-field-dirt-soil-resp-color-und').val() >= 0) {
        // Do non-fast-riser calculations with hour 24:
        Drupal.dirtMonthlySurvey.doStandardCO2Calcs($('#edit-field-dirt-color-number-by-hour-und-7-field-dirt-soil-resp-color-und').val());
      }
      else {
        // Not enough info to do calcs, so clear fields
        Drupal.dirtMonthlySurvey.clearCO2Fields();
      }
    }
  };

  /**
   * Clears all CO2-C fields.
   */
  Drupal.dirtMonthlySurvey.clearCO2Fields = function () {
    Drupal.dirtMonthlySurvey.clearRoomCO2Field();
    Drupal.dirtMonthlySurvey.clearFieldCO2Field();
  };

  /**
   * Clears CO2-C room temp fields.
   */
  Drupal.dirtMonthlySurvey.clearRoomCO2Field = function () {
    $('#edit-field-dirt-co2c-room-temp-range-und-0-value').val('');
    $('#edit-field-dirt-co2c-room-temp-lower-und-0-value').val('');
    $('#edit-field-dirt-co2c-room-temp-upper-und-0-value').val('');
    $('#edit-field-dirt-co2c-room-temp-avg-und-0-value').val('');
  };

  /**
   * Clears CO2-C field temp fields.
   */
  Drupal.dirtMonthlySurvey.clearFieldCO2Field = function () {
    $('#edit-field-dirt-co2c-field-temp-range-und-0-value').val('');
    $('#edit-field-dirt-co2c-field-temp-lower-und-0-value').val('');
    $('#edit-field-dirt-co2c-field-temp-upper-und-0-value').val('');
    $('#edit-field-dirt-co2c-field-temp-avg-und-0-value').val('');
  }


  /**
   * Calculates CO2-C fields in fast-riser case with color number given as
   * input (color value at one of 3h, 4h, or 5h).
   */
  Drupal.dirtMonthlySurvey.doFastRiserCO2Calcs = function (color) {
    var roomCO2CStr = '';   // CO2-C at room temp range as a string
    var roomCO2CLower = 0;  // CO2-C at room temp lower end of range (number)
    var roomCO2CUpper = 0;  // CO2-C at room temp upper end of range (number)
    var roomCO2CAvg = 0;    // CO2-C at room temp average (number)
    var fieldCO2CStr = '';  // CO2-C at field temp range as string
    var fieldCO2CLower = 0; // CO2-C at field temp lower end of range (number)
    var fieldCO2CUpper = 0; // CO2-C at field temp upper end of range (number)
    var fieldCO2CAvg = 0;   // CO2-C at field temp average (number)

    //--------------------
    // CO2-C AT ROOM TEMP
    //--------------------

    // TABLE 6 LOOKUP:
    // We know that color > 4 (=5 or 6)
    // Dec 19 2018 change: removed ranges for color <= 4
    /*
    if (color <= 3.5) {
      roomCO2CStr = '>12-38';
      roomCO2CLower = 12;
      roomCO2CUpper = 38;
    }
    else if (color <= 4.0) {
      roomCO2CStr = '>37-63';
      roomCO2CLower = 37;
      roomCO2CUpper = 63;
    }
    */
    if (color <= 5.0) {
      roomCO2CStr = '>62-150';
      roomCO2CLower = 62;
      roomCO2CUpper = 150;
    }
    else {
      roomCO2CStr = '>150-400';
      roomCO2CLower = 150;
      roomCO2CUpper = 400;
    }

    // Set string and number fields according to Table 6 lookup above,
    // converting the lower and upper range bounds to integers
    $('#edit-field-dirt-co2c-room-temp-range-und-0-value').val(roomCO2CStr);
    $('#edit-field-dirt-co2c-room-temp-lower-und-0-value').val(Math.floor(roomCO2CLower));
    $('#edit-field-dirt-co2c-room-temp-upper-und-0-value').val(Math.ceil(roomCO2CUpper));
 
    roomCO2CAvg = (+roomCO2CLower + +roomCO2CUpper) / 2.0;
    roomCO2CAvg = Math.round(roomCO2CAvg * 100) / 100;
    $('#edit-field-dirt-co2c-room-temp-avg-und-0-value').val(roomCO2CAvg);

    //---------------------
    // CO2-C AT FIELD TEMP
    //---------------------

    Drupal.dirtMonthlySurvey.doCO2CFieldCalc(roomCO2CLower, roomCO2CUpper, color);
  };

  /**
   * Calculates CO2-C fields in non-fast-riser case with color number given as
   * input (color value at one 10h or 24h).
   */
  Drupal.dirtMonthlySurvey.doStandardCO2Calcs = function (color) {
    var roomCO2CStr = '';   // CO2-C at room temp range as a string
    var roomCO2CLower = 0;  // CO2-C at room temp lower end of range (number)
    var roomCO2CUpper = 0;  // CO2-C at room temp upper end of range (number)
    var roomCO2CAvg = 0;    // CO2-C at room temp average (number)

    //--------------------
    // CO2-C AT ROOM TEMP
    //--------------------

    // TABLE 6 LOOKUP:
    // We know that color >= 0
    if (color <= 1) {
      roomCO2CStr = '0.5-1';
      roomCO2CLower = 0.5;
      roomCO2CUpper = 1;
    }
    else if (color <= 2.5) {
      roomCO2CStr = '>1-5';
      roomCO2CLower = 1;
      roomCO2CUpper = 5;
    }
    else if (color <= 3.5) {
      roomCO2CStr = '>5-15';
      roomCO2CLower = 5;
      roomCO2CUpper = 15;
    }
    else if (color <= 4.0) {
      roomCO2CStr = '>15-25';
      roomCO2CLower = 15;
      roomCO2CUpper = 25;
    }
    else if (color <= 5.0) {
      roomCO2CStr = '>25-60';
      roomCO2CLower = 25;
      roomCO2CUpper = 60;
    }
    else {
      // Color must be between 5.1-6.0
      roomCO2CStr = '>60-160';
      roomCO2CLower = 60;
      roomCO2CUpper = 160;
    }

    // Set string and number fields according to Table 6 lookup above
    $('#edit-field-dirt-co2c-room-temp-range-und-0-value').val(roomCO2CStr);
    $('#edit-field-dirt-co2c-room-temp-lower-und-0-value').val(roomCO2CLower);
    $('#edit-field-dirt-co2c-room-temp-upper-und-0-value').val(roomCO2CUpper);

    roomCO2CAvg = (+roomCO2CLower + +roomCO2CUpper) / 2.0;
    roomCO2CAvg = Math.round(roomCO2CAvg * 100) / 100;
    $('#edit-field-dirt-co2c-room-temp-avg-und-0-value').val(roomCO2CAvg);

    //---------------------
    // CO2-C AT FIELD TEMP
    //---------------------

    Drupal.dirtMonthlySurvey.doCO2CFieldCalc(roomCO2CLower, roomCO2CUpper, color);
  };

  /**
   * Calculates CO2-C at field temp fields.
   * The calculation is the same for fast-riser and non-fast-riser cases, with
   * different input values (lower and upper range and color hour value).
   */
  Drupal.dirtMonthlySurvey.doCO2CFieldCalc = function (lower, upper, color) {
    var fieldCO2CStr = '';  // CO2-C at field temp range as string
    var fieldCO2CLower = 0; // CO2-C at field temp lower end of range (number)
    var fieldCO2CUpper = 0; // CO2-C at field temp upper end of range (number)
    var fieldCO2CAvg = 0;   // CO2-C at field temp average (number)

    // Clear any warnings if they were triggered previously
    $('.avg-temp-warning').hide()

    // CO2-C at field temp relies on CO2-C at room temp (passed in as parameters) and average temp at 5cm depth
    // If avg temp at 5cm depth is present, do the calculation
    var avgTemp = $('#edit-field-dirt-avg-soil-temp-5cm-und-0-value').val();
    if (avgTemp) {
      if ( (avgTemp >= 4.5) && (avgTemp < 40.5) ) {
        // TABLE 7 lookup:
        // need to look up conversion factor based on average temp
        var convFactor = 1;  // initialize
        if (avgTemp < 5.5) {
          convFactor = 4.0;
        }
        else if (avgTemp < 6.5) {
          convFactor = 3.6;
        }
        else if (avgTemp < 7.5) {
          convFactor = 3.2;
        }
        else if (avgTemp < 8.5) {
          convFactor = 2.8;
        }
        else if (avgTemp < 9.5) {
          convFactor = 2.4;
        }
        else if (avgTemp < 10.5) {
          convFactor = 2.0;
        }
        else if (avgTemp < 11.5) {
          convFactor = 1.9;
        }
        else if (avgTemp < 12.5) {
          convFactor = 1.8;
        }
        else if (avgTemp < 13.5) {
          convFactor = 1.7;
        }
        else if (avgTemp < 14.5) {
          convFactor = 1.6;
        }
        else if (avgTemp < 15.5) {
          convFactor = 1.5;
        }
        else if (avgTemp < 16.5) {
          convFactor = 1.4;
        }
        else if (avgTemp < 17.5) {
          convFactor = 1.3;
        }
        else if (avgTemp < 18.5) {
          convFactor = 1.2;
        }
        else if (avgTemp < 19.5) {
          convFactor = 1.1;
        }
        else if (avgTemp < 20.5) {
          convFactor = 1.0;
        }
        else if (avgTemp < 21.5) {
          convFactor = 0.95;
        }
        else if (avgTemp < 22.5) {
          convFactor = 0.9;
        }
        else if (avgTemp < 23.5) {
          convFactor = 0.85;
        }
        else if (avgTemp < 24.5) {
          convFactor = 0.8;
        }
        else if (avgTemp < 25.5) {
          convFactor = 0.75;
        }
        else if (avgTemp < 26.5) {
          convFactor = 0.7;
        }
        else if (avgTemp < 27.5) {
          convFactor = 0.65;
        }
        else if (avgTemp < 28.5) {
          convFactor = 0.6;
        }
        else if (avgTemp < 29.5) {
          convFactor = 0.55;
        }
        else if (avgTemp < 30.5) {
          convFactor = 0.5;
        }
        else if (avgTemp < 31.5) {
          convFactor = 0.55;
        }
        else if (avgTemp < 32.5) {
          convFactor = 0.6;
        }
        else if (avgTemp < 33.5) {
          convFactor = 0.65;
        }
        else if (avgTemp < 34.5) {
          convFactor = 0.7;
        }
        else if (avgTemp < 35.5) {
          convFactor = 0.75;
        }
        else if (avgTemp < 36.5) {
          convFactor = 0.8;
        }
        else if (avgTemp < 37.5) {
          convFactor = 0.85;
        }
        else if (avgTemp < 38.5) {
          convFactor = 0.9;
        }
        else if (avgTemp < 39.5) {
          convFactor = 0.95;
        }
        // else convFactor = 1.0 (default)

        // Compute CO2-C at field temp from CO2-C range at room temp and conversion factor,
        // then take floor (lower bound) and ceil (upper bound) to get integer valued ranges
        fieldCO2CLower = Math.floor(lower / convFactor);
        fieldCO2CUpper = Math.ceil(upper / convFactor);

        fieldCO2CStr = fieldCO2CLower + '-' + fieldCO2CUpper;
        // in every column other than first, ranges are prefixed with ">"
        if (color > 1) {
          fieldCO2CStr = '>' + fieldCO2CStr;
        }

        // Set string and number fields according to Table 7 lookup above
        $('#edit-field-dirt-co2c-field-temp-range-und-0-value').val(fieldCO2CStr);
        $('#edit-field-dirt-co2c-field-temp-lower-und-0-value').val(fieldCO2CLower);
        $('#edit-field-dirt-co2c-field-temp-upper-und-0-value').val(fieldCO2CUpper);
        fieldCO2CAvg = (+fieldCO2CLower + +fieldCO2CUpper) / 2.0;
        fieldCO2CAvg = Math.round(fieldCO2CAvg * 100) / 100;
        $('#edit-field-dirt-co2c-field-temp-avg-und-0-value').val(fieldCO2CAvg);
      }
      else {
        // average temp outside table 7 range, display error and clear any previous CO2-C field temp values
        $('.avg-temp-warning').html("<b><i>Average soil temperature is too low or too high to calculate CO₂-C at field soil temperature ranges.</i></b>").fadeIn();
        Drupal.dirtMonthlySurvey.clearFieldCO2Field();
      }
    }
    else {
      // otherwise clear field temp CO2-C field
      Drupal.dirtMonthlySurvey.clearFieldCO2Field();
    }
  };

  /**
   * Sets value for C.
   */
  Drupal.dirtMonthlySurvey.setC = function () {
    var A = $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-wet-soil-bag-und-0-value').val();
    var B = $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-bag-und-0-value').val();
    var C;

    if (A && B) {
      C = A - B;
      C = Math.round(C * 100) / 100;
      $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-wet-soil-und-0-value').val(C);
    }
    else {
      // One of A or B missing, so clear C:
      $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-wet-soil-und-0-value').val('');
    }
  };

  /**
   * Sets value for F.
   */
  Drupal.dirtMonthlySurvey.setF = function () {
    var D = $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-bowl-und-0-value').val();
    var E = $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-bowl-und-0-value').val();
    var F;

    if (D && E) {
      F = D - E;
      F = Math.round(F * 100) / 100;
      $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-und-0-value').val(F);
    }
    else {
      // One of D or E missing, so clear F:
      $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-und-0-value').val('');
    }
  };

  /**
   * Sets value for H.
   */
  Drupal.dirtMonthlySurvey.setH = function () {
    var F = $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-und-0-value').val();
    var G = $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-volume-soil-sample-und-0-value').val();
    var H;
  
    if (F && G) {
      H = F / G;
      H = Math.round(H * 100) / 100;
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-soil-bulk-density-und-0-value').val(H);
    }
    else {
      // One of F or G missing, so clear H:
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-soil-bulk-density-und-0-value').val('');
    }
  };

  /**
   * Sets value for I.
   */
  Drupal.dirtMonthlySurvey.setI = function () {
    var C = $('#edit-field-dirt-sample-before-drying-und-0-field-dirt-weight-wet-soil-und-0-value').val();
    var F = $('#edit-field-dirt-sample-after-drying-und-0-field-dirt-weight-dry-soil-und-0-value').val();
    var I;

    if (C && F) {
      I = ((C - F) / F) * 100;
      I = Math.round(I * 100) / 100;
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-gravimetric-und-0-value').val(I);
    }
    else {
      // One of C or F missing, so clear I:
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-gravimetric-und-0-value').val('');
    }
  };

  /**
   * Sets value for J.
   */
  Drupal.dirtMonthlySurvey.setJ = function () {
    var I = $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-gravimetric-und-0-value').val();
    var H = $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-soil-bulk-density-und-0-value').val();
    var J;

    if (I && H) {
      J = I * H;
      J = Math.round(J * 100) / 100;
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-volumetric-und-0-value').val(J);
    }
    else {
      // One of I or H missing, so clear J:
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-volumetric-und-0-value').val('');
    }
  };

   /**
   * Sets value for K.
   */
  Drupal.dirtMonthlySurvey.setK = function () {
    var H = $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-soil-bulk-density-und-0-value').val();
    var K;
  
    if (H) {
      K = (1 - H/2.65) * 100;
      K = Math.round(K * 100) / 100;
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-total-soil-porosity-und-0-value').val(K);
    }
    else {
      // H missing, so clear K:
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-total-soil-porosity-und-0-value').val('');
    }
  };

  /**
   * Sets value for L.
   */
  Drupal.dirtMonthlySurvey.setL = function () {
    // Clear any warnings if they were triggered previously
    $('.water-filled-pores-warning').hide();

    var J = $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-volumetric-und-0-value').val();
    var K = $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-total-soil-porosity-und-0-value').val();
    var L;

    if (J && K) {
      L = (J/K) * 100;
      L = Math.round(L * 100) / 100;
      // If L > 110% leave blank and report message to user
      if (L > 110) {
        $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-filled-pores-und-0-value').val('');
        $('.water-filled-pores-warning').html("<b><i>% Water-Filled Pores > 110. Verify your measurements for A-F.</i></b>").fadeIn();
      }
      else {
        $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-filled-pores-und-0-value').val(L);
      }
    }
    else {
      // One of J or K missing, so clear L:
      $('#edit-field-dirt-bulk-density-calcs-und-0-field-dirt-water-filled-pores-und-0-value').val('');
    }
  };

})(jQuery);
