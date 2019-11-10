<?php

/**
 * @file
 * This file specifies the definitions for custom field groups created by this
 * module.
 */

/**
 * Returns an associative array of field group basenames => info including
 * bundle name and mode(s).
 * Bundle name "ALL_SURVEYS" indicates that this group should be
 * attached to all survey content types.
 *
 * @param[in] $modes *Optional* View modes (defaults to form and default).
 *
 * @retval Associative array of field group basenames and info.
 */
function dirt_get_field_group_basenames($modes = array('form', 'default')) {
  // Note: group basenames need to stay <= 24 characters so that
  // group name = basename + mode <= 32 characters.

  return array(
    // Group for site address fields
    'group_dirt_site_address' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for site geo coords (lat/lon fields)
    'group_dirt_site_coords' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for site description fields part 1
    'group_dirt_site_desc' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for site description fields part 2
    'group_dirt_site_desc_con' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for habitat type fields
    'group_dirt_site_habitat' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => array('form'),
    ),
    // Group for land management updates
    'group_dirt_site_updates' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for soil core variables fields
    'group_dirt_site_soilcore' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for site drawing fields
    'group_dirt_site_drawing' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for site photo fields
    'group_dirt_site_photos' => array(
      'bundles' => array('dirt_site_description_survey'),
      'modes' => $modes,
    ),
    // Group for temp & cloud cover fields
    'group_dirt_temp_cloud' => array(
      'bundles' => array('dirt_monthly_data_survey'),
      'modes' => $modes,
    ),
    // Group for soil respiration fields
    'group_dirt_soil_resp' => array(
      'bundles' => array('dirt_monthly_data_survey'),
      'modes' => $modes,
    ),
    // Group for CO2-C calculations fields
    'group_dirt_co2c' => array(
      'bundles' => array('dirt_monthly_data_survey'),
      'modes' => $modes,
    ),
    // Group for soil water content fields (calculations)
    'group_dirt_bulk_density' => array(
      'bundles' => array('dirt_monthly_data_survey'),
      'modes' => $modes,
    ),
    // Group for soil water content fields (sample before drying)
    'group_dirt_before_drying' => array(
      'bundles' => array('dirt_monthly_data_survey'),
      'modes' => $modes,
    ),
    // Group for soil water content fields (sample after drying)
    'group_dirt_after_drying' => array(
      'bundles' => array('dirt_monthly_data_survey'),
      'modes' => $modes,
    ),
    // Group for chemical soil properties fields
    'group_dirt_chem_soil' => array(
      'bundles' => array('dirt_twice_year_data_survey'),
      'modes' => $modes,
    ),
    // Group for composite sample
    'group_dirt_soil_sample' => array(
      'bundles' => array('dirt_twice_year_data_survey'),
      'modes' => $modes,
    ),
    // Group for group leader information
    'group_dirt_group_leader' => array(
      'bundles' => array('dirt_group_information_survey'),
      'modes' => $modes,
    ),
    // Group for admin-only user profile fields
    'group_dirt_user_admin' => array(
      'bundles' => array('user'),
      'modes' => $modes,
    ),
    // Group for admin-only survey fields (if any)
    'group_dirt_admin_only' => array(
      'bundles' => array('ALL_SURVEYS'),
      'modes' => $modes,
    ),
  );
}


/**
 * Returns array of field groups where each array item is an array with keys:
 *
 * 'basename' : group basename (without mode)
 * 'bundle' : bundle group is attached to
 * 'mode' : mode (form or node view),
 *
 * using definitions from dirt_get_field_group_basenames().
 *
 * This function effectively enumerates all field groups installed in this
 * module.
 *
 * @retval Array of arrays with keys 'basename', 'bundle', and 'mode'.
 *
 * @sa dirt_get_field_group_basenames().
 */
function dirt_get_field_group_info() {
  $field_groups = dirt_get_field_group_basenames();

  return dirt_get_field_group_info_from_group_basenames($field_groups);
}


