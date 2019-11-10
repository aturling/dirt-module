<?php

/**
 * @file
 * This file specifies the definitions for custom fields created by this module.
 */

/**
 * Returns fields created by this module.
 */
function dirt_datatable_get_installed_fields() {
  // Data curation fields: allow admins to decide whether to include/exclude a
  // survey in the data search portal (include by default) and leave optionally
  // notes on why survey was excluded.

  return array(
    'field_dirt_datatable_in_portal' => array(
      'field_name' => 'field_dirt_datatable_in_portal',
      'type' => 'list_text',
      'cardinality' => 1,
      'settings' => array(
        'allowed_values' => array(
          'include' => t('Include this survey (default)'),
          'exclude' => t('Exclude this survey'),
        ),
        'dirt_admin_only' => array(
          'edit' => TRUE,
          'view' => TRUE,
        ),
      ),
      'locked' => TRUE,
    ),
    'field_dirt_datatable_admin_notes' => array(
      'field_name' => 'field_dirt_datatable_admin_notes',
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
 * Returns field instances added by this module (during install).
 */
function dirt_datatable_get_installed_instances() {
  $instances = array();

  $bundle = 'dirt_datatable_data_search_page';

  // Multiple instances of this field; append bundle name to instance key
  $instances['body' . $bundle] = array(
    'field_name' => 'body',
    'type' => 'text_with_summary',
    'entity_type' => 'node',
    'bundle' => $bundle,
    'label' => t('Body'),
    'widget' => array(
      'type'   => 'text_textarea_with_summary',
      'weight' => 1,
    ),
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'weight' => 1,
      ),
    ),
    'required' => TRUE,
  );

  return $instances;
}


/**
 * Returns survey field instances added to all survey types (in addition to
 * those added by core module).
 *
 * These instances will be created whenever a survey type is added, and deleted
 * whenever a survey type is deleted.
 *
 * @param[in] $type Machine name of survey type to attach field instance to.
 *
 * @retval Array of field instances common to all survey types.
 */
function dirt_datatable_get_survey_field_instances($type) {
  $instances = array();

  $instances['field_dirt_datatable_in_portal' . $type] = array(
    'field_name' => 'field_dirt_datatable_in_portal',
    'type' => 'list_text',
    'entity_type' => 'node',
    'bundle' => $type,
    'label' => t('Include Survey in Data Portal'),
    'required' => FALSE,
    'widget' => array(
      'type'   => 'options_buttons',
      'weight' => 0,
    ),
    'display' => array(
      'default' => array(
        'label' => 'inline',
        'weight' => 0,
      ),
    ),
    'description' => t('All surveys are included in the data search portal ' .
      'default. If this survey needs to be excluded from the data search ' .
      'portal (e.g., all values are incorrect), click "Exclude this survey" ' .
      'and type in some notes on why the survey is being excluded in the ' .
      'text field below. This survey will be archived in the system and ' .
      'visible to the participant through the data entry portal.') . 
      '<br><br><strong><em>' . t('If no selection is made above, the default ' .
      'option is "Include this survey" and this survey will be included in ' .
      'the data search table.') . '</em></strong>',
  );

  $instances['field_dirt_datatable_admin_notes' . $type] = array(
    'field_name' => 'field_dirt_datatable_admin_notes',
    'entity_type' => 'node',
    'bundle' => $type,
    'type'  => 'text_long',
    'label' => t('Data Curation Notes'),
    'widget' => array(
      'type' => 'text_textarea',
      'weight' => 1,
      'settings' => array(
        'rows' => 5,
      ),
    ),
    'required' => FALSE,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'weight' => 1,
        'type' => 'text_default',
      ),
    ),
    'description' => t('Enter notes above on why the survey is being ' .
      'excluded from the data search portal or why the content of cells has ' .
      'been altered. (Leave this field blank if not applicable.)'),
  );

  return $instances;
}


/**
 * Add field instances created in this module to new survey type that all survey
 * types have in common.
 */
function dirt_datatable_add_survey_field_instances_to_survey_type($type) {
  foreach(dirt_datatable_get_survey_field_instances($type) as $instance) {
    // Check that instance doesn't already exist before creating it:
    $current_instance = field_info_instance('node', $instance['field_name'], $type);
    if (!$current_instance) field_create_instance($instance);
    
    // Update admin-only field group to include these fields
    $modes = array('form', 'default');
    foreach($modes as $mode) {
      $group = field_group_load_field_group('group_dirt_admin_only_' . $mode, 'node', $type, $mode);
      if ($group && !in_array($instance['field_name'], $group->children)) {
        $group->children[] = $instance['field_name'];
        field_group_group_save($group);
      }
    }
  }
}


/**
 * Delete field instances created in this module from content type that is no
 * longer a survey type.
 */
function dirt_datatable_delete_survey_field_instances_from_survey_type($type) {
  $field_names = array();

  foreach (dirt_datatable_get_survey_field_instances($type) as $instance) {
    $field_names[] = $instance['field_name'];
    field_delete_instance($instance, FALSE);
  }

  field_cache_clear();

  // Update admin-only field group to exclude these fields
  $modes = array('form', 'default');
  foreach($modes as $mode) {
    $group = field_group_load_field_group('group_dirt_admin_only_' . $mode, 'node', $type, $mode);

    if ($group) {
      $group->children = array_diff($group->children, $field_names);
      field_group_group_save($group);
    }
  }
}

