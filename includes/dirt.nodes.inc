<?php

/**
 * @file
 * This file specifies the definitions for custom nodes created by this module
 * and contains functions related to handling nodes (e.g., altering node view).
 */

/**
 * Returns info on nodes created by this module.
 *
 * @retval Array of node information.
 */
function dirt_get_installed_nodes() {
  $demographics_body  = t('Enter info on participant demographics here, ' .
                          'if applicable, e.g., number of active soil sites, ' .
                          'number of participants and/or institutions, etc.'); 

  $type = 'dirt_data_entry_portal_page';

  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);

  return array(
    0 => array(
      'type' => $type,
      'title' => t('Citizen Science Soil Health Surveys'),
      'alias' => $citizen_sci_path,
      'body' => t('Enter a description of the Soil Health Citizen Science project here.'),
    ),
    1 => array(
      'type' => $type,
      'title' => t('Resources'),
      'alias' => $citizen_sci_path . '/resources',
      'body' => t('Enter info on resources here, e.g., manuals, guides, etc.'),
    ),
    2 => array(
      'type' => $type,
      'title' => t('Soil Health Survey Training Sessions'),
      'alias' => $citizen_sci_path . '/trainingsessions',
      'body' => t('Enter info on training sessions here, if applicable.'),
    ),
    3 => array(
      'type' => $type,
      'title' => t('Soil Health Surveys Demographics'),
      'alias' => $citizen_sci_path . '/demographics',
      'body' => $demographics_body,
    ),
  );
}


/**
 * Adds "submitted by [username]" tag to survey nodes above survey content.
 *
 * @param[in,out] $build: A renderable array representing the node content.
 */
function dirt_add_submitted_by_tag_to_survey_node_view(&$build) {
  // Get survey author username
  $node = $build['#node'];
  $author_username = $node->name;

  $markup  = '<div id="dirt-submitted-by-container">';
  $markup .= '<span class="dirt-submitted-by">' . t('Submitted by: ') . '</span>';

  // For site admins, we'll also link to the user profile
  if (user_access('administer site configuration'))
    $markup .= l(t('@name', array('@name' => $author_username)), 'user/' . $node->uid);
  else
    $markup .= t('@name', array('@name' => $author_username));

  $markup .= '</div>'; // Close div

  // Create markup field
  // Assign it weight of "submission note" field which is below form header
  // and above date of survey, as submission note is not displayed on node
  // view.
  $build['dirt_submitted_by'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#weight' => variable_get('dirt_field_dirt_form_submission_note_weight', 0),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
}


/**
 * Hides all of the calculated N, P, K, pH, and Active Carbon field
 * components on the survey node view so that only the summary fields
 * are displayed.
 *
 * @param[in,out] $build: A renderable array representing the node content.
 */
function dirt_hide_twice_year_component_fields_in_survey_node_view(&$build) {
  // These component fields are calculated via JavaScript from the summary
  // fields and stored in the database separately in case they need to be
  // referred to later (e.g., in the data search portal, if installed).
  $component_fields = array(
    'field_dirt_soil_n_value',
    'field_dirt_soil_n_ppm',
    'field_dirt_soil_n_category',
    'field_dirt_soil_p_value',
    'field_dirt_soil_p_ppm',
    'field_dirt_soil_p_category',
    'field_dirt_soil_k_value',
    'field_dirt_soil_k_ppm',
    'field_dirt_soil_k_category',
    'field_dirt_soil_ph_value',
    'field_dirt_soil_ph_category',
    'field_dirt_soil_ac_range',
    'field_dirt_soil_quality',
  );

  foreach($component_fields as $field) {
    $build[$field]['#access'] = FALSE;
  }
}


/**
 * Adds a "Back to the data portal" link at the bottom of each survey node.
 *
 * @param[in,out] $build: A renderable array representing the node content.
 */
function dirt_add_back_to_data_portal_link_to_survey_node_view(&$build) {
  // Limit to the full view
  if ($build['#view_mode'] == 'full') {
    $build['dirt_back_to_data_portal_link'] = array(
      '#type' => 'markup',
      '#markup' => l('« ' . t('Back to Data Portal'), variable_get('dirt_data_entry_portal_path', DIRT_DATA_ENTRY_PATH_DEFAULT)),
      '#weight' => 999,
      '#prefix' => '<h3 class="dirt-back-to-data-portal-link">',
      '#suffix' => '</h3>',
    );
  }
}


/**
 * Sets title of survey node according to survey type and date of survey.
 *
 * @param[in,out] $node Node object (assumed to be of survey type).
 */
function dirt_set_survey_title(&$node) {
  // Set according to type
  $type = $node->type;

  // Get fields using Entity Metadata Wrapper
  try {
    $node_wrapper = entity_metadata_wrapper('node', $node);
    $survey_date = $node_wrapper->field_dirt_date_of_survey->value();
    $survey_date_formatted = format_date($survey_date, DIRT_SURVEY_DATE_TYPE);
    $name = dirt_get_name_of_survey_type($node->type);

    // Set title
    $title = '';

    // Special cases (makes titles less clunky):
    switch ($type) {
      case 'dirt_group_information_survey':
        // Group info: "Group Information for <group_name> as of <survey_date>"
        $title = t('Group Information');
        $group_name = $node_wrapper->field_dirt_group_name->value();
        if ($group_name) $title .= ' ' . t('for !group', array('!group' => $group_name));
        $title .= ' ' . t('as of') . ' ' . $survey_date_formatted;
        break;

      case 'dirt_site_description_survey':
        // Site desc: "Site Description Collected <survey_date>"
        $title = t('Site Description Collected') . ' ' . $survey_date_formatted;
        break;

      case 'dirt_monthly_data_survey':
        // Monthly: "Monthly Data Collected <survey_date>"
        $title = t('Monthly Data Collected') . ' ' . $survey_date_formatted;
        break;

      case 'dirt_twice_year_data_survey':
        // Twice a Year: "Twice a Year Data Collected <survey_date>"
        $title = t('Twice a Year Data Collected') . ' ' . $survey_date_formatted;
        break;

      default:
        // Default option: <survey_type> Collected <survey_date>
        $title = t('@name Collected', array('@name' => $name)) . ' ' . $survey_date_formatted;
    }

    $node->title=$title;
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt', $e);
  }
}


/**
 * Sets breadcrumbs for survey content types.
 *
 * Adds links back to:
 * - Site homepage
 * - Citizen Science data entry homepage
 * - Data Entry Portal
 *
 * @param[in,out] $node Node object.
 */
function dirt_set_survey_node_view_breadcrumbs(&$node) {
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>'); // begin each breadcrumb with Home

  $breadcrumb_args = array();

  // Get title for Citizen Science data entry homepage
  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);
  $title = _dirt_get_page_title($citizen_sci_path);
  if ($title) $breadcrumb_args[$title] = $citizen_sci_path;

  $breadcrumb_args[t('Data Entry Portal')] = variable_get('dirt_data_entry_portal_path', DIRT_DATA_ENTRY_PATH_DEFAULT);

  foreach($breadcrumb_args as $text => $url) {
    $breadcrumb[] = l($text, $url);
  }
  // Note: some themes may make changes to breadcrumbs (e.g., in templates.php)
  // that makes this last line not necessary - either change here or in theme.
  // Default theme Bartik does not add current page so it's added below:
  $breadcrumb[] = $node->title;

  drupal_set_breadcrumb($breadcrumb);
}