/**
 * Returns array of field groups where each array item is an array with keys:
 *
 * 'basename' : group basename (without mode)
 * 'bundle' : bundle group is attached to
 * 'mode' : mode (form or node view),
 *
 * using definitions from group basenames array input.
 *
 * @param[in] $field_groups Array of field group definitions keyed by group
 *   basename.
 *
 * @retval Array of arrays with keys 'basename', 'bundle', and 'mode'.
 *
 * @sa dirt_get_field_group_basenames().
 * @sa dirt_get_field_group_info().
 */
function dirt_get_field_group_info_from_group_basenames($field_groups) {
  // Initialize return
  $group_info = array();

  // Unpack groups from basenames => info
  foreach ($field_groups as $group_basename => $info) {
    foreach ($info['modes'] as $mode) {
      $bundles = $info['bundles'];
      // If attach to all surveys, get all survey content type names
      // as available bundles to attach to
      if (in_array('ALL_SURVEYS', $bundles))
        $bundles = dirt_get_survey_types();

      foreach ($bundles as $bundle) {
        $group_info[] = array(
          'basename' => $group_basename,
          'bundle' => $bundle,
          'mode' => $mode,
        );
      }
    }
  }

  return $group_info;
}


/**
 * Returns group object for $group_basename and $mode.
 *
 * Input:
 * $group_basename String: field group machine name minus '_$mode' suffix.
 * $bundle String: bundle (e.g., node type) field group attached to.
 * $mode String view mode.
 *
 * @retval Object representing field group.
 */
