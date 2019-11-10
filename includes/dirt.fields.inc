<?php

/**
 * @file
 * This file specifies the definitions for custom fields created by this module.
 */

/**
 * Returns fields created by this module.
 * 
 * @retval Array of field info keyed by field name.
 */
function dirt_get_installed_fields() {
  // Variables for markup fields
  $site_name = variable_get('site_name');
  $dirt_images = '/' . drupal_get_path('module', 'dirt') . '/images/';

  // First line of all form headers:
  $form_header_markup  = '<h2>';
  $form_header_markup .= t('@sitename ', array('@sitename' => $site_name));
  $form_header_markup .= t('Citizen Science Project Soil Survey Form');
  $form_header_markup .= '</h2>';

  return array(
    'field_dirt_date_of_survey' => array(
      'field_name' => 'field_dirt_date_of_survey',
      'type' => 'datetime',
      'locked' => TRUE,
      'settings' => array(
        'granularity' => array(
          'year'  => 'year',
          'month' => 'month',
          'day'   => 'day',
        ),
      ),
    ),
    'field_dirt_form_header' => array(
      'field_name' => 'field_dirt_form_header',
      'type' => 'markup',
      'locked' => TRUE,
      'settings' => array(
        'markup' => array(
          'value' => $form_header_markup,
          'format' => 'full_html',
        ),
      ),
    ),
    'field_dirt_form_submission_note' => array(
      'field_name' => 'field_dirt_form_submission_note',
      'type' => 'markup',
      'locked' => TRUE,
      'settings' => array(
        'markup' => array(
          'value' => '<span id="dirt-submission-note-save">' . t('When you ' .
            'are finished filling out this form, click Save at the bottom of ' .
            'the page to submit your data.') . '</span><br><br>' .
            '<span id="dirt-submission-note-confirm">' . t('After submitting ' .
            'this survey, you will see a message in a green box near the top ' .
            'of the screen confirming that your data has been saved.') .
            '</span>',
          'format' => 'full_html',
        ),
      ),
    ),
    'field_dirt_site_address' => array(
      'field_name' => 'field_dirt_site_address',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_site_city' => array(
      'field_name' => 'field_dirt_site_city',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_site_county' => array(
      'field_name' => 'field_dirt_site_county',
      'type' => 'text',
    ),
    'field_dirt_site_state' => array(
      'field_name' => 'field_dirt_site_state',
      'type' => 'text',
    ),
    'field_dirt_site_zip_code' => array(
      'field_name' => 'field_dirt_site_zip_code',
      'type' => 'text',
    ),
    'field_dirt_site_mean_annual_temp' => array(
      'field_name' => 'field_dirt_site_mean_annual_temp',
      'type' => 'number_float',
      'settings' => array(
        'precision' => 10,
      ),
    ),
    'field_dirt_site_annual_precip' => array(
      'field_name' => 'field_dirt_site_annual_precip',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_site_elevation' => array(
      'field_name' => 'field_dirt_site_elevation',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_site_slope' => array(
      'field_name' => 'field_dirt_site_slope',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_site_aspect' => array(
      'field_name' => 'field_dirt_site_aspect',
      'type' => 'list_text',
      'cardinality' => 1,
      'settings' => array(
        'allowed_values' => array(
          'N' => 'N',
          'S' => 'S',
          'E' => 'E',
          'W' => 'W',
          'NE' => 'NE',
          'NW' => 'NW',
          'SE' => 'SE',
          'SW' => 'SW',
          'N/A' => 'N/A',
        ),
      ),
    ),
    'field_dirt_site_geo_coords' => array(
      'field_name' => 'field_dirt_site_geo_coords',
      'locked' => TRUE,
      'cardinality' => 5,
      'type' => 'field_collection',
    ),
    'field_dirt_replicate_number' => array(
      'field_name' => 'field_dirt_replicate_number',
      'type' => 'number_integer',
      'locked' => TRUE,
    ),
    'field_dirt_site_geo_coords_lat' => array(
      'field_name' => 'field_dirt_site_geo_coords_lat',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 6,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_site_geo_coords_lon' => array(
      'field_name' => 'field_dirt_site_geo_coords_lon',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 6,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_site_geo_coords_note' => array(
      'field_name' => 'field_dirt_site_geo_coords_note',
      'type' => 'markup',
      'settings' => array(
        'markup' => array(
          'value' => t('** Only include these coordinates if you cannot ' .
                       'leave the stakes in place due to management ' .
                       'practices.'),
          'format' => 'full_html',
        ),
      ),
    ),
    'field_dirt_site_map_unit_note' => array(
      'field_name' => 'field_dirt_site_map_unit_note',
      'type' => 'markup',
      'settings' => array(
        'markup' => array(
          'value' => t('Map Unit Name and Symbol<br><br>Visit the SoilWeb to ' .
                       'find the information (@soilweburl).', array(
            '@soilweburl' => 'http://casoilresource.lawr.ucdavis.edu/gmap/',
          )),
          'format' => 'full_html',
        ),
      ),
    ),
    'field_dirt_site_map_unit_name' => array(
      'field_name' => 'field_dirt_site_map_unit_name',
      'type' => 'text',
    ),
    'field_dirt_site_map_unit_symbol' => array(
      'field_name' => 'field_dirt_site_map_unit_symbol',
      'type' => 'number_integer',
    ),
    'field_dirt_site_landscape_posit' => array(
      'field_name' => 'field_dirt_site_landscape_posit',
      'type' => 'list_text',
      'cardinality' => 1,
      'settings' => array(
        'allowed_values' => array(
          'SU' => 'SU',
          'SH' => 'SH',
          'BS' => 'BS',
          'FS' => 'FS',
          'TS' => 'TS',
        ),
      ),
    ),
    'field_dirt_site_habitat_type' => array(
      'field_name' => 'field_dirt_site_habitat_type',
      'type' => 'text',
    ),
    'field_dirt_site_type_of_crop' => array(
      'field_name' => 'field_dirt_site_type_of_crop',
      'type' => 'text',
    ),
    'field_dirt_site_cropping_system' => array(
      'field_name' => 'field_dirt_site_cropping_system',
      'type' => 'text',
    ),
    'field_dirt_site_topography' => array(
      'field_name' => 'field_dirt_site_topography',
      'type' => 'text',
    ),
    // No longer in use
    //'field_dirt_site_signs_of_erosion' => array(
    //  'field_name' => 'field_dirt_site_signs_of_erosion',
    //  'type' => 'text',
    //),
    'field_dirt_site_present_manage' => array(
      'field_name' => 'field_dirt_site_present_manage',
      'type' => 'text',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    ),
    'field_dirt_site_past_manage' => array(
      'field_name' => 'field_dirt_site_past_manage',
      'type' => 'text',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    ),
    'field_dirt_site_manage_updates' => array(
      'field_name' => 'field_dirt_site_manage_updates',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'type' => 'field_collection',
    ),
    'field_dirt_site_update_date' => array(
      'field_name' => 'field_dirt_site_update_date',
      'type'     => 'datetime',
      'locked' => TRUE,
      'settings'   => array(
        'granularity' => array(
          'year'  => 'year',
          'month' => 'month',
        ),
      ),
    ),
    'field_dirt_site_update_notes' => array(
      'field_name' => 'field_dirt_site_update_notes',
      'type' => 'text_long',
      'locked' => TRUE,
    ),
    'field_dirt_site_layer_data' => array(
      'field_name' => 'field_dirt_site_layer_data',
      'cardinality' => 4,
      'type' => 'field_collection',
    ),
    'field_dirt_site_layer_number' => array(
      'field_name' => 'field_dirt_site_layer_number',
      'type' => 'number_integer',
      'locked' => TRUE,
    ),
    'field_dirt_site_layer_texture' => array(
      'field_name' => 'field_dirt_site_layer_texture',
      'type' => 'list_text',
      'cardinality' => 1,
      'settings' => array(
        'allowed_values' => array(
          'Sandy loam' => 'Sandy loam',
          'Loam' => 'Loam',
          'Silt loam' => 'Silt loam',
          'Sandy clay loam' => 'Sandy clay loam',
          'Clay loam' => 'Clay loam',
          'Silty clay loam' => 'Silty clay loam',
          'Sandy clay' => 'Sandy clay',
          'Clay' => 'Clay',
          'Silty clay' => 'Silty clay',
          'Sand' => 'Sand',
          'Loamy sand' => 'Loamy sand',
        )
      ),
    ),
    'field_dirt_site_layer_color' => array(
      'field_name' => 'field_dirt_site_layer_color',
      'type' => 'text',
    ),
    'field_dirt_site_drawing_guide' => array(
      'field_name' => 'field_dirt_site_drawing_guide',
      'type' => 'markup',
      'settings' => array(
        'markup' => array(
          'value' => t('<p>Aerial view of site showing replicates sites and ' .
                       'location of landmarks (river, lake, road, etc.) and ' .
                       'other features you think are important to include in ' .
                       'the survey. Use colors to illustrate different ' .
                       'features. Take a photo of your drawing and of the ' .
                       'dominant habitat of the survey site to be used ' .
                       'digitally. To upload the image of your drawing is ' .
                       'optional if you do not have a way to create a ' .
                       'digital image of it.</p>' .
                       '<a href="@url" target="_blank"><img src="@url"/></a>' . 
                       '<br><em>Click on the image to open a larger version ' .
                       'in a new window or tab.</em>', array(
            '@url' => $dirt_images . 'sitegrid.png',
          )),
          'format' => 'full_html',
        ),
      ),
    ),
    'field_dirt_site_drawing' => array(
      'field_name' => 'field_dirt_site_drawing',
      'type' => 'image',
    ),
    'field_dirt_site_drawing_scale' => array(
      'field_name' => 'field_dirt_site_drawing_scale',
      'type' => 'text',
    ),
    'field_dirt_site_habitat_photos' => array(
      'field_name' => 'field_dirt_site_habitat_photos',
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'type' => 'field_collection',
    ),
    'field_dirt_site_habitat_photo' => array(
      'field_name' => 'field_dirt_site_habitat_photo',
      'type' => 'image',
    ),
    'field_dirt_site_desc_notes' => array(
      'field_name' => 'field_dirt_site_desc_notes',
      'type' => 'text_long',
    ),
    'field_dirt_temp_cloud_cover' => array(
      'field_name' => 'field_dirt_temp_cloud_cover',
      'cardinality' => 5,
      'type' => 'field_collection',
    ),
    'field_dirt_air_temp' => array(
      'field_name' => 'field_dirt_air_temp',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_soil_temp_surface' => array(
      'field_name' => 'field_dirt_soil_temp_surface',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_soil_temp_5cm' => array(
      'field_name' => 'field_dirt_soil_temp_5cm',
       'type' => 'number_decimal',
      'locked' => TRUE,
       'settings' => array(
         'precision' => 10,
         'scale' => 2,
         'decimal_separator' => '.',
       ),
    ),
    'field_dirt_soil_temp_10cm' => array(
      'field_name' => 'field_dirt_soil_temp_10cm',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_avg_soil_temp_5cm' => array(
      'field_name' => 'field_dirt_avg_soil_temp_5cm',
      'type' => 'number_decimal',
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_cloud_cover' => array(
      'field_name' => 'field_dirt_cloud_cover',
      'type' => 'number_integer',
    ),
    'field_dirt_temp_cloud_notes' => array(
      'field_name' => 'field_dirt_temp_cloud_notes',
      'type' => 'text_long',
    ),
    'field_dirt_color_number_by_hour' => array(
      'field_name' => 'field_dirt_color_number_by_hour',
      'cardinality' => 8,
      'type' => 'field_collection',
    ),
    'field_dirt_soil_resp_hour' => array(
      'field_name' =>'field_dirt_soil_resp_hour',
      'type' => 'number_integer',
      'locked' => TRUE,
    ),
    'field_dirt_soil_resp_time' => array(
      'field_name' => 'field_dirt_soil_resp_time',
      'type'     => 'datetime',
      'locked' => TRUE,
      'settings'   => array(
        'granularity' => array(
          'year'  => 'year',
          'hour' => 'hour',
          'minute'   => 'minute',
        ),
      ),
    ),
    'field_dirt_soil_resp_color' => array(
      'field_name' => 'field_dirt_soil_resp_color',
      'type' => 'list_float',
      'locked' => TRUE,
      'settings' => array(
        'allowed_values' => array(
          '0' => 0,
          '1' => 1,
          '2' => 2,
          '2.5' => 2.5,
          '3' => 3,
          '3.5' => 3.5,
          '4' => 4,
          '5' => 5,
          '6' => 6,
        ),
      ),
    ),
    'field_dirt_co2c_room_temp_range' => array(
      'field_name' => 'field_dirt_co2c_room_temp_range',
      'type' => 'text',
    ),
    'field_dirt_co2c_room_temp_lower' => array(
      'field_name' => 'field_dirt_co2c_room_temp_lower',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_co2c_room_temp_upper' => array(
      'field_name' => 'field_dirt_co2c_room_temp_upper',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_co2c_room_temp_avg' => array(
      'field_name' => 'field_dirt_co2c_room_temp_avg',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_co2c_field_temp_range' => array(
      'field_name' => 'field_dirt_co2c_field_temp_range',
      'type' => 'text',
    ),
    'field_dirt_co2c_field_temp_lower' => array(
      'field_name' => 'field_dirt_co2c_field_temp_lower',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_co2c_field_temp_upper' => array(
      'field_name' => 'field_dirt_co2c_field_temp_upper',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_co2c_field_temp_avg' => array(
      'field_name' => 'field_dirt_co2c_field_temp_avg',
      'type' => 'number_float',
      'settings' => array(
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_soil_resp_notes' => array(
      'field_name' => 'field_dirt_soil_resp_notes',
      'type' => 'text_long',
    ),
    'field_dirt_sample_before_drying' => array(
      'field_name' => 'field_dirt_sample_before_drying',
      'cardinality' => 1,
      'type' => 'field_collection',
    ),
    'field_dirt_weight_wet_soil_bag' => array(
      'field_name' => 'field_dirt_weight_wet_soil_bag',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_weight_bag' => array(
      'field_name' => 'field_dirt_weight_bag',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_weight_wet_soil' => array(
      'field_name' => 'field_dirt_weight_wet_soil',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_sample_after_drying' => array(
      'field_name' => 'field_dirt_sample_after_drying',
      'cardinality' => 1,
      'type' => 'field_collection',
    ),
    'field_dirt_weight_dry_soil_bowl' => array(
      'field_name' => 'field_dirt_weight_dry_soil_bowl',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_weight_bowl' => array(
      'field_name' => 'field_dirt_weight_bowl',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_weight_dry_soil' => array(
      'field_name' => 'field_dirt_weight_dry_soil',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_container_weight_note' => array(
      'field_name' => 'field_dirt_container_weight_note',
      'type' => 'markup',
      'settings' => array(
        'markup' => array(
          'value' => t('** If you use the compostable bowls to dry the soil, ' .
                       'use the average weight of the five bowls as the ' .
                       '"Weight of container" (E).'),
          'format' => 'full_html',
        ),
      ),
    ),
    'field_dirt_bulk_density_calcs' => array(
      'field_name' => 'field_dirt_bulk_density_calcs',
      'type' => 'field_collection',
    ),
    'field_dirt_volume_soil_sample' => array(
      'field_name' => 'field_dirt_volume_soil_sample',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_soil_bulk_density' => array(
      'field_name' => 'field_dirt_soil_bulk_density',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_water_gravimetric' => array(
      'field_name' => 'field_dirt_water_gravimetric',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_water_volumetric' => array(
      'field_name' => 'field_dirt_water_volumetric',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_total_soil_porosity' => array(
      'field_name' => 'field_dirt_total_soil_porosity',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_water_filled_pores' => array(
      'field_name' => 'field_dirt_water_filled_pores',
      'type' => 'number_decimal',
      'locked' => TRUE,
      'settings' => array(
        'precision' => 10,
        'scale' => 2,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_bulk_density_notes' => array(
      'field_name' => 'field_dirt_bulk_density_notes',
      'type' => 'text_long',
    ),
    'field_dirt_soil_n' => array(
      'field_name' => 'field_dirt_soil_n',
      'type' => 'list_text',
      'settings' => array(
        'allowed_values' => array(
          'N0 - 0 - Depleted',
          'N1 - 10 - Deficient',
          'N2 - 20 - Adequate',
          'N3 - 40 - Sufficient',
          'N4 - 80 - Surplus',
        ),
      ),
    ),
    'field_dirt_soil_n_value' => array(
      'field_name' => 'field_dirt_soil_n_value',
      'type' => 'text',
    ),
    'field_dirt_soil_n_ppm' => array(
      'field_name' => 'field_dirt_soil_n_ppm',
      'type' => 'number_integer',
    ),
    'field_dirt_soil_n_category' => array(
      'field_name' => 'field_dirt_soil_n_category',
      'type' => 'text',
    ),
    'field_dirt_soil_p' => array(
      'field_name' => 'field_dirt_soil_p',
      'type' => 'list_text',
      'settings' => array(
        'allowed_values' => array(
          'P0 - 5 - Depleted',
          'P1 - 10 - Deficient',
          'P2 - 20 - Adequate',
          'P3 - 50 - Sufficient',
          'P4 - 100 - Surplus',
        ),
      ),
    ),
    'field_dirt_soil_p_value' => array(
      'field_name' => 'field_dirt_soil_p_value',
      'type' => 'text',
    ),
    'field_dirt_soil_p_ppm' => array(
      'field_name' => 'field_dirt_soil_p_ppm',
      'type' => 'number_integer',
    ),
    'field_dirt_soil_p_category' => array(
      'field_name' => 'field_dirt_soil_p_category',
      'type' => 'text',
    ),
    'field_dirt_soil_k' => array(
      'field_name' => 'field_dirt_soil_k',
      'type' => 'list_text',
      'settings' => array(
        'allowed_values' => array(
          'K0 - 50 - Depleted',
          'K1 - 200 - Deficient',
          'K2 - 400 - Adequate',
          'K3 - 600 - Sufficient',
          'K4 - 900 - Surplus',
        ),
      ),
    ),
    'field_dirt_soil_k_value' => array(
      'field_name' => 'field_dirt_soil_k_value',
      'type' => 'text',
    ),
    'field_dirt_soil_k_ppm' => array(
      'field_name' => 'field_dirt_soil_k_ppm',
      'type' => 'number_integer',
    ),
    'field_dirt_soil_k_category' => array(
      'field_name' => 'field_dirt_soil_k_category',
      'type' => 'text',
    ),
    'field_dirt_soil_ph' => array(
      'field_name' => 'field_dirt_soil_ph',
      'type' => 'list_text',
      'settings' => array(
        'allowed_values' => array(
          '4.5 (Very Acid)',
          '5.0 (Very Acid)',
          '5.5 (Acid)',
          '6.0 (Acid)',
          '6.5 (Slight Acid)',
          '7.0 (Neutral)',
          '7.5 (Alkaline)',
        ),
      ),
    ),
    'field_dirt_soil_ph_value' => array(
      'field_name' => 'field_dirt_soil_ph_value',
      'type' => 'number_decimal',
      'settings' => array(
        'precision' => 10,
        'scale' => 1,
        'decimal_separator' => '.',
      ),
    ),
    'field_dirt_soil_ph_category' => array(
      'field_name' => 'field_dirt_soil_ph_category',
      'type' => 'text',
    ),
    'field_dirt_soil_active_carbon' => array(
      'field_name' => 'field_dirt_soil_active_carbon',
      'type' => 'list_text',
      'settings' => array(
        'allowed_values' => array(
          '>0 - 232 (Poor)',
          '>232 - 464 (Fair)',
          '>464 - 928 (Good)',
          '>928 (Excellent)',
        ),
      ),
    ),
    'field_dirt_soil_ac_range' => array(
      'field_name' => 'field_dirt_soil_ac_range',
      'type' => 'text',
    ),
    'field_dirt_soil_quality' => array(
      'field_name' => 'field_dirt_soil_quality',
      'type' => 'text',
    ),
    'field_dirt_soil_organic_matter' => array(
      'field_name' => 'field_dirt_soil_organic_matter',
      'type' => 'list_text',
      'settings' => array(
        'allowed_values' => array(
          '>0 - 400',
          '>400 - 800',
          '>800 - 1600',
          '>1600',
        ),
      ),
    ),
    'field_dirt_chem_soil_notes' => array(
      'field_name' => 'field_dirt_chem_soil_notes',
      'type' => 'text_long',
    ),
    'field_dirt_group_name' => array(
      'field_name' => 'field_dirt_group_name',
      'type' => 'text',
    ),
    'field_dirt_group_participants' => array(
      'field_name' => 'field_dirt_group_participants',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_group_institution' => array(
      'field_name' => 'field_dirt_group_institution',
      'type' => 'text',
    ),
    'field_dirt_group_leader_name' => array(
      'field_name' => 'field_dirt_group_leader_name',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_group_leader_email' => array(
      'field_name' => 'field_dirt_group_leader_email',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_group_leader_phone' => array(
      'field_name' => 'field_dirt_group_leader_phone',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_user_first_name' => array(
      'field_name' => 'field_dirt_user_first_name',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_user_last_name' => array(
      'field_name' => 'field_dirt_user_last_name',
      'type' => 'text',
      'settings' => array(
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_user_collection_state' => array(
      'field_name' => 'field_dirt_user_collection_state',
      'type' => 'list_text',
      'cardinality' => 1,
      'locked' => TRUE,
      'settings' => array(
        'allowed_values' => array(
          'active' => 'Active',
          'closed' => 'Closed',
        ),
        'dirt_admin_only' => array(
          'edit' => TRUE,
          'view' => TRUE,
        ),
      ),
    ),
    'field_dirt_user_collection_start' => array(
      'field_name' => 'field_dirt_user_collection_start',
      'type' => 'datetime',
      'locked' => TRUE,
      'settings' => array(
        'granularity' => array(
          'year'  => 'year',
          'month' => 'month',
          'day'   => 'day',
        ),
        'dirt_admin_only' => array(
          'edit' => TRUE,
          'view' => TRUE,
        ),
      ),
    ),
    'field_dirt_user_organization' => array(
      'field_name' => 'field_dirt_user_organization',
      'type' => 'text',
    ),
    'field_dirt_user_county' => array(
      'field_name' => 'field_dirt_user_county',
      'type' => 'text',
    ),
    'field_dirt_user_account_reason' => array(
      'field_name' => 'field_dirt_user_account_reason',
      'type' => 'text_long',
      'settings' => array(
        'dirt_admin_only' => array(
          'edit' => TRUE,
          'view' => TRUE,
        ),
      ),
    ),
    'field_dirt_user_sharing_perms' => array(
      'field_name' => 'field_dirt_user_sharing_perms',
      'type' => 'list_text',
      'cardinality' => 1,
      'locked' => TRUE,
      'settings' => array(
        'allowed_values' => array(
          'public' => t('<strong>Public: </strong>Accessible to the public ' .
                        'through this web (no login required). Your name, ' .
                        'e-mail address, site address (street and city), and ' .
                        'other contact information will remain private. ' .
                        '(Default)'),
          'semipublic' => t('<strong>Semipublic: </strong>Accessible only to ' .
                            'other participants of the soil health surveys ' .
                            '(login required). Your name, address, site ' .
                            'address (street and city), and other contact ' .
                            'information will remain private.'),
          'semiprivate' => t('<strong>Semiprivate: </strong>Accessible ' .
                             'neither to the public nor with other soil ' .
                             'health survey participants.'),
        ),
        'dirt_private_field' => TRUE,
      ),
    ),
    'field_dirt_user_admin_notes' => array(
      'field_name' => 'field_dirt_user_admin_notes',
      'type' => 'text_long',
      'settings' => array(
        'dirt_admin_only' => array(
          'edit' => TRUE,
          'view' => TRUE,
        ),
      ),
    ),
  );
}


/**
 * Returns field instances added by this module.
 * 
 * @retval Array of field instances.
 */
function dirt_get_installed_instances() {
  $instances = array();

  // Fields common to all surveys will be added at the top (using negative weights)
  // Set weight for admin-only survey fields group to high number (98) to drop it to the bottom
  variable_set('dirt_group_dirt_admin_only_weight', 98);

  /*--------------------------------*/
  /* Site Description Survey Fields */
  /*--------------------------------*/
  $bundle = 'dirt_site_description_survey';

  // Keep weight counters so that new fields can be easily be added without
  // manually updating weight numbers
  $weight_in_form = 0;

  // Site Address Group Begins Here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_address_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_address'] = array(
    'field_name' => 'field_dirt_site_address',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Address (Street No. and Name)'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('e.g., 123 Main St.'),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_city'] = array(
    'field_name' => 'field_dirt_site_city',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('City'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_county'] = array(
    'field_name' => 'field_dirt_site_county',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('County'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_state'] = array(
    'field_name' => 'field_dirt_site_state',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('State'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_zip_code'] = array(
    'field_name' => 'field_dirt_site_zip_code',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Zip Code'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );

  // Site Description group begins here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_desc_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_mean_annual_temp'] = array(
    'field_name' => 'field_dirt_site_mean_annual_temp',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Mean Annual Temperature (°C)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'max' => 50,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_annual_precip'] = array(
    'field_name' => 'field_dirt_site_annual_precip',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Total Annual Precipitation (mm)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 1500,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_elevation'] = array(
    'field_name' => 'field_dirt_site_elevation',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Elevation (m)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'max' => 2000,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_slope'] = array(
    'field_name' => 'field_dirt_site_slope',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Slope (%)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 100,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_aspect'] = array(
    'field_name' => 'field_dirt_site_aspect',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Aspect'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  // Geo coords field group goes here
  // Assign its weight according to $weight_in_group
  variable_set('dirt_group_dirt_site_coords_weight', $weight_in_group);
  $weight_in_group += 1;

  // New weight counter for inside geo field group
  $weight_in_geo_group = 0;

  $instances['field_dirt_site_geo_coords'] = array(
    'field_name' => 'field_dirt_site_geo_coords',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Geographical Coordinates in Decimal Degrees'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_geo_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_geo_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );
  $weight_in_geo_group += 1;

  // Next fields belong to geo coords field collection 
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  // Multiple instances of this field; append FC (bundle) name to instance key
  $instances['field_dirt_replicate_number_field_dirt_site_geo_coords'] = array(
    'field_name' => 'field_dirt_replicate_number',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_geo_coords',
    'type' => 'number_integer',
    'label' => t('Replicate Number'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 5,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_site_geo_coords_lat'] = array(
    'field_name' => 'field_dirt_site_geo_coords_lat',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_geo_coords',
    'type' => 'number_decimal',
    'label' => t('Latitude'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => -90,
      'max' => 90,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 6,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_site_geo_coords_lon'] = array(
    'field_name' => 'field_dirt_site_geo_coords_lon',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_geo_coords',
    'type' => 'number_decimal',
    'label' => t('Longitude'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => -180,
      'max' => 180,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 6,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  // Next field in geo coords field group
  $instances['field_dirt_site_geo_coords_note'] = array(
    'field_name' => 'field_dirt_site_geo_coords_note',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Geographic Coordinates Note'),
    'widget' => array(
      'type'   => 'markup',
      'weight' => $weight_in_geo_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'weight' => $weight_in_geo_group,
      ),
    ),
  );

  // Next field in field group
  $instances['field_dirt_site_map_unit_note'] = array(
    'field_name' => 'field_dirt_site_map_unit_note',
    'entity_type' => 'node',
    'bundle' => $bundle, 
    'label' => t('Map Unit Web Note'),
    'widget' => array(
      'type'   => 'markup',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_map_unit_name'] = array(
    'field_name' => 'field_dirt_site_map_unit_name',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Map Unit Name'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_map_unit_symbol'] = array(
    'field_name' => 'field_dirt_site_map_unit_symbol',
    'type' => 'number_integer',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Map Unit Symbol'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'min' => 10000,
      'max' => 99999,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline', 
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  // Site description continuation field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_desc_con_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_landscape_posit'] = array(
    'field_name' => 'field_dirt_site_landscape_posit',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Study Site Landscape Position'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  // Insert habitat type field group here
  variable_set('dirt_group_dirt_site_habitat_weight', $weight_in_group);
  $weight_in_group += 1;

  // Habitat type fields here, count weight from zero
  // (for form - continue using group weight for display)
  $weight_in_habitat_group = 0;

  $instances['field_dirt_site_habitat_type'] = array(
    'field_name' => 'field_dirt_site_habitat_type',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Habitat Type'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'select_or_other',
      'weight' => $weight_in_habitat_group,
      'settings' => array(
        'available_options' => implode(array(
          'Forest',
          'Woodland',
          'Prairie',
          'Grassland',
          'Cropland tillage',
          'Cropland no-tillage',
          'Lawn',
          'Animal pasture'
        ), "\n"),
        'other' => 'Other:',
        'other_size' => 60,
        'sort_options' => FALSE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_habitat_group += 1;
  $weight_in_group += 1;

  $instances['field_dirt_site_type_of_crop'] = array(
    'field_name' => 'field_dirt_site_type_of_crop',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('If Cropland, Include Type of Crop'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_habitat_group,
    ),
    'description' => t('Soy, corn, sorghum, alfalfa, etc.'),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_habitat_group += 1;
  $weight_in_group += 1;

  $instances['field_dirt_site_cropping_system'] = array(
    'field_name' => 'field_dirt_site_cropping_system',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('If Cropland, Include Cropping System'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_habitat_group,
    ),
    'description' => t('Rotation, cover crops, etc.'),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_habitat_group += 1;
  $weight_in_group += 1;

  // Back to site description continuation group, resume counting weight with 
  // $weight_in_group
  $instances['field_dirt_site_topography'] = array(
    'field_name' => 'field_dirt_site_topography',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Topography'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('Hills, ridges, depressions, knolls, potholes, etc.'),
  );
  $weight_in_group += 1;

  /*
  $instances['field_dirt_site_signs_of_erosion'] = array(
    'field_name' => 'field_dirt_site_signs_of_erosion',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Signs of Erosion'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'select_or_other',
      'weight' => $weight_in_group,
      'settings' => array(
        'available_options' => implode(array(
          'Exposed subsoil',
          'Gullies',
          'Rills',
          'Pedestals',
          'Wind blow impact',
          'None'
        ), "\n"),
        'other' => 'Other:',
        'other_size' => 60,
        'sort_options' => FALSE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;
  */

  $instances['field_dirt_site_present_manage'] = array(
    'field_name' => 'field_dirt_site_present_manage',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Present Land Management'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'select_or_other_buttons',
      'weight' => $weight_in_group,
      'settings' => array(
        'available_options' => implode(array(
          'Logging',
          'Reforestation',
          'Pesticides',
          'Fertilizers',
          'Irrigation',
          'Tourism',
          'Prescribed burns',
          'Flooding',
          'Undisturbed'
        ), "\n"),
        'other' => 'Other:',
        'other_size' => 60,
        'sort_options' => FALSE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_past_manage'] = array(
    'field_name' => 'field_dirt_site_past_manage',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Past Land Management History'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'select_or_other_buttons',
      'weight' => $weight_in_group,
      'settings' => array(
        'available_options' => implode(array(
          'Logging',
          'Reforestation',
          'Pesticides',
          'Fertilizers',
          'Irrigation',
          'Tourism',
          'Prescribed burns',
          'Flooding'
        ), "\n"),
        'other' => 'Other:',
        'other_size' => 60,
        'sort_options' => FALSE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('Select an option from the list or choose "other" to ' .
                       'enter custom text.'),
  );
  $weight_in_group += 1;

  // Land management updates field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_updates_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_manage_updates'] = array(
    'field_name' => 'field_dirt_site_manage_updates',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Land Management Updates'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => TRUE,
        ),
      ),
    ),
  );

  // Next fields belong to land management updates field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  $instances['field_dirt_site_update_date'] = array(
    'field_name' => 'field_dirt_site_update_date',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_manage_updates',
    'type'  => 'datetime',
    'label' => t('Date'),
    'widget' => array(
      'type' => 'date_popup',
      'weight' => $weight_in_fc,
      'module' => 'date',
      'settings' => array(
        'input_format' => 'custom',
        'input_format_custom' => 'm/Y',
        'no_fieldset' => 1,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
        'type' => 'date_default',
        'format_type' => DIRT_SURVEY_DATE_SHORT_TYPE,
        'settings' => array(
           'format_type' => DIRT_SURVEY_DATE_SHORT_TYPE,
        ),
      ),
    ),
    'settings' => array(
      'default_value' => 'blank',
      'default_format' => DIRT_SURVEY_DATE_TYPE,
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_site_update_notes'] = array(
    'field_name' => 'field_dirt_site_update_notes',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_manage_updates',
    'type'  => 'text_long',
    'label' => t('Describe the changes in land management (e.g., prairie ' .
                 'burn, flooding, tree fall, etc.)'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_fc,
      'settings' => array(
        'rows' => 3,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
        'type' => 'text_default',
      ),
    ),
  );

  // Soil core variables field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_soilcore_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_layer_data'] = array(
    'field_name' => 'field_dirt_site_layer_data',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Layer Data'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );

  // Next fields belong to soil core vars data field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  $instances['field_dirt_site_layer_number'] = array(
    'field_name' => 'field_dirt_site_layer_number',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_layer_data',
    'type' => 'number_integer',
    'label' => t('Layer'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 1,
      'max' => 4,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_site_layer_texture'] = array(
    'field_name' => 'field_dirt_site_layer_texture',
    'type' => 'list_text',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_layer_data',
    'label' => t('Texture'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_site_layer_color'] = array(
    'field_name' => 'field_dirt_site_layer_color',
    'type' => 'text',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_layer_data',
    'label' => t('Color'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );

  // Back to overall form, resume counting at $weight_in_form

  // Site drawing field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_drawing_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_drawing_guide'] = array(
    'field_name' => 'field_dirt_site_drawing_guide',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Drawing of Site Directions and Grid'),
    'widget' => array(
      'type'   => 'markup',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_drawing'] = array(
    'field_name' => 'field_dirt_site_drawing',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'type' => 'image',
    'label' => t('Drawing of Site'),
    'widget' => array(
      'type' => 'image_image',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'default_image' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'image',
        'weight' => $weight_in_group,
        'settings' => array(
          'image_style' => 'large',
          'image_link' => '',
        ),
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_site_drawing_scale'] = array(
    'field_name' => 'field_dirt_site_drawing_scale',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Scale 1 inch = ____ ft'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('Enter the number of feet represented by 1 inch (two ' .
                       'squares in the grid) or enter "NA" to indicate that ' .
                       'the sketch is not to scale.'),
  );

  // Back to overall form, resume counting at $weight_in_form

  // Site photo(s) field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_site_photos_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_site_habitat_photos'] = array(
    'field_name' => 'field_dirt_site_habitat_photos',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Survey Site Habitat Photo(s)'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );
  $weight_in_group += 1;

  // First and only field in field collection:
  $instances['field_dirt_site_habitat_photo'] = array(
    'field_name' => 'field_dirt_site_habitat_photo',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_site_habitat_photos',
    'type' => 'image',
    'label' => t('Photo'),
    'widget' => array(
      'type' => 'image_image',
      'weight' => 0,
    ),
    'settings' => array(
      'default_image' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'image',
        'weight' => 0,
        'settings' => array(
          'image_style' => 'large',
          'image_link' => '',
        ),
      ),
    ),
  );

  // Back to overall form, resume counting at $weight_in_form

  $instances['field_dirt_site_desc_notes'] = array(
    'field_name' => 'field_dirt_site_desc_notes',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'type'  => 'text_long',
    'label' => t('Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_form,
      'settings' => array(
        'rows' => 5,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_form,
        'type' => 'text_default',
      ),
    ),
  );
  $weight_in_form += 1;


  /*----------------------------*/
  /* Monthly Data Survey Fields */
  /*----------------------------*/
  $bundle = 'dirt_monthly_data_survey';

  // Form weight counter
  $weight_in_form = 0;

  // Temperatures and Cloud Cover Group Begins Here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_temp_cloud_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_temp_cloud_cover'] = array(
    'field_name' => 'field_dirt_temp_cloud_cover',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Temp and Cloud Cover Replicate'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );
  $weight_in_group += 1;

  // Next fields belong to temp and cloud cover field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  // Multiple instances of this field; append FC (bundle) name to instance key
  $instances['field_dirt_replicate_number_field_dirt_temp_cloud_cover'] = array(
    'field_name' => 'field_dirt_replicate_number',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_temp_cloud_cover',
    'type' => 'number_integer',
    'label' => t('Replicate Number'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 1,
      'max' => 5,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_air_temp'] = array(
    'field_name' => 'field_dirt_air_temp',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_temp_cloud_cover',
    'type' => 'number_decimal',
    'label' => t('Air Temperature (°C)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => -10,
      'max' => 45,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_soil_temp_surface'] = array(
    'field_name' => 'field_dirt_soil_temp_surface',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_temp_cloud_cover',
    'type' => 'number_decimal',
    'label' => t('Soil Temperature (°C) at Surface (0 cm Depth)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => -10,
      'max' => 45,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_soil_temp_5cm'] = array(
    'field_name' => 'field_dirt_soil_temp_5cm',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_temp_cloud_cover',
    'type' => 'number_decimal',
    'label' => t('Soil Temperature (°C) at 5 cm Depth'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => -10,
      'max' => 45,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_soil_temp_10cm'] = array(
    'field_name' => 'field_dirt_soil_temp_10cm',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_temp_cloud_cover',
    'type' => 'number_decimal',
    'label' => t('Soil Temperature (°C) at 10 cm Depth'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => -10,
      'max' => 45,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );

  // Resume counting at $weight_in_group

  $instances['field_dirt_avg_soil_temp_5cm'] = array(
    'field_name' => 'field_dirt_avg_soil_temp_5cm',
    'type' => 'number_decimal',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Average Soil Temperature (°C) at 5 cm Depth'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'min' => -10,
      'max' => 45,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('(Automatically Calculated)'),
  );
  $weight_in_group += 1;

  $instances['field_dirt_cloud_cover'] = array(
    'field_name' => 'field_dirt_cloud_cover',
    'type' => 'number_integer',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Cloud Cover (%)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_group,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 100,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_temp_cloud_notes'] = array(
    'field_name' => 'field_dirt_temp_cloud_notes',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'type'  => 'text_long',
    'label' => t('Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_group,
      'settings' => array(
        'rows' => 5,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_group,
        'type' => 'text_default',
      ),
    ),
  );

  // Soil respiration field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_soil_resp_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_color_number_by_hour'] = array(
    'field_name' => 'field_dirt_color_number_by_hour',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Color Number By Hour'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );
  $weight_in_group += 1;

  // Next fields belong to soil respiration field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;
  
  $instances['field_dirt_soil_resp_hour'] = array(
    'field_name' => 'field_dirt_soil_resp_hour',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_color_number_by_hour',
    'type' => 'number_integer',
    'label' => t('Hour (h)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 24,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_soil_resp_time'] = array(
    'field_name' => 'field_dirt_soil_resp_time',
    'type'  => 'datetime',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_color_number_by_hour',
    'label' => t('Time'),
    'widget' => array(
      'type' => 'date_text',
      'weight' => $weight_in_fc,
      'settings' => array(
        'input_format' => 'custom',
        'input_format_custom' => DIRT_RESP_TIME_FORMAT,
        'no_fieldset' => 1,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
        'type' => 'date_default',
        'format_type' => DIRT_RESP_TIME_TYPE,
        'settings' => array(
           'format_type' => DIRT_RESP_TIME_TYPE,
        ),
      ),
    ),
    'settings' => array(
      'default_value' => 'blank',
      'default_format' => DIRT_RESP_TIME_TYPE,
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_soil_resp_color'] = array(
    'field_name' => 'field_dirt_soil_resp_color',
    'type' => 'list_float',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_color_number_by_hour',
    'label' => t('Color'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );

  // Back to soil resp group, resume counting with $weight_in_group

  // Insert interpretation of colors for CO2-C emissions field group here
  variable_set('dirt_group_dirt_co2c_weight', $weight_in_group);
  $weight_in_group += 1;

  // CO2-C emissions fields here, count weight from zero
  $weight_in_emissions_group = 0;

  $room_temp_description  = t('(Automatically calculated)');
  $room_temp_description .= '<br><br>';
  $room_temp_description .= t('CO₂-C emissions (lbs/acre/day) at room ' .
                              'temperature (Table 6 in protocol).');
  $room_temp_description .= '<br>';
  $room_temp_description .= t('** If by 5 hours into the experiment the ' .
                              'number 3 or higher is recorded, the soil is ' .
                              'considered a fast riser. Consequently, the ' .
                              'CO₂-C at 24 hours is automatically ' .
                              'calculated based on the color given at 5 ' .
                              'hours (see the soil respiration protocol, ' .
                              'page 46).');

  $instances['field_dirt_co2c_room_temp_range'] = array(
    'field_name' => 'field_dirt_co2c_room_temp_range',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Room Temperature'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
    'description' => $room_temp_description,
  );
  $weight_in_emissions_group += 1;

  unset($room_temp_description);

  $instances['field_dirt_co2c_room_temp_lower'] = array(
    'field_name' => 'field_dirt_co2c_room_temp_lower',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Room Temperature Lower End'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
  );
  $weight_in_emissions_group += 1;

  $instances['field_dirt_co2c_room_temp_upper'] = array(
    'field_name' => 'field_dirt_co2c_room_temp_upper',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Room Temperature Upper End'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
  );
  $weight_in_emissions_group += 1;

  $instances['field_dirt_co2c_room_temp_avg'] = array(
    'field_name' => 'field_dirt_co2c_room_temp_avg',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Room Temperature Average'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
  );
  $weight_in_emissions_group += 1;

  $field_temp_description  = t('(Automatically calculated)');
  $field_temp_description .= '<br><br>';
  $field_temp_description .= t('CO₂-C emissions (lbs/acre/day) at field soil ' .
                               'temperature (Table 7 in protocol).');
  $instances['field_dirt_co2c_field_temp_range'] = array(
    'field_name' => 'field_dirt_co2c_field_temp_range',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Field Soil Temperature'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
    'description' => $field_temp_description,
  );
  $weight_in_emissions_group += 1;

  unset($field_temp_description);

  $instances['field_dirt_co2c_field_temp_lower'] = array(
    'field_name' => 'field_dirt_co2c_field_temp_lower',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Field Soil Temperature Lower End'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
  );
  $weight_in_emissions_group += 1;

  $instances['field_dirt_co2c_field_temp_upper'] = array(
    'field_name' => 'field_dirt_co2c_field_temp_upper',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Field Soil Temperature Upper End'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
  );
  $weight_in_emissions_group += 1;

  $instances['field_dirt_co2c_field_temp_avg'] = array(
    'field_name' => 'field_dirt_co2c_field_temp_avg',
    'type' => 'number_float',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('CO₂-C Emissions at Field Soil Temperature Average'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_emissions_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_emissions_group,
      ),
    ),
  );
  $weight_in_emissions_group += 1;

  // Back to soil resp group, resume counting with $weight_in_group

  $instances['field_dirt_soil_resp_notes'] = array(
    'field_name' => 'field_dirt_soil_resp_notes',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'type'  => 'text_long',
    'label' => t('Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_group,
      'settings' => array(
        'rows' => 6,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_group,
        'type' => 'text_default',
      ),
    ),
  );

  // Bulk density calcs field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_bulk_density_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  // Sample before drying field group goes here
  // Assign its weight according to $weight_in_group
  variable_set('dirt_group_dirt_before_drying_weight', $weight_in_group);
  $weight_in_group += 1;

  // New weight counter for inside sample before drying group
  $weight_in_before_drying_group = 0;

  $instances['field_dirt_sample_before_drying'] = array(
    'field_name' => 'field_dirt_sample_before_drying',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Sample Before Drying Weights Replicate'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_before_drying_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_before_drying_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );

  // Next fields belong to sample before drying field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  // Multiple instances of this field; append FC (bundle) name to instance key
  $instances['field_dirt_replicate_number_field_dirt_sample_before_drying'] = array(
    'field_name' => 'field_dirt_replicate_number',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_before_drying',
    'type' => 'number_integer',
    'label' => t('Composite soil sample'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 3,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_weight_wet_soil_bag'] = array(
    'field_name' => 'field_dirt_weight_wet_soil_bag',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_before_drying',
    'label' => t('Weight of Wet Soil + Bag (g)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_weight_bag'] = array(
    'field_name' => 'field_dirt_weight_bag',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_before_drying',
    'label' => t('Weight of Bag (g)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_weight_wet_soil'] = array(
    'field_name' => 'field_dirt_weight_wet_soil',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_before_drying',
    'label' => t('Weight of Wet Soil (g)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );

  // Sample after drying field group goes here
  // Assign its weight according to $weight_in_group
  variable_set('dirt_group_dirt_after_drying_weight', $weight_in_group);
  $weight_in_group += 1;

  // New weight counter for inside sample after drying group
  $weight_in_after_drying_group = 0;

  $instances['field_dirt_sample_after_drying'] = array(
    'field_name' => 'field_dirt_sample_after_drying',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Sample After Drying Weights Replicate'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_after_drying_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_after_drying_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );
  $weight_in_after_drying_group += 1;

  // Next fields belong to sample after drying field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  // Multiple instances of this field; append FC (bundle) name to instance key
  $instances['field_dirt_replicate_number_field_dirt_sample_after_drying'] = array(
    'field_name' => 'field_dirt_replicate_number',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_after_drying',
    'type' => 'number_integer',
    'label' => t('Composite soil sample'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 3,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_weight_dry_soil_bowl'] = array(
    'field_name' => 'field_dirt_weight_dry_soil_bowl',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_after_drying',
    'label' => t('Weight of Dry Soil + Container (g)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_weight_bowl'] = array(
    'field_name' => 'field_dirt_weight_bowl',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_after_drying',
    'label' => t('Weight of Container (g) **'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_weight_dry_soil'] = array(
    'field_name' => 'field_dirt_weight_dry_soil',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_sample_after_drying',
    'label' => t('Weight of Dry Soil (g)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  // End of fc, resume counting weight with $weight_in_after_drying_group

  $instances['field_dirt_container_weight_note'] = array(
    'field_name' => 'field_dirt_container_weight_note',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Container Weight Note'),
    'widget' => array(
      'type'   => 'markup',
      'weight' => $weight_in_after_drying_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'markup_view',
        'module' => 'markup',
        'weight' => $weight_in_after_drying_group,
      ),
    ),
  );

  // End of after drying group, resume counting weight with $weight_in_group

  $instances['field_dirt_bulk_density_calcs'] = array(
    'field_name' => 'field_dirt_bulk_density_calcs',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Bulk Density etc. Calculations'),
    'widget' => array(
      'type'   => 'field_collection_table',
      'weight' => $weight_in_group,
      'settings' => array(
        'nodragging' => TRUE,
        'title_on_top' => TRUE,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'field_collection_table_view',
        'weight' => $weight_in_group,
        'settings' => array(
          'edit' => '',
          'delete' => '',
          'add' => '',
          'hide_empty' => TRUE,
          'empty' => FALSE,
        ),
      ),
    ),
  );
  $weight_in_group += 1;

  // Next fields belong to bulk density etc. calculations field collection
  // New weight counter for inside field collection
  $weight_in_fc = 0;

  // Multiple instances of this field; append FC (bundle) name to instance key
  $instances['field_dirt_replicate_number_field_dirt_bulk_density_calcs'] = array(
    'field_name' => 'field_dirt_replicate_number',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'type' => 'number_integer',
    'label' => t('Composite soil sample'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
      'max' => 3,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_volume_soil_sample'] = array(
    'field_name' => 'field_dirt_volume_soil_sample',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'label' => t('Volume of Soil Sample (cm³)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_soil_bulk_density'] = array(
    'field_name' => 'field_dirt_soil_bulk_density',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'label' => t('Soil Bulk Density (g/cm³)'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'settings' => array(
      'min' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_water_gravimetric'] = array(
    'field_name' => 'field_dirt_water_gravimetric',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'label' => t('% Gravimetric Soil Water Content'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_water_volumetric'] = array(
    'field_name' => 'field_dirt_water_volumetric',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'label' => t('% Volumetric Soil Water Content'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_total_soil_porosity'] = array(
    'field_name' => 'field_dirt_total_soil_porosity',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'label' => t('% Total Soil Porosity'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );
  $weight_in_fc += 1;

  $instances['field_dirt_water_filled_pores'] = array(
    'field_name' => 'field_dirt_water_filled_pores',
    'type' => 'number_decimal',
    'entity_type' => 'field_collection_item',
    'bundle' => 'field_dirt_bulk_density_calcs',
    'label' => t('% Water-Filled Pores'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_fc,
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(
          'scale' => 2,
        ),
        'weight' => $weight_in_fc,
      ),
    ),
  );

  // End of fc, resume weights with $weight_in_group

  $instances['field_dirt_bulk_density_notes'] = array(
    'field_name' => 'field_dirt_bulk_density_notes',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'type'  => 'text_long',
    'label' => t('Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_group,
      'settings' => array(
        'rows' => 6,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_group,
        'type' => 'text_default',
      ),
    ),
  );
  

  /*---------------------------------*/
  /* Twice a Year Data Survey Fields */
  /*---------------------------------*/
  $bundle = 'dirt_twice_year_data_survey';

  // Form weight counter
  $weight_in_form = 0;

  // Chemical soil properties field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_chem_soil_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  // Composite sample field group goes here
  // Assign its weight according to $weight_in_group
  variable_set('dirt_group_dirt_soil_sample_weight', $weight_in_group);
  $weight_in_group += 1;

  // New weight counter for inside field group
  $weight_in_sample_group = 0;

  $instances['field_dirt_soil_n'] = array(
    'field_name' => 'field_dirt_soil_n',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('N - Value from Color Comparator, Equivalency in ppm, and Category'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_n_value'] = array(
    'field_name' => 'field_dirt_soil_n_value',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('N - Value from Color Comparator'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_n_ppm'] = array(
    'field_name' => 'field_dirt_soil_n_ppm',
    'type' => 'number_integer',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('N - Equivalency in ppm'),
     'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_n_category'] = array(
    'field_name' => 'field_dirt_soil_n_category',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('N - Category'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_p'] = array(
    'field_name' => 'field_dirt_soil_p',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('P - Value from Color Comparator, Equivalency in ppm, and Category'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_p_value'] = array(
    'field_name' => 'field_dirt_soil_p_value',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('P - Value from Color Comparator'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_p_ppm'] = array(
    'field_name' => 'field_dirt_soil_p_ppm',
    'type' => 'number_integer',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('P - Equivalency in ppm'),
     'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_p_category'] = array(
    'field_name' => 'field_dirt_soil_p_category',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('P - Category'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_k'] = array(
    'field_name' => 'field_dirt_soil_k',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('K - Value from Color Comparator, Equivalency in ppm, and Category'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_k_value'] = array(
    'field_name' => 'field_dirt_soil_k_value',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('K - Value from Color Comparator'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_k_ppm'] = array(
    'field_name' => 'field_dirt_soil_k_ppm',
    'type' => 'number_integer',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('K - Equivalency in ppm'),
     'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_k_category'] = array(
    'field_name' => 'field_dirt_soil_k_category',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('K - Category'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_ph'] = array(
    'field_name' => 'field_dirt_soil_ph',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('pH'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_ph_value'] = array(
    'field_name' => 'field_dirt_soil_ph_value',
    'type' => 'number_decimal',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('pH Number'),
    'widget' => array(
      'type'   => 'number',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'settings' => array(
          'scale' => 1,
        ),
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_ph_category'] = array(
    'field_name' => 'field_dirt_soil_ph_category',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('pH - Category'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_active_carbon'] = array(
    'field_name' => 'field_dirt_soil_active_carbon',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Active Carbon (lbs/A) and Soil Quality'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_ac_range'] = array(
    'field_name' => 'field_dirt_soil_ac_range',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Active Carbon (lbs/A)'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_quality'] = array(
    'field_name' => 'field_dirt_soil_quality',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Soil Quality'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );
  $weight_in_sample_group += 1;

  $instances['field_dirt_soil_organic_matter'] = array(
    'field_name' => 'field_dirt_soil_organic_matter',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Active Organic Matter (lbs/A)'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_select',
      'weight' => $weight_in_sample_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_sample_group,
      ),
    ),
  );

  $instances['field_dirt_chem_soil_notes'] = array(
    'field_name' => 'field_dirt_chem_soil_notes',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'type'  => 'text_long',
    'label' => t('Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_form,
      'settings' => array(
        'rows' => 6,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_form,
        'type' => 'text_default',
      ),
    ),
  );


  /*---------------------------------*/
  /* Group Information Survey Fields */
  /*---------------------------------*/
  $bundle = 'dirt_group_information_survey';

  // Keep weight counters so that new fields can be easily be added without
  // manually updating weight numbers
  $weight_in_form = 0;

  $instances['field_dirt_group_name'] = array(
    'field_name' => 'field_dirt_group_name',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Group Name'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
  );
  $weight_in_form += 1;

  $instances['field_dirt_group_participants'] = array(
    'field_name' => 'field_dirt_group_participants',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Participant Names'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'description' => t('List names of participants separated by commas.'),
  );
  $weight_in_form += 1;

  $instances['field_dirt_group_institution'] = array(
    'field_name' => 'field_dirt_group_institution',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Institution'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'description' => t('School, youth organization, etc.'),
  );
  $weight_in_form += 1;

  // Group leader contact info field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_group_leader_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_group_leader_name'] = array(
    'field_name' => 'field_dirt_group_leader_name',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Name'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_group_leader_email'] = array(
    'field_name' => 'field_dirt_group_leader_email',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('E-mail Address'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_group_leader_phone'] = array(
    'field_name' => 'field_dirt_group_leader_phone',
    'type' => 'text',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Phone Number'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('Enter the phone number with area code in the format ' .
                       'xxx-xxx-xxxx.'),
  );
  $weight_in_group += 1;


  /*---------------------*/
  /* User Profile Fields */
  /*---------------------*/

  // For these fields, we're adding extra array keys to denote
  // which fields are related to data entry and which are admin-only.
  // These keys will be ignored in field_create_instance() but
  // will be used to identify these fields as needed.

  // Keep weight counters so that new fields can be easily be added without
  // manually updating weight numbers
  // Want these fields to be above usual user profile fields so set 
  // initial weight lower:
  $weight_in_form = -50;

  $instances['field_dirt_user_first_name'] = array(
    'field_name' => 'field_dirt_user_first_name',
    'type' => 'text',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('First Name'),
    'required' => TRUE,
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
  );
  $weight_in_form += 1;

  $instances['field_dirt_user_last_name'] = array(
    'field_name' => 'field_dirt_user_last_name',
    'type' => 'text',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('Last Name'),
    'required' => TRUE,
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
  );
  $weight_in_form += 1;

  $instances['field_dirt_user_organization'] = array(
    'field_name' => 'field_dirt_user_organization',
    'type' => 'text',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('Institution/Organization'),
    'required' => TRUE,
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
    'required' => TRUE,
  );
  $weight_in_form += 1;

  $instances['field_dirt_user_county'] = array(
    'field_name' => 'field_dirt_user_county',
    'type' => 'text',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('County'),
    'widget' => array(
      'type'   => 'text_textfield',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
    'description' => t('Enter the county name of your current location.'),
  );
  $weight_in_form += 1;

  $instances['field_dirt_user_account_reason'] = array(
    'field_name' => 'field_dirt_user_account_reason',
    'entity_type' => 'user',
    'bundle' => 'user',
    'type'  => 'text_long',
    'required' => TRUE,
    'label' => t('Reason for account'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_form,
      'settings' => array(
        'rows' => 5,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_form,
        'type' => 'text_default',
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
    'description' => t('Briefly explain why you are requesting a data entry ' .
                       'account.<br><em>Your answer will not be visible on ' .
                       'your profile. Only site administrators will be able ' .
                       'to view this field.</em>'),
  );
  $weight_in_form += 1;

  $instances['field_dirt_user_sharing_perms'] = array(
    'field_name' => 'field_dirt_user_sharing_perms',
    'type' => 'list_text',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('Data Sharing Permissions'),
    'required' => TRUE,
    'default_value' => array(
      0 => array(
        'value' => 'public',
      ) 
    ),
    'widget' => array(
      'type'   => 'options_buttons',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_form,
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
    'description' => t('Select your survey data sharing preferences below ' .
                       'Regardless of your sharing preference, all soil ' .
                       'survey data collected will be accessible to site ' .
                       'administrators and scientists upon request.'),
  );
  $weight_in_form += 1;

  // Admin only field group goes here
  // Assign its weight according to $weight_in_form
  variable_set('dirt_group_dirt_user_admin_weight', $weight_in_form);
  $weight_in_form += 1;

  // New weight counter for inside field group
  $weight_in_group = 0;

  $instances['field_dirt_user_collection_state'] = array(
    'field_name' => 'field_dirt_user_collection_state',
    'type' => 'list_text',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('Collection Status'),
    'required' => TRUE,
    'default_value' => array(
      0 => array(
        'value' => 'active',
      )
    ),
    'widget' => array(
      'type'   => 'options_buttons',
      'weight' => $weight_in_group,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
      ),
    ),
    'description' => t('Indicate whether this site is Active (actively ' .
                       'collecting data) or Closed (no longer collecting ' .
                       'data).'),
  );
  $weight_in_group += 1;

  $instances['field_user_collection_start'] = array(
    'field_name' => 'field_dirt_user_collection_start',
    'type'  => 'datetime',
    'entity_type' => 'user',
    'bundle' => 'user',
    'label' => t('Collection Start'),
    'description' => t('Enter collection start date. (Optional, defaults to ' .
                       'date of first survey.)'),
    'widget' => array(
      'type' => 'date_popup',
      'weight' => $weight_in_group,
      'module' => 'date',
      'settings' => array(
        'input_format' => 'custom',
        'input_format_custom' => DIRT_SURVEY_DATE_FORMAT,
        'no_fieldset' => 1,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => $weight_in_group,
        'type' => 'date_default',
        'format_type' => DIRT_SURVEY_DATE_TYPE,
        'settings' => array(
           'format_type' => DIRT_SURVEY_DATE_TYPE,
        ),
      ),
    ),
    'settings' => array(
      'default_value' => 'blank',
      'default_format' => DIRT_SURVEY_DATE_TYPE,
    ),
  );
  $weight_in_group += 1;

  $instances['field_dirt_user_admin_notes'] = array(
    'field_name' => 'field_dirt_user_admin_notes',
    'entity_type' => 'user',
    'bundle' => 'user',
    'type'  => 'text_long',
    'label' => t('Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => $weight_in_group,
      'settings' => array(
        'rows' => 5,
      ),
    ),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => $weight_in_group,
        'type' => 'text_default',
      ),
    ),
    'settings' => array(
      'user_register_form' => TRUE,
    ),
    'description' => t('Optionally add notes for this account (e.g., why ' .
                       'account was approved/denied).'),
  );
  $weight_in_group += 1;


  /*--------------------------------*/
  /* Data Entry Portal Pages Fields */
  /*--------------------------------*/
  $bundle = 'dirt_data_entry_portal_page';

  // Keep weight counters so that new fields can be easily be added without
  // manually updating weight numbers
  $weight_in_form = 0;

  // Multiple instances of this field; append bundle name to instance key
  $instances['body' . $bundle] = array(
    'field_name' => 'body',
    'type' => 'text_with_summary',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Body'),
    'widget' => array(
      'type'   => 'text_textarea_with_summary',
      'weight' => $weight_in_form,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'weight' => $weight_in_form,
      ),
    ),
  );

  return $instances;
}


/**
 * Returns survey field instances added to all survey types.
 *
 * These instances will be created whenever a survey type is added, and deleted
 * whenever a survey type is deleted.
 *
 * @param[in] $type Machine name of survey type to attach field instance to.
 *
 * @retval Array of field instances common to all survey types.
 */
function dirt_get_survey_field_instances($type) {
  $instances = array();

  // Place these fields at the top of all surveys (use weights below 0).
  // Save their weights for later lookup
  variable_set('dirt_field_dirt_form_header_weight', -3);

  $instances['field_dirt_form_header'  . $type] = array(
    'field_name' => 'field_dirt_form_header',
    'entity_type' => 'node',
    'bundle' => $type,
    'label' => t('Form Header'),
    'widget' => array(
      'type'   => 'markup',
      'weight' => -3,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'markup_view',
        'module' => 'markup',
        'weight' => -3,
      ),
    ),
  );

  variable_set('dirt_field_dirt_form_submission_note_weight', -2);

  $instances['field_dirt_form_submission_note' . $type] = array(
    'field_name' => 'field_dirt_form_submission_note',
    'entity_type' => 'node',
    'bundle' => $type,
    'label' => t('Form Submission Note'),
    'widget' => array(
      'type'   => 'markup',
      'weight' => -2,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'weight' => -2,
      ),
    ),
  );

  variable_set('dirt_field_dirt_date_of_survey_weight', -1);

  $instances['field_dirt_date_of_survey' . $type] = array(
    'field_name' => 'field_dirt_date_of_survey',
    'type'  => 'datetime',
    'entity_type' => 'node',
    'bundle' => $type,
    'label' => t('Date of Survey'),
    'widget' => array(
      'type' => 'date_popup',
      'weight' => -1,
      'module' => 'date',
      'settings' => array(
        'input_format' => 'custom',
        'input_format_custom' => DIRT_SURVEY_DATE_FORMAT,
        'no_fieldset' => 0,
        'label_position' => 'none',
      ),
    ),
    'required' => TRUE,
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => -1,
        'type' => 'date_default',
        'format_type' => DIRT_SURVEY_DATE_TYPE,
        'settings' => array(
           'format_type' => DIRT_SURVEY_DATE_TYPE,
        ),
      ),
    ),
    'settings' => array(
      'default_value' => 'blank',
      'default_format' => DIRT_SURVEY_DATE_TYPE,
    ),
    'description' => t('Enter the survey date in the form @format or ' .
      'select the survey date from the pop-up calendar.', array(
      '@format' => DIRT_SURVEY_DATE_HINT)),
  );

  return $instances;
}


/**
 * Displays data entry fields for data entry accounts and hides for all other
 * account types.
 *
 * @param[in,out] $account User object on which the operation is being performed.
 */
function dirt_toggle_data_entry_fields_display(&$account) {
  $data_entry_fields = dirt_get_data_entry_user_field_names();
  $data_entry_user_rid = variable_get('dirt_data_entry_user_role');

  if (!(user_has_role($data_entry_user_rid, $account))) {
    foreach ($data_entry_fields as $field) {
      if (array_key_exists($field, $account->content))
        $account->content[$field]['#access'] = FALSE;
    }
  }  
}


/**
 * Returns list of field (machine) names of user profile fields which are
 * related to data entry.
 *
 * @retval Array of field names.
 */
function dirt_get_data_entry_user_field_names() {
  return array(
    'field_dirt_user_organization',
    'field_dirt_user_county',
    'field_dirt_user_account_reason',
    'field_dirt_user_sharing_perms',
    'field_dirt_user_collection_state',
    'field_dirt_user_collection_start',
    'field_dirt_user_admin_notes',
  );
}


/**
 * Returns bulk density column title information (corresponding letters and
 * formulas).
 *
 * @retval Array of arrays indexed by field name, with keys 'bundle' and
 *   either one or both of 'title_prefix' and 'title_suffix'.
 */
function dirt_get_bulk_density_column_title_info() {
  // Return arrays indexed by field name have these keys:
  // 'bundle': field collection field name
  // 'title_prefix': text that goes above existing field label
  // 'title_suffix': text that goes below existing field label
  // Each will have one or both of 'title_prefix' & 'title_suffix'.

  return array(
    'field_dirt_weight_wet_soil_bag' => array(
      'bundle' => 'field_dirt_sample_before_drying',
      'title_suffix' => '<br><div class="letter">' . t('A') . '</div>',
    ),
    'field_dirt_weight_bag' => array(
      'bundle' => 'field_dirt_sample_before_drying',
      'title_suffix' => '<br><div class="letter">' . t('B') . '</div>',
    ),
    'field_dirt_weight_wet_soil' => array(
      'bundle' => 'field_dirt_sample_before_drying',
      'title_suffix' => '<br><div class="letter">' . t('C = A - B') . '</div>',
    ),
    'field_dirt_weight_dry_soil_bowl' => array(
      'bundle' => 'field_dirt_sample_after_drying',
      'title_suffix' => '<br><div class="letter">' . t('D') . '</div>',
    ),
    'field_dirt_weight_bowl' => array(
      'bundle' => 'field_dirt_sample_after_drying',
      'title_suffix' => '<br><div class="letter">' . t('E') . '</div>',
    ),
    'field_dirt_weight_dry_soil' => array(
      'bundle' => 'field_dirt_sample_after_drying',
      'title_suffix' => '<br><div class="letter">' . t('F = D - E') . '</div>',
    ),
    'field_dirt_volume_soil_sample' => array(
      'bundle' => 'field_dirt_bulk_density_calcs',
      'title_prefix' => '<div class="top-half">',
      'title_suffix' => '<br>' . t('G') . '</div><div class="bottom-half">' .
        t('Equals the interior volume of the metal ring (π x r<sup>2</sup> x ' .
        'L) (cm<sup>3</sup>)') . '</div>',
    ),
    'field_dirt_soil_bulk_density' => array(
      'bundle' => 'field_dirt_bulk_density_calcs',
      'title_prefix' => '<div class="top-half">',
      'title_suffix' => '<br>' . t('H = F / G') . '</div>' .
        '<div class="bottom-half">' . t('H = Weight of dry soil / Volume of ' .
        'soil sample') . '</div>',
    ),
    'field_dirt_water_gravimetric' => array(
      'bundle' => 'field_dirt_bulk_density_calcs',
      'title_prefix' => '<div class="top-half">',
      'title_suffix' => '<br>' . t('I = [(C – F) / F] x 100') . '</div>' .
        '<div class="bottom-half">' . t('I = [(Weight of wet soil – Weight ' .
        'of dry soil) / Weight of dry soil] x 100') . '</div>',
    ),
    'field_dirt_water_volumetric' => array(
      'bundle' => 'field_dirt_bulk_density_calcs',
      'title_prefix' => '<div class="top-half">',
      'title_suffix' => '<br>' . t('J = I x H') . '</div>' .
        '<div class="bottom-half">' . t('J = % Gravimetric soil water ' .
        'content x Soil bulk density') . '</div>',
    ),
    'field_dirt_total_soil_porosity' => array(
      'bundle' => 'field_dirt_bulk_density_calcs',
      'title_prefix' => '<div class="top-half">',
      'title_suffix' => '<br>' . t('K = [1 – (H/2.65 g/cm<sup>3</sup>)] x ' .
        '100') . '</div><div class="bottom-half">' . t('[1 – (Soil bulk ' .
        'density / Soil particle density)] x 100') . '</div>',
    ),
    'field_dirt_water_filled_pores' => array(
      'bundle' => 'field_dirt_bulk_density_calcs',
      'title_prefix' => '<div class="top-half">',
      'title_suffix' => '<br>' . t('L = J/K x 100') . '</div>' .
        '<div class="bottom-half">' . t('(% Volumetric water content / % ' .
        'Total porosity) x 100') . '</div>',
    ),
  );
}