/**
 * Sets breadcrumbs for data entry portal page content types.
 *
 * @param[in,out] $node Node object.
 */
function dirt_set_data_entry_portal_page_breadcrumbs(&$node) {
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');

  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);
  if (drupal_get_path_alias() != $citizen_sci_path) {
    $title = _dirt_get_page_title($citizen_sci_path);
    if ($title) $breadcrumb[] = l($title, $citizen_sci_path);
  }

  // Note: some themes may make changes to breadcrumbs (e.g., in templates.php)
  // that makes this last line not necessary - either change here or in theme.
  // Default theme Bartik does not add current page so it's added below:
  $breadcrumb[] = $node->title;

  drupal_set_breadcrumb($breadcrumb);
}


/**
 * Sets the bulk density column titles in the monthly survey node view.
 *
 * @param[in,out] $build: Renderable array representing the node content.
 */
function dirt_set_bulk_density_node_view_column_titles(&$build) {
  dpm($build);

  module_load_include('inc', 'dirt', 'includes/dirt.fields');

  // Get array of bundle name, field (column) name, and titles to set:
  $column_info = dirt_get_bulk_density_column_title_info();
 
  $bundles = array(
    'field_dirt_sample_before_drying',
    'field_dirt_sample_after_drying',
    'field_dirt_bulk_density_calcs',
  );

  foreach($bundles as $bundle) {
    if (array_key_exists($bundle, $build)) {
      $header = $build[$bundle][0]['#header'];

      foreach($header as $idx => $data_arr) {
        $title = $data_arr['data'];
        $field_name = $data_arr['class'];
        if (array_key_exists($field_name, $column_info)) {
          $title_info = $column_info[$field_name];

          if (array_key_exists('title_prefix', $title_info)) {
            // Set title prefix
            $build[$bundle][0]['#header'][$idx]['data'] = $title_info['title_prefix'] . $title;
          }
        
          if (array_key_exists('title_suffix', $title_info)) {
            // Set title suffix
            $build[$bundle][0]['#header'][$idx]['data'] .= $title_info['title_suffix'];
          }

          unset($title);
          unset($field_name);
          unset($title_info);
        }
      }
    }
  }
}


/**
 * Helper function to get title of a node given its path alias.
 *
 * @param[in] $path Path to node (alias).
 * 
 * @retval String title of node (empty string if none found).
 */
function _dirt_get_page_title($path) {
  // Initialize return value
  $title = '';

  // Get nid from node/nid:
  $path_args = explode('/', drupal_get_normal_path($path));
  $nid = $path_args[1];

  try {
    $node_wrapper = entity_metadata_wrapper('node', $nid);
    $title = $node_wrapper->title->value();
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt', $e);
  }

  return $title;
}