function dirt_get_field_group_object($group_basename, $bundle, $mode) {
  $field_group = new stdClass();

  // Set group name according to basename
  $group_name = $group_basename . '_' . $mode;

  // Initialize required variables
  // The following MUST be set in switch() below for each group
  $label = '';
  $children = array();

  // Default options for all field groups
  // These may be overwritten for some groups in switch() below
  $entity_type = 'node';
  $parent_name = '';
  $format_type = 'fieldset';
  $formatter = 'collapsible';
  $description = '';
  $classes = '';

  // Attempt to get field group weight from stored variable; 
  // default to 0 if not set
  $weight = variable_get('dirt_' . $group_basename . '_weight', 0);

  switch($group_basename) {
    case 'group_dirt_site_address':
      $label = t('Site Address');
      $children = array(
        0 => 'field_dirt_site_address',
        1 => 'field_dirt_site_city',
        2 => 'field_dirt_site_county',
        3 => 'field_dirt_site_state',
        4 => 'field_dirt_site_zip_code',
      );
      break;

    case 'group_dirt_site_desc':
      $label = t('Site Description');
      $children = array(
        0 => 'field_dirt_site_mean_annual_temp',
        1 => 'field_dirt_site_annual_precip',
        2 => 'field_dirt_site_elevation',
        3 => 'field_dirt_site_slope',
        4 => 'field_dirt_site_aspect',
        5 => 'field_dirt_site_map_unit_note',
        6 => 'group_dirt_site_coords_' . $mode,
        7 => 'field_dirt_site_map_unit_name',
        8 => 'field_dirt_site_map_unit_symbol',
      );
      break;

    case 'group_dirt_site_coords':
      $label = t('Geographical Coordinates in Decimal Degrees');
      $children = array(
        0 => 'field_dirt_site_geo_coords',
        3 => 'field_dirt_site_geo_coords_note',
      );
      break;

    case 'group_dirt_site_desc_con':
      $label = t('Site Description Continuation');
      // Set children according to mode:
      $children = array();
      $children[] = 'field_dirt_site_landscape_posit';
      if ($mode == 'form') {
        $children[] = 'group_dirt_site_habitat_form';
      }
      else {
        $children[] = 'field_dirt_site_habitat_type';
        $children[] = 'field_dirt_site_type_of_crop';
        $children[] = 'field_dirt_site_cropping_system';
      }
      $children[] = 'field_dirt_site_topography';
      $children[] = 'field_dirt_site_present_manage';
      $children[] = 'field_dirt_site_past_manage';
      break;

    case 'group_dirt_site_habitat':
      $label = t('Habitat Type');
      $children = array(
        0 => 'field_dirt_site_habitat_type',
        1 => 'field_dirt_site_type_of_crop',
        2 => 'field_dirt_site_cropping_system',
      );
      break;

    case 'group_dirt_site_updates':
      $label = t('Land Management Updates');
      $children = array(
        0 => 'field_dirt_site_manage_updates',
      );
      // Set description according to mode
      $desc_txt = t('Enter any changes in the land management of your site.');
      $description = ($mode == 'form') ? $desc_txt : '';
      break;

    case 'group_dirt_site_soilcore':
      $label = t('Soil Core Variables Data');
      $children = array(
        0 => 'field_dirt_site_layer_data',
      );
      // Set description according to mode
      $desc_txt = t('Up to 25 cm (10") depth');
      $description = ($mode == 'form') ? $desc_txt : '';
      break;

    case 'group_dirt_site_drawing':
      $label = t('Drawing of Site');
      $children = array(
        0 => 'field_dirt_site_drawing_guide',
        1 => 'field_dirt_site_drawing',
        2 => 'field_dirt_site_drawing_scale',
      );
      break;

    case 'group_dirt_site_photos':
      $label = t('Survey Site Habitat Photo(s)');
      $children = array(
        0 => 'field_dirt_site_habitat_photos',
      );
      break;

    case 'group_dirt_temp_cloud':
      $label = t('Temperatures and Cloud Cover');
      $children = array(
        0 => 'field_dirt_temp_cloud_cover',
        1 => 'field_dirt_avg_soil_temp_5cm',
        2 => 'field_dirt_cloud_cover',
        3 => 'field_dirt_temp_cloud_notes',
      );
      // Set description according to mode
      $desc_txt = t('Enter the measured temperatures. As you type, the ' .
                    'average soil temperature at 5 cm depth will be ' .
                    'automatically calculated.');
      $description = ($mode == 'form') ? $desc_txt : '';
      break;

    case 'group_dirt_soil_resp':
      $label = t('Soil Respiration');
      $children = array(
        0 => 'field_dirt_color_number_by_hour',
        1 => 'group_dirt_co2c_' . $mode,
        2 => 'field_dirt_soil_resp_notes',
      );
      // Set description according to mode
      $desc_txt = t('Time of data collection as hh:mm where hh range is ' . 
                    '00-23 and mm range is 00-59. Color number by hour (h).');
      $description = ($mode == 'form') ? $desc_txt : '';
      break;

    case 'group_dirt_co2c':
      $label = t('Interpretation of Colors for CO₂-C Emissions ' .
        '(lbs/acre/day) after 24 Hours');
      $children = array(
        0 => 'field_dirt_co2c_room_temp_range',
        1 => 'field_dirt_co2c_room_temp_lower',
        2 => 'field_dirt_co2c_room_temp_upper',
        3 => 'field_dirt_co2c_room_temp_avg',
        4 => 'field_dirt_co2c_field_temp_range',
        5 => 'field_dirt_co2c_field_temp_lower',
        6 => 'field_dirt_co2c_field_temp_upper',
        7 => 'field_dirt_co2c_field_temp_avg',
      );
      // Set description according to mode
      $desc_txt  = t('The values for CO₂-C emissions after 24h at room ' .
                     'temperature and at field temperature are automatically ' .
                     'calculated based on the color numbers entered in the ' .
                     'above table and the average soil temperature at 5 cm ' .
                     'depth that was previously calculated.');
      $description = ($mode == 'form') ? $desc_txt : '';
      $format_type = 'div';
      $formatter = 'open';
      break;

    case 'group_dirt_bulk_density':
      $label = t('Calculations for Bulk Density, Water Content, and Soil ' .
        'Water-Filled Pore Space (%WFP)');
      $children = array(
        0 => 'group_dirt_before_drying_' . $mode,
        1 => 'group_dirt_after_drying_' . $mode,
        2 => 'field_dirt_bulk_density_calcs',
        3 => 'field_dirt_bulk_density_notes',
      );
      // Set description according to mode
      $desc_txt  = t('Enter the amounts for columns <b>A</b>, <b>B</b>, ' .
                     '<b>D</b>, and <b>E</b>. As you type, the values for ' .
                     '<b>C</b> and <b>F</b> will be automatically calculated.' .
                     '<br><br>Also enter the value for column <b>G</b> ' .
                     '(volume of soil sample), and the rest of the ' .
                     'values will be automatically calculated as well.');
      $description = ($mode == 'form') ? $desc_txt : '';
      break;

    case 'group_dirt_before_drying':
      $label = t('Sample Before Drying');
      $children = array(
        0 => 'field_dirt_sample_before_drying',
      );
      $format_type = 'div';
      break;

    case 'group_dirt_after_drying':
      $label = t('Sample After Drying');
      $children = array(
        0 => 'field_dirt_sample_after_drying',
        1 => 'field_dirt_container_weight_note',
      );
      $format_type = 'div';
      break;

    case 'group_dirt_chem_soil':
      $label = t('Chemical Soil Properties');
      $children = array(
        0 => 'group_dirt_soil_sample_' . $mode,
      );
      break;

    case 'group_dirt_soil_sample':
      $label = t('Composite Sample');
      $children = array(
        0 => 'field_dirt_soil_n',
        1 => 'field_dirt_soil_n_value',
        2 => 'field_dirt_soil_n_ppm',
        3 => 'field_dirt_soil_n_category',
        4 => 'field_dirt_soil_p',
        5 => 'field_dirt_soil_p_value',
        6 => 'field_dirt_soil_p_ppm',
        7 => 'field_dirt_soil_p_category',
        8 => 'field_dirt_soil_k',
        9 => 'field_dirt_soil_k_value',
        10 => 'field_dirt_soil_k_ppm',
        11 => 'field_dirt_soil_k_category',
        12 => 'field_dirt_soil_ph',
        13 => 'field_dirt_soil_ph_value',
        14 => 'field_dirt_soil_ph_category',
        15 => 'field_dirt_soil_active_carbon',
        16 => 'field_dirt_soil_ac_range',
        17 => 'field_dirt_soil_quality',
        18 => 'field_dirt_soil_organic_matter',
      );
      $format_type = 'div';
      $formatter = 'open';
      break;

    case 'group_dirt_group_leader':
      $label = t('Contact Information of Survey Leader');
      $children = array(
        0 => 'field_dirt_group_leader_name',
        1 => 'field_dirt_group_leader_email',
        2 => 'field_dirt_group_leader_phone',
      );
      break;

    case 'group_dirt_user_admin':
      // Admin-only fields for user profiles
      $label = t('Admin-only Fields');
      $children = array(
        0 => 'field_dirt_user_account_reason',
        1 => 'field_dirt_user_collection_state',
        2 => 'field_dirt_user_collection_start',
        3 => 'field_dirt_user_admin_notes',
      );
      $entity_type = 'user';
      // Set description for all modes
      $description = t('These fields are visible to site admins only.');
      break;

    case 'group_dirt_admin_only':
      // Admin-only fields for survey content types
      $label = t('Admin-only Fields');
      // Set description for all modes
      $description = t('These fields are visible to site admins only.');
      break;
  }

  // Set identifier based on group name and entity_type
  // group_name|entity_type|bundle|mode
  $identifier = $group_name . '|' . $entity_type . '|' . $bundle . '|' . $mode;

  // Now create the field group:

  $field_group->disabled = FALSE;
  $field_group->api_version = 1;
  $field_group->identifier = $identifier;
  $field_group->group_name = $group_name;
  $field_group->entity_type = $entity_type;
  $field_group->bundle = $bundle;
  $field_group->mode = $mode;
  $field_group->parent_name = $parent_name;
  $field_group->data = array(
    'label' => $label,
    'weight' => $weight,
    'children' => $children,
    'format_type' => $format_type,
    'format_settings' => array(
      'formatter' => $formatter,
      'instance_settings' => array(
        'description' => $description,
        'classes' => $classes,
        'required_fields' => 1,
      ),
    ),
  );

  return $field_group;
}

