<?php

/**
 * @file
 * This file includes functions that handle adding and removing survey types and
 * retrieving properties of survey types.
 */


/**
 * Adds a new survey type (i.e., designates an existing content type as a survey
 * type).
 *
 * @param[in] $type Survey type machine name.
 * @param[in] $collection_frequency Number of times per year survey is
 *   collected.
 * @param[in] $collection_months *Optional* Array of collection months (1-12),
 *   if not evenly spaced throughout the calendar year.
 * @param[in] $is_admin_only *Optional* Boolean whether survey is admin-only
 *   (default is false).
 */
function _dirt_add_survey_type($type, $collection_frequency, $collection_months=array(), $is_admin_only=FALSE) {
  // Store survey type parameters in array
  $type_info = array(
    'type' => $type,
    'collection_frequency' => $collection_frequency,
  );
  if ($is_admin_only) $type_info['admin_only'] = 1;
  
  try {
    // Begin by adding the survey type to the survey types database table.
    $fields = array();

    foreach(drupal_get_schema('dirt_survey_types')['fields'] as $field => $spec) {
      if (array_key_exists($field, $type_info))
        $fields[$field] = $type_info[$field];
    }

    // Handle database insertion based on collection months:
    if (!empty($collection_months)) {
      // If collection months given, insert one row per month
      foreach($collection_months as $idx => $month) {
        $fields['delta'] = $idx;
        $fields['collection_month'] = $month;
        dirt_insert_into_database_table('dirt_survey_types', $fields);
      }
    }
    else {
      // Otherwise insert one row without collection_month field
      dirt_insert_into_database_table('dirt_survey_types', $fields);
    }

    watchdog('dirt', 'Added new survey type %type.',
      array('%type' => $type), WATCHDOG_INFO);

    // Next, update permissions for data entry users to access this new survey type.
    _dirt_update_survey_type_permissions_for_data_entry_users($type, 'grant');

    // Add common field instances to this new survey type.
    _dirt_add_common_field_instances_to_survey_type($type);

    // Allow other modules to implement additional behavior upon addition of
    // new survey type.
    module_invoke_all('dirt_add_survey_type', $type);
  }
  catch (Exception $e) {
    watchdog('dirt', 'Error: unable to add new survey type %type.',
      array('%type' => $type), WATCHDOG_ERROR);

    // Throw exception
    throw $e;
  }
}


/**
 * Deletes a survey type.
 *
 * Note that this does not change the content type itself, only its status as a
 * survey type.
 *
 * @param[in] $type String type machine name.
 */
function _dirt_delete_survey_type($type) {
  try {
    // Begin by deleting survey from survey types database table.
    $conditions = array();
    $conditions[] = array(
      'field' => 'type',
      'value' => $type,
      'operator' => '=',
    );

    dirt_delete_from_database_table('dirt_survey_types', $conditions);

    watchdog('dirt', 'Removed survey type %type.',
      array('%type' => $type), WATCHDOG_INFO);

    // Update permissions: now that no longer a survey type, revoke add/edit
    // permissions for data entry users.
    _dirt_update_survey_type_permissions_for_data_entry_users($type, 'revoke');

    // Remove common field instances from this content type.
    _dirt_delete_common_field_instances_from_survey_type($type);

    // Allow other modules to implement additional behavior upon deletion of
    // new survey type.
    module_invoke_all('dirt_delete_survey_type', $type);
  }
  catch (Exception $e) {
    watchdog('dirt', 'Error: unable to remove survey type %type.',
      array('%type' => $type), WATCHDOG_ERROR);

    // Throw exception
    throw $e;
  }
}


/**
 * Returns list of machine names of all survey types in the system.
 *
 * @retval Array of survey type names (empty if none found).
 */
function _dirt_get_survey_types() {
  $types = array();

  if (db_table_exists('dirt_survey_types')) {
    $query = db_select('dirt_survey_types', 't');
    $query->fields('t', array('type'));
    $query->groupBy('t.type');
    $query->orderBy('t.collection_frequency');
    $types = $query->execute()->fetchCol();
  }

  return $types;
}


/**
 * Determines whether a content type is a survey type.
 *
 * @param[in] $type Node type machine name string.
 * 
 * @retval Boolean true if survey type, false otherwise.
 */
function _dirt_node_type_is_survey_type($type) {
  $survey_types = _dirt_get_survey_types();

  return in_array($type, $survey_types);
}


/**
 * Returns the most recently submitted survey node ID for a user. (Primarily
 * used to get the unique nid for one of the one-time survey types.)
 * Returns empty string if no nid found.
 *
 * @param[in] $uid User ID.
 * @param[in] $type Survey type (machine name).
 *
 * @retval String Node ID of survey node if found, or empty string otherwise.
 */
function _dirt_get_most_recent_survey_of_type_for_user($uid, $type) {
  // Initialize return
  $nid = '';

  // Do database query to retrieve most recent survey:
  $query = db_select('node', 'n');
  $query->condition('n.uid', $uid);
  $query->condition('n.type', $type);
  $query->fields('n', array('nid'));
  // Get the most recent
  $query->orderBy('n.nid', 'DESC');
  $result = $query->execute()->fetchAssoc(); // pull first result
  if ($result) {
    $nid = $result['nid'];
  }

  return $nid;
}


/**
 * Return the earliest survey date of all surveys submitted by a user.
 *
 * @param[in] $uid User ID (uid).
 * 
 * @retval Start date of earliest survey, or empty string if none found.
 */
function _dirt_get_earliest_survey_date($uid) {
  $survey_types = _dirt_get_survey_types();
  $field_name = 'field_dirt_date_of_survey';

  $query = db_select('node', 'n');
  $query->condition('n.uid', $uid);
  $query->condition('n.type', $survey_types, 'IN');
  $query->leftJoin('field_data_' . $field_name, 'd', 'n.nid = d.entity_id');
  $query->fields('d', array($field_name . '_value'));
  $query->orderBy('d.' . $field_name . '_value', 'ASC'); // order by survey date, ascending
  $result = $query->execute()->fetchAssoc(); // just need the first

  if ($result) return $result[$field_name . '_value'];
  else return '';
}


/**
 * Returns the collection frequency for a survey type.
 *
 * @param[in] $type String type machine name.
 *
 * @retval Integer collection frequency (0 if none found).
 */
function _dirt_get_collection_frequency_of_survey_type($type) {
  $frequency = 0;

  if (db_table_exists('dirt_survey_types')) {
    $query = db_select('dirt_survey_types', 't');
    $query->condition('t.type', $type);
    $query->fields('t', array('collection_frequency'));
    $query->groupBy('t.type');
    $result = $query->execute()->fetchAssoc(); // expect one result

    if ($result) $frequency = $result['collection_frequency'];
  }

  return (int) $frequency;
}


/**
 * Returns the collection frequency number as a string, e.g., "once a year",
 * "twice a year", etc.
 *
 * @param[in] $frequency_num Integer collection frequency.
 *
 * @retval String collection frequency.
 */
function _dirt_get_collection_frequency_str_of_survey_type($frequency_num) {
  switch ($frequency_num) {
    case DIRT_ONE_TIME_COLLECTION:
      return t('one-time');

    case 1:
      return t('once a year');

    case 2:
      return t('twice a year');

    case 3:
      return t('three times a year');

    case 4:
      return t('four times a year');

    case 5:
      return t('five times a year');

    case 6:
      return t('six times a year');

    case 7:
      return t('seven times a year');

    case 8:
      return t('eight times a year');

    case 9:
      return t('nine times a year');

    case 10:
      return t('ten times a year');

    case 11:
      return t('eleven times a year');

    case 12:
      return t('monthly');

    case 0:
      // Return empty string
      return '';

    default:
      // Shouldn't happen, but just in case...
      return t('@num times a year', array('@num' => $frequency));
  }
}


/**
 * Returns the collection months for a survey type.
 *
 * @param[in] $type String type machine name.
 *
 * @retval Array of collection months (empty if none found).
 */
function _dirt_get_collection_months_of_survey_type($type) {
  $months = array();

  if (db_table_exists('dirt_survey_types')) {
    $query = db_select('dirt_survey_types', 't');
    $query->condition('t.type', $type);
    $query->fields('t', array('collection_month'));
    $query->orderBy('t.collection_month', 'ASC');
    $result = $query->execute()->fetchAll();

    foreach ($result as $result_obj) {
      $month = $result_obj->collection_month;
      // Exclude zero
      if ($month > 0) $months[] = $result_obj->collection_month;
    }
  }

  return $months;
}



/**
 * Returns the admin-only entry status of a survey type.
 *
 * @param[in] $type String type machine name.
 *
 * @retval Integer admin-only status (1 if admin-only, 0 if not).
 */
function _dirt_get_admin_only_status_of_survey_type($type) {
  $admin_only = 0;

  if (db_table_exists('dirt_survey_types')) {
    $query = db_select('dirt_survey_types', 't');
    $query->condition('t.type', $type);
    $query->fields('t', array('admin_only'));
    $query->groupBy('t.type');
    $result = $query->execute()->fetchAssoc(); // expect one result

    if ($result) $admin_only = $result['admin_only'];
  }

  return $admin_only;
}


/**
 * Helper function to update permissions for survey type being added or removed.
 *
 * @param[in] $type Survey type machine name.
 * @param[in] $op Update operator ('grant' or 'revoke').
 */
function _dirt_update_survey_type_permissions_for_data_entry_users($type, $op) {
  // Get data entry role ID
  $data_entry_user_rid = variable_get('dirt_data_entry_user_role');

  // Actions to update for survey type
  $actions = array();
  $actions[] = 'create ' . $type . ' content';
  $actions[] = 'edit own ' . $type . ' content';

  switch ($op) {
    case 'grant':
      // Grant permissions to data entry users
      user_role_grant_permissions($data_entry_user_rid, $actions);

      // Additionally give permissions to admins
      // (Technically not necessary but this checks the boxes on the
      // admin/people/permissions page).
      $admin_rid = variable_get('user_admin_role');

      $actions[] = 'edit any ' . $type . ' content';
      $actions[] = 'delete own ' . $type . ' content';
      $actions[] = 'delete any ' . $type . ' content';

      user_role_grant_permissions($admin_rid, $actions);
      break;

    case 'revoke':
      // Revoke permissions for data entry users
      user_role_revoke_permissions($data_entry_user_rid, $actions);
      break;
  }
}


/**
 * Helper function to add field instances to new survey type that all survey
 * types have in common.
 */
function _dirt_add_common_field_instances_to_survey_type($type) {
  module_load_include('inc', 'dirt', 'includes/dirt.fields');

  foreach(dirt_get_survey_field_instances($type) as $instance) {
    // Check that instance doesn't already exist before creating it:
    $current_instance = field_info_instance('node', $instance['field_name'], $type);
    if (!$current_instance) field_create_instance($instance);
  }
}


/**
 * Helper function to delete common survey field instances from content type
 * that is no longer a survey type.
 */
function _dirt_delete_common_field_instances_from_survey_type($type) {
  module_load_include('inc', 'dirt', 'includes/dirt.fields');

  foreach (dirt_get_survey_field_instances($type) as $instance) {
    field_delete_instance($instance, FALSE);
  }

  field_cache_clear();
}

