<?php

/**
 * @file
 * This file contains functions that perform database operations on the data search tables.
 * Many of the functions use the db ops wrappers provided by the DIRT Core module.
 */


/**
 * Function to get all column values from the database for a given node ID (nid) and survey type.
 *
 * @param[in] $site_id Numeric site id.
 * @param[in] $nid Node id (nid) of survey.
 * @param[in] $type String of survey type as its machine name.
 *
 * @retval Array An assocative array where keys are column names and values are the field values.
 */
function dirt_datatable_get_survey_columns_from_database($site_id, $nid, $type) {
  $row = array();

  // Populate common fields
  dirt_datatable_populate_common_fields_in_row($row, $nid, $site_id, $type);

  // Get column data
  $table_col_data = dirt_datatable_get_table_column_data();

  // Initialize arrays to hold field info for queries
  // Lookup database column name of machine field name (used for single-, multi-
  // valued, and tax term queries):
  $db_col_name_of = array();

  // Lookup whether field is select field
  $is_select_field = array();

  // Lookup machine field name of database column name (used for field
  // collection field queries):
  $machine_name_of = array();

  // Single-valued fields (machine names) to be grouped in one query
  $single_valued_fields = array();

  // Multi-valued fields (machine names) to be queried separately
  $multi_valued_fields = array();

  // Field collection fields to be grouped together querying on field
  // collection name
  $field_collection_fields = array();

  // Loop through columns and organize into field types to do the queries later
  foreach($table_col_data->tablecols as $col_obj) {
    if ( ($col_obj->primary_field) && ($col_obj->survey_type === $type) ) {
      $field_type = $col_obj->field_type;
      $machine_field_name = $col_obj->field_name;
      $column_name = $col_obj->column_name;

      // Field types:
      switch ($field_type) {
        case 'single_valued':
          // Add to single-valued field array
          $single_valued_fields[] = $machine_field_name;
          // Store for lookup later:
          $db_col_name_of[$machine_field_name] = $column_name;
          $is_select_field[$machine_field_name] = $col_obj->field_select;
          break;

        case 'multi_valued':
          // Add to multi-valued field array
          $multi_valued_fields[] = $machine_field_name;
          // Store for lookup later:
          $db_col_name_of[$machine_field_name] = $column_name;
          $is_select_field[$machine_field_name] = $col_obj->field_select;
          break;

        case 'field_collection':
          $fc_name = $col_obj->fc_name;
          if (!array_key_exists($fc_name, $field_collection_fields)) {
            // If we haven't seen this field collection yet, initialize it
            $field_collection_fields[$fc_name] = array();
            $field_collection_fields[$fc_name]['max_delta'] = $col_obj->max_delta;
            $field_collection_fields[$fc_name]['fields'] = array();
            $field_collection_fields[$fc_name]['field_info'] = array();
          }
          $field_collection_fields[$fc_name]['fields'][] = $machine_field_name;
          $field_collection_fields[$fc_name]['field_info'][$column_name] = $col_obj->delta;
          $machine_name_of[$column_name] = $machine_field_name;
          $is_select_field[$column_name] = $col_obj->field_select;
          break;
      }
    }
  }

  // Run database queries:
  // First single valued fields and set column values
  $field_values = dirt_datatable_run_single_valued_query($nid, $single_valued_fields);

  foreach($single_valued_fields as $machine_field_name) {
    // Special case for drop-downs: we need the label, not the key
    if ($is_select_field[$machine_field_name]) {
      $field_values[$machine_field_name] = _dirt_datatable_get_select_labels_from_keys($field_values[$machine_field_name], $machine_field_name);
    }
    _dirt_datatable_set_column_value($row, $db_col_name_of[$machine_field_name], $field_values[$machine_field_name]);
  }

  // Next, multi-valued fields and set column values
  // (multi fields will be converted to comma separated text)
  foreach($multi_valued_fields as $machine_field_name) {
    $field_contents = dirt_datatable_get_standard_field($nid, $machine_field_name);

    // Special case for drop-downs: need label, not key
    if ($is_select_field[$machine_field_name]) {
      $field_contents = _dirt_datatable_get_select_labels_from_keys($field_contents, $machine_field_name);
    }
    _dirt_datatable_set_column_value($row, $db_col_name_of[$machine_field_name], $field_contents);
  }

  // Next, field collection fields and set column values
  foreach($field_collection_fields as $fc_name => $fc_info) {
    $fields = $fc_info['fields'];
    $fields = array_unique($fields);  // remove repeats
    $max_delta = $fc_info['max_delta'];

    $field_values = dirt_datatable_run_field_collection_query($nid, $fc_name, $fields, $max_delta);

    foreach ($fc_info['field_info'] as $column_name => $delta) {
      $machine_field_name = $machine_name_of[$column_name];
      // If select field, store label instead of key
      if ($is_select_field[$column_name]) {
        $field_values[$machine_field_name][$delta] = _dirt_datatable_get_select_labels_from_keys($field_values[$machine_field_name][$delta], $machine_field_name);
      }

      _dirt_datatable_set_column_value($row, $column_name, $field_values[$machine_field_name][$delta]);
    }
  }

  return $row;
}


/**
 * Database query for standard fields. Query one field only.
 * Multiple values are concatenated as comma-separated strings.
 *
 * @param[in] $nid Numeric Node ID (nid).
 * @param[in] $field_name Field name (machine name) string to query for the selected nid.
 *
 * @retval String Field value (with multiple values concatenated as comma-separated string).
 */
function dirt_datatable_get_standard_field($nid, $field_name) {
  $field_contents = '';

  // Set up query
  // Select on type node
  $query = db_select('node', 'n');
  // Restrict to given nid
  $query->condition('n.nid', $nid);
  // Get field
  $query->leftJoin('field_data_' . $field_name, 'fd', 'n.nid = fd.entity_id');
  $query->fields('fd', array($field_name . '_value'));
  // May get multiple results
  $result = $query->execute()->fetchAll();

  foreach ($result as $idx => $result_obj) {
    // Create comma-separated list of results
    if ($idx > 0) {
      $field_contents .= ', ';
    }
    $property_name = $field_name . '_value';
    $field_contents .= $result_obj->$property_name;
    unset($property_name);
  }
  unset($result);
  unset($query);

  return $field_contents;
}


/**
 * Run a database query returning field values from array $field_names
 * on node with node id = $nid where the fields are all single-valued.
 *
 * @param[in] $nid Numeric Node ID (nid).
 * @param[in] $field_names Array of field names to query on.
 *
 * @retval Array Associative array with keys as field names and values
 *  as the field values.
 */
function dirt_datatable_run_single_valued_query($nid, $field_names) {
  $field_values = array();  // Initialize

  if (!empty($field_names)) {
    // Set up query
    // Select on type node
    $query = db_select('node', 'n');
    // Restrict to given nid
    $query->condition('n.nid', $nid);
    // Get fields
    foreach ($field_names as $field_name) {
      $query->leftJoin('field_data_' . $field_name, $field_name, 'n.nid = ' . $field_name . '.entity_id');
      $query->fields($field_name, array($field_name . '_value'));
    }
    $result = $query->execute()->fetchAssoc(); // expect one result

    if ($result) {
      foreach ($field_names as $field_name) {
        $field_values[$field_name]  = (array_key_exists($field_name . '_value', $result)) ? $result[$field_name . '_value'] : '';
      }
    }
    else {
      // Store empty strings for empty fields because some fields are
      // not allowed to be stored as NULL in the table.
      foreach($field_names as $field_name) {
        $field_values[$field_name] = '';
      }
    }
  }

  return $field_values;
}


/**
 * Database query for field collection fields.
 *
 * @param[in] $nid Numeric Node ID (nid).
 * @param[in] $fc_name Field collection name.
 * @param[in] $field_names Field names (machine names) that are members of the
 *  field collection.
 * @param[in] $max_delta Number of items in field collection.
 *
 * @retval Array Associative array with keys as field names and values as an
 *   array of delta => field value.
 */
function dirt_datatable_run_field_collection_query($nid, $fc_name, $field_names, $max_delta) {
  $field_values = array();  // initialize

  foreach ($field_names as $field_name) {
    $field_values[$field_name] = array();
  }

  // Set up query
  // Select on type node
  $query = db_select('node', 'n');

  // Restrict to given nid
  $query->condition('n.nid', $nid);

  // Get field collection id
  $query->leftJoin('field_data_' . $fc_name, 'fc', 'n.nid = fc.entity_id');
  $query->fields('fc', array('delta'));

  // Restrict delta <= max_delta
  $query->condition('fc.delta', $max_delta, '<=');

  // Order by delta increasing so we get them in order 1, 2, 3, ...
  $query->orderBy('fc.delta', 'ASC');

  // Get field collection fields
  foreach ($field_names as $field_name) {
    $query->leftJoin('field_data_' . $field_name, $field_name, 'fc.' . $fc_name . '_value = ' . $field_name . '.entity_id');
    $query->fields($field_name, array($field_name . '_value'));
  }
  $result = $query->execute()->fetchAll();

  foreach ($result as $idx => $result_obj) {
    foreach ($field_names as $field_name) {
      $prop_name = $field_name . '_value';

      if (property_exists($result_obj, 'delta')) {
        $field_values[$field_name][$result_obj->delta] = (property_exists($result_obj, $prop_name)) ? $result_obj->$prop_name : '';
      }
    }
  }

  // If no results return empty strings
  foreach ($field_names as $field_name) {
    if (empty($field_values[$field_name])) {
      for ($idx = 0; $idx <= $max_delta; ++$idx) {
        $field_values[$field_name][$idx] = '';
      }
    }
  }

  return $field_values;
}


/**
 * Runs select on table for site ID and returns multi-dimensional array
 * of query results.
 * Optionally, limit to a certain survey type.
 *
 * @param[in] $site_id Numeric site ID to query on.
 * @param[in] $fields Array of column name strings to specify fields to be returned from query.
 * @param[in] $type *Optional* String of survey type as its machine name to limit query to.
 *
 * @retval Array An associative array, indexed first by node ID (nid), and second by column name string, e.g., $table_rows[$nid]['survey_month'] = 6.
 */
function dirt_datatable_get_rows_from_data_table($site_id, $fields, $type = '') {
  $table_rows = array();

  if (!empty($fields)) {
    // Run query
    $query = db_select(DIRT_SEARCH_TABLE_DATA, 't');
    // Limit to rows with site id = $site_id
    $query->condition('t.site_id', $site_id);
    if ( $type != '' ) {
      // Limit to surveys of type $type
      $query->condition('t.survey_type', $type);
    }
    // Add fields
    $query->fields('t', $fields);
    $results = $query->execute()->fetchAll();
    foreach($results as $result_obj) {
      $nid = $result_obj->nid;
      $table_rows[$nid] = array();
      foreach($result_obj as $key => $value) {
        $table_rows[$nid][$key] = $value;
      }

      unset($nid);
    }
    unset($results);
    unset($query);
  }

  return $table_rows;
}


/**
 * Gets all column values for specified column in table subject to max access
 * level (i.e. site's sharing permissions <= max access level, with max access
 * level = 2 meaning show all site ids in table).
 *
 * @param[in] $max_access_level Max access level integer (0, 1, or 2) of
 *   logged-in user.
 * @param[in] $column_name Column name to query on
 *
 * @retval Array An array of field values (unique).
 */
function dirt_datatable_get_all_of_column_from_data_table($max_access_level, $column_name) {
  // Initialize field values array
  $field_values = array();

  global $user;
  $logged_in_uid = $user->uid;

  // Run query:
  $query = db_select(DIRT_SEARCH_TABLE_DATA, 't');

  // Add an OR condition
  $db_or = db_or();
  // Limit to rows where sharing_permissions <= max_access_level
  $db_or->condition('t.dirt_user_sharing_perms', $max_access_level, '<=');
  // OR site ID matches logged-in user ID
  $db_or->condition('t.site_id', $logged_in_uid);
  $query->condition($db_or);

  // Add field
  $query->fields('t', array($column_name));

  // Count up number of unique field values
  $query->groupBy('t.' . $column_name);
  // Put them in ascending order
  $query->orderBy('t.' . $column_name);

  $result = $query->execute();
  while ($record = $result->fetchAssoc()) {
    $field_values[] = $record[$column_name];
  }

  unset($result);
  unset($query);

  return $field_values;
}


/**
 * Gets all site ids in table subject to max access level
 * (i.e. site's sharing permissions <= max access level, with max access
 * level = 2 meaning show all site ids in table).
 *
 * @param[in] $max_access_level Max access level integer (0, 1, or 2) of
 *   logged-in user.
 *
 * @retval Array An array of numeric site IDs (unique).
 */
function dirt_datatable_get_all_site_ids_from_data_table($max_access_level) {
  return dirt_datatable_get_all_of_column_from_data_table($max_access_level, 'site_id');
}


/**
 * Gets all habitat types in table subject to max access level.
 *
 * @param[in] $max_access_level Max access level integer (0, 1, or 2) of
 *   logged-in user.
 *
 * @retval Array An array of habitat type strings (unique).
 */
function dirt_datatable_get_all_habitat_types_from_data_table($max_access_level) {
  return dirt_datatable_get_all_of_column_from_data_table($max_access_level, 'dirt_site_habitat_type');
}


/**
 * Gets all counties in table subject to max access level
 * (i.e. site's sharing permissions <= max access level, with max access level = 2
 * meaning show all site ids in table).
 *
 * @param[in] $max_access_level Max access level integer (0, 1, or 2) of
 *   logged-in user.
 *
 * @retval Array An array of county strings (unique).
 */
function dirt_datatable_get_all_counties_from_data_table($max_access_level) {
  $counties = dirt_datatable_get_all_of_column_from_data_table($max_access_level, 'dirt_site_county');

  // Remove empty strings from array
  return array_filter($counties);
}


/**
 * Returns entire data table formatted for data search table page, i.e.,
 *  - rows with matching site_id and survey date (month+year) are combined
 *  - nid and survey_type columns are hidden
 *  - only rows with sharing_permission <= max_access_level returned.
 *
 * Note that, as an exception to the rule above, all participants will be able
 * to view their own data in the search table while logged in (even if their
 * data is semiprivate).
 *
 * @param[in] $max_access_level Max access level integer determined by logged-in
 *  user's access (0, 1, or 2).
 * @retval Array An associative array indexed first by row number and second by
 *  column name, e.g. $table_rows[$row_number]['survey_month'] = 6.
 */
function dirt_datatable_get_all_data_from_data_table($max_access_level) {
  // Initialize result array
  $table_rows = array();

  // Get all column data
  $table_col_data = dirt_datatable_get_table_column_data();

  // Field names split into two groups: non-floats (ints, strings, etc.) and floats
  $fields = array();
  $float_fields = array();
  $user_is_admin = user_access('administer nodes');

  foreach($table_col_data->tablecols as $col_obj) {
    if ( ($col_obj->primary_field) && (!($col_obj->admin_only) || $user_is_admin) ) {
      $query_type = $col_obj->query_type;
      if ($query_type === 'standard') {
        $fields[] = $col_obj->column_name;
      }
      elseif ($query_type === 'float') {
        $float_fields[] = $col_obj->column_name;
      }
      unset($query_type);
    }
  }
  unset($table_col_data);

  // Get uid of logged-in user (0 for not logged in)
  global $user;
  $logged_in_uid=$user->uid;

  // Run query:
  $query = db_select(DIRT_SEARCH_TABLE_DATA, 't');

  // Begin an OR condition:
  $db_or = db_or();
  // Limit to rows where sharing_permissions <= max_access_level
  $db_or->condition('t.dirt_user_sharing_perms', $max_access_level, '<=');
  // OR the site ID is the logged-in user (so that all users see their own data
  // even if it's private)
  // Note if user is anonymous, site ID is 0 which is not a valid site ID so no
  // additional results will be returned
  $db_or->condition('t.site_id', $logged_in_uid);
  $query->condition($db_or);

  // Additionally, exclude group info rows UNLESS logged-in user is an admin
  if (!$user_is_admin)
    $query->condition('t.survey_type', 'Group', '!=');

  // Also only include surveys marked as included (default option) vs. excluded
  $query->condition('t.include', 'include');

  // Concatenate survey types separated by a single space
  $query->addExpression("GROUP_CONCAT(survey_type separator ' ')", 'row_types');

  // Special case for color number by hour times: Need to convert times to HH:MM
  // (note datetime stored in database in UTC time so force conversion to local
  // time)

  $field_info = field_info_field('field_dirt_color_number_by_hour');
  if ($field_info) {
    $time_fields = array();

    // Get UTC offset in hours for site time zone
    // All color number hour times stored with default date of Jan 1 (no year)
    $offset_hours = date('Z', strtotime('January 1'))/3600;
    $offset_hours_abs = abs($offset_hours);

    $cardinality = $field_info['cardinality'];
    for($k = 1; $k <= $cardinality; ++$k) {
      $field_name = 'dirt_soil_resp_time_' . $k;
      $time_fields[] = $field_name;

      if ($offset_hours > 0)
        $query->addExpression('MAX(DATE_FORMAT(DATE_ADD(' . $field_name . ', INTERVAL ' . $offset_hours_abs . " HOUR), '%H:%i'))", $field_name);
      elseif ($offset_hours < 0)
        $query->addExpression('MAX(DATE_FORMAT(DATE_SUB(' . $field_name . ', INTERVAL ' . $offset_hours_abs . " HOUR), '%H:%i'))", $field_name);
      else
        $query->addExpression('MAX(DATE_FORMAT(' . $field_name . "), '%H:%i'))", $field_name);
    }

    $fields = array_diff($fields, $time_fields);
  }

  // Add MAX(column_name) expressions for the grouping:
  // For all fields, we want MAX(column_name) when grouping rows with matching
  // site id and survey date which returns the one non-null column value in each
  // column (we know it's only one because there shouldn't be more than one
  // survey type per survey date)
  foreach($fields as $field_name) {
    $query->addExpression('MAX(' . $field_name . ')', $field_name);
  }

  // Add survey month separately because need to pad with leading zero
  $query->addExpression("LPAD(MAX(survey_month), 2, '0')", 'survey_month');

  // Add survey month abbreviation
  $query->addExpression('MAX(survey_month_abbr)', 'survey_month_abbr');

  // Add nid separately, need to concatenate to form comma separated list (admin users only)
  if ($user_is_admin)
    $query->addExpression("GROUP_CONCAT(nid, ':', survey_type ORDER BY survey_type SEPARATOR ',')", 'nids');

  // Add geo coords
  $query->addExpression('MAX(dirt_site_geo_coords_lat)', 'dirt_site_geo_coords_lat');
  $query->addExpression('MAX(dirt_site_geo_coords_lon)', 'dirt_site_geo_coords_lon');

  // Get year and month together
  $query->addExpression("CONCAT(survey_year, '-', LPAD(survey_month, 2, '0'))", 'survey_year_month');

  // Add ROUND(MAX(column_name), 2) expressions to round floats to up to 2 decimal places (trim extra zeroes)
  foreach($float_fields as $field_name) {
    $query->addExpression('TRIM(ROUND(MAX(' . $field_name . '), 2))+0', $field_name);
  }

  // Group by site id, survey year, and survey month
  $query->groupBy('t.site_id');
  $query->groupBy('t.survey_year');
  $query->groupBy('t.survey_month');

  // Order by first site id (ascending), then survey year (descending), then survey month (descending)
  $query->orderBy('t.site_id');
  $query->orderBy('t.survey_year', 'DESC');
  $query->orderBy('t.survey_month', 'DESC');
  $result = $query->execute();
  while($record = $result->fetchAssoc()) {
    $table_rows[] = $record;
  }

  unset($query);
  unset($result);
  unset($fields);
  unset($float_fields);

  // Survey link (admin-only) fields need additional formatting before being
  // displayed in the table.
  // (This slows page load somewhat, but only for site admins.)
  if ($user_is_admin) {
    $num_rows = count($table_rows);

    for($idx = 0; $idx < $num_rows; ++$idx) {
      // Create survey links from nids (admin users only):
      $survey_links = '';
      $nids_and_types = explode(',', $table_rows[$idx]['nids']);

      foreach($nids_and_types as $k => $nid_and_type) {
        if ($k > 0)  $survey_links .= ' ';

        $parts = explode(':', $nid_and_type);
        $nid = $parts[0];
        $type_long = $parts[1];

        // Get abbreviation for survey type
        $type = _dirt_datatable_get_survey_type_abbr($type_long);

        $survey_links .= '<span class="link-type-' . $type_long . '">';
        $survey_links .= $type . ':';
        $survey_links .= l($nid, 'node/' . $nid, array('attributes' => array('target'=>'_blank')));
        $survey_links .= '</span>';

        unset($parts);
        unset($type);
        unset($type_long);
        unset($nid);
      }

      $table_rows[$idx]['nids'] = $survey_links;
    }

    unset($num_rows);
  }

  // Return in expected format for datatables data
  return array("data" => $table_rows);
}


/**
 * Returns array of node IDs all with the same site ID, survey type,
 * and survey date (year and month) as those given as input, excluding
 * the node ID given as input.
 *
 * @param[in] $site_id Site ID.
 * @param[in] $type String survey type (machine name).
 * @param[in] $survey_year Survey year in the form YYYY.
 * @param[in] $survey_month Survey month in the form MM.
 * @param[in] $nid Node ID (exclude in duplicate count).
 *
 * @retval Array of duplicate survey node IDs (nids).
 */
function dirt_datatable_get_duplicate_surveys_in_data_table($site_id, $type, $survey_year, $survey_month, $nid) {
  $duplicate_surveys = array();

  // db query for duplicates
  $query = db_select(DIRT_SEARCH_TABLE_DATA, 't');
  $query->condition('t.site_id', $site_id);   // limit to site ID
  $query->condition('t.survey_type', $type);  // limit to survey type
  $query->condition('t.survey_year', $survey_year); // same survey year
  $query->condition('t.survey_month', $survey_month); // same month
  $query->condition('t.include', 'include'); // limit to surveys included in table
  $query->condition('t.nid', $nid, '!='); // exclude node ID
  $query->fields('t', array('nid'));
  $results = $query->execute()->fetchAll();
  foreach ($results as $result_obj) {
    $duplicate_surveys[] = $result_obj->nid;
  }
  unset($results);
  unset($query);

  return $duplicate_surveys;
}


/**
 * Returns all duplicate surveys of a survey type.
 *
 * @param[in] $type String machine name of survey content type.
 *
 * @retval Object resulting from database query, with properties 'username', 'site_id',
 *   'survey_year', and 'survey_month'.
 */
function dirt_datatable_get_duplicate_surveys_in_data_table_for_type($type) {
  // Set up database query
  $query = db_select(DIRT_SEARCH_TABLE_DATA, 't');
  $query->condition('t.survey_type', $type);  // limit to survey type
  $query->condition('t.include', 'include'); // limit to surveys included in data table
  $query->fields('t', array('username', 'site_id', 'survey_year', 'survey_month'));
  $query->addExpression("COUNT(nid)", 'nid_count'); // count nids
  $query->addExpression("GROUP_CONCAT(nid separator ', ')", 'nid_list'); // list nids
  $query->groupBy('t.site_id');
  $query->groupBy('t.survey_year');
  $query->groupBy('t.survey_month');
  $query->havingCondition('nid_count', '1', '>'); // count > 1 indicates duplicates
  $query->orderBy('t.site_id');
  $query->orderBy('t.survey_year');
  $query->orderBy('t.survey_month');
  $results = $query->execute()->fetchAll();

  return $results;
}


/**
 * Load table column data from dirt_datatable_tablecols
 * and dirt_datatable_colgroups tables.
 *
 * @retval Object Object representing table column data, with two
 *  properties:
 *  colgroups => all rows from dirt_datatable_colgroups.
 *  tablecols => all rows from dirt_datatable_tablecols.
 */
function dirt_datatable_get_table_column_data() {
  $col_data = new stdClass();

  // Add the group info
  $col_data->colgroups = dirt_datatable_get_table_column_groups();

  // Add the table column data
  $col_data->tablecols = dirt_datatable_get_table_column_metadata();

  return $col_data;
}


/**
 * Load table column metadata from dirt_datatable_tablecols.
 * Sorts the table columns first by column group, then by
 * weight within group.
 *
 * @retval Array of classes, one per table row, where class
 *   properties are table columns.
 */
function dirt_datatable_get_table_column_metadata() {
  // Need to sort by colgroup weight first, then weight in column
  $cols = array();
  $table_name = DIRT_SEARCH_TABLE_COLUMNS;

  if (db_table_exists($table_name)) {
    $query = db_select($table_name, 't');
    $query->fields('t');
    $query->leftJoin(DIRT_SEARCH_TABLE_COLUMN_GROUPS, 'c', 'c.gid = t.colgroup');
    $query->orderBy('c.weight');
    $query->orderBy('t.colgroup');
    $query->orderBy('t.weight');
    $cols = $query->execute()->fetchAll();
  }

  return $cols;
}


/**
 * Get all columns of a database table.
 *
 * @param[in] $table_name String name of table in database.
 * @param[in] $sort_weights *Optional* Boolean whether to return
 *   rows sorted by 'weight' column, if it exists. Defaults
 *   to FALSE.
 *
 * @retval Array of classes, one per table row, where class
 *   properties are table columns.
 */
function dirt_datatable_get_all_table_columns($table_name, $sort_weights = FALSE) {
  $cols = array();

  if (db_table_exists($table_name)) {
    $query = db_select($table_name, 't');
    $query->fields('t');
    if ($sort_weights && db_field_exists($table_name, 'weight'))
      $query->orderBy('t.weight');
    $cols = $query->execute()->fetchAll();
  }

  return $cols;
}


/**
 * Load variable filter groups from DIRT_SEARCH_TABLE_FILTER_GROUPS.
 *
 * @retval Array of classes, one per table row, where class
 *   properties are table columns and rows.
 */
function dirt_datatable_get_filter_groups() {
  return dirt_datatable_get_all_table_columns(DIRT_SEARCH_TABLE_FILTER_GROUPS, TRUE);
}


/**
 * Get filter element id from the filter ID (fid).
 *
 * @param[in] $fid Integer filter ID
 *
 * @retval String element ID
 */
function dirt_datatable_get_filter_element_id($fid) {
  $query = db_select(DIRT_SEARCH_TABLE_FILTER_GROUPS, 'f');
  $query->condition('f.fid', $fid);
  $query->fields('f', array('element_id'));
  $result = $query->execute()->fetchAssoc();

  if ($result) {
    return $result['element_id'];
  }
  else {
    return '';
  }
}


/**
 * Return list of all data table column groups contained in a filter group.
 *
 * @param[in] $fid Filter group ID.
 *
 * @retval Array of column group IDs belonging to filter group.
 */
function dirt_datatable_get_column_groups_in_filter_group($fid) {
  $query = db_select(DIRT_SEARCH_TABLE_COLUMN_GROUPS, 't');
  $query->condition('t.filter_group', $fid);
  $query->fields('t', array('gid'));
  $gids = $query->execute()->fetchCol();

  return $gids;
}


/**
 * Return list of all data table columns contained in a variable filter group.
 *
 * @param[in] $fid Filter group ID.
 * @param[in] $include_admin_only *Optional* Boolean whether to include
 *   admin-only columns, defaults to TRUE.
 * @param[in] $include_hidden *Optional* Boolean whether to include hidden
 *   columns, defaults to TRUE.
 *
 * @retval Array of column names belonging to filter group.
 */
function dirt_datatable_get_columns_in_filter_group($fid, $include_admin_only=TRUE, $include_hidden=TRUE) {
  // Initialize return array
  $col_names = array();

  // Begin database query
  $query = db_select(DIRT_SEARCH_TABLE_COLUMNS, 't');

  if (!($include_admin_only)) {
    // Exclude admin-admin only columns
    $query->condition('t.admin_only', 0);
  }

  if (!($include_hidden)) {
    // Exclude hidden columns
    $query->condition('t.visible', 1);
  }

  // Join in column group data
  $query->leftJoin(DIRT_SEARCH_TABLE_COLUMN_GROUPS, 'c', 't.colgroup = c.gid');

  // Join in filter group data
  $query->leftJoin(DIRT_SEARCH_TABLE_FILTER_GROUPS, 'f', 'c.filter_group = f.fid');

  // Limit to the filter group passed in as parameter
  $query->condition('f.fid', $fid);

  // Get the column name
  $query->fields('t', array('column_name'));

  // Group by field label to remove duplicates (in case of field collections)
  $query->groupBy('t.field_label');

  // Order by column group first, then weight within column group
  $query->orderBy('c.gid');
  $query->orderBy('t.weight');

  // Get results
  $result = $query->execute()->fetchAll();

  foreach($result as $result_obj) {
    $col_names[] = $result_obj->column_name;
  }

  return $col_names;
}


/**
 * Returns filter group ID (fid) for site description filter.
 *
 * @retval Filter group ID or empty string if not found.
 */
function dirt_datatable_get_site_desc_fid() {
  $fid = '';

  // Begin database query
  $query = db_select(DIRT_SEARCH_TABLE_FILTER_GROUPS, 'f');
  $query->condition('f.element_id', 'site-desc');
  $query->fields('f', array('fid'));
  $result = $query->execute()->fetchAssoc(); // expect one result
  if ($result) {
    $fid = $result['fid'];
  }

  return $fid;
}



/**
 * Load table column groups from dirt_datatable_colgroups.
 * Sorts the column groups by weight.
 *
 * @retval Array of classes, one per table row, where class
 *   properties are table columns.
 */
function dirt_datatable_get_table_column_groups() {
  return dirt_datatable_get_all_table_columns(DIRT_SEARCH_TABLE_COLUMN_GROUPS, TRUE);
}


/**
 * Return list of column IDs belonging to column group.
 *
 * @param $gid Column group ID (gid).
 *
 * @retval Array of column IDs (cids).
 */
function dirt_datatable_get_columns_in_column_group($gid) {
  $query = db_select(DIRT_SEARCH_TABLE_COLUMNS, 't');
  $query->condition('t.colgroup', $gid);
  $query->fields('t', array('cid'));
  $cids = $query->execute()->fetchCol();

  return $cids;
}


/**
 * Updates a single row specified by its Node ID in the data table in database.
 * To update all rows for a Site ID, use the function
 * dirt_datatable_update_rows_in_data_table() (for any subset of columns)
 * or dirt_datatable_update_site_desc_columns_in_data_table() (for site
 * description columns specifically).
 *
 * @param[in] $row Assocative array where keys are column names and values are
 *  column values to be updated.
 * @param[in] $nid Node ID (nid) of survey row to be updated.
 * @param[in,out] $num_rows_updated This integer is incremented by the number
 *  of rows updated when the database update function is called.
 *
 * @retval Boolean Whether the update operation was successful.
 */
function dirt_datatable_update_row_in_data_table($row, $nid, &$num_rows_updated) {
  $success = FALSE;
  $updated_row_count = 0;

  // First of all if $nid is not defined the update will not go through, so
  // indicate failure and don't bother (btw 0 counts as undefined)
  if (empty($nid)) {
    watchdog('dirt_datatable', 'Error: nid undefined; cannot update row with no nid.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // Wrap in try/catch to not break entire site, log any errors
  try {
    $conditions = array(0 => array(
      'field' => 'nid',
      'value' => $nid,
      'operator' => '=')
    );

    $updated_row_count = dirt_update_database_table(DIRT_SEARCH_TABLE_DATA, $row, $conditions);
    if (!is_null($updated_row_count)) {
      $success = TRUE;
      $num_rows_updated += $updated_row_count;
    }
  }
  catch (Exception $e) {
    $error_text = print_r($e, TRUE);
    watchdog('dirt_datatable', 'Error when updating DataTable row with nid %nid in database: %error.',
      array('%nid' => $nid, '%error' => $error_text), WATCHDOG_ERROR);
    unset($error_text);
  }

  unset($updated_row_count);
  return $success;
}


/**
 * Updates multiple rows specified by the Site ID in the data table in
 * database.
 *
 * @param[in] $row Assocative array where keys are column names and values are
 *  column values to be updated.
 * @param[in] $site_id Site ID of survey rows to be updated.
 * @param[in,out] $num_rows_updated This integer is incremented by the number
 *  of rows updated when the database update function is called.
 *
 * @retval Boolean Whether the update operation was successful.
 */
function dirt_datatable_update_rows_in_data_table($row, $site_id, &$num_rows_updated) {
  $success = FALSE;
  $updated_row_count = 0;

  // First of all if $site_id is empty then something went wrong, report error and don't bother (btw 0 counts as undefined)
  if (empty($site_id)) {
    watchdog('dirt_datatable', 'Cannot update DataTable rows for an empty site ID.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // Update all user fields that appear in all surveys for this site ID
  // Wrap in try/catch to not break entire site, log any errors
  try {
    $conditions = array(0 => array(
      'field' => 'site_id',
      'value' => $site_id,
      'operator' => '=')
    );
    $updated_row_count = dirt_update_database_table(DIRT_SEARCH_TABLE_DATA, $row, $conditions);
    if (!is_null($updated_row_count)) {
      $num_rows_updated += $updated_row_count;
      $success = TRUE;
    }
  }
  catch (Exception $e) {
    $error_text = print_r($e, TRUE);
    watchdog('dirt_datatable', 'Error when updating rows for Site ID %id in DataTable: %error.',
      array('%id' => $site_id, '%error' => $error_text), WATCHDOG_ERROR);
    unset($error_text);
  }
  unset($updated_row_count);

  return $success;
}


/**
 * Update the site description info in all rows of the table for a
 * site ID.
 *
 * @param[in] $site_id Site ID.
 *
 * @retval Boolean Success of row update (TRUE if successful, FALSE otherwise).
 */
function dirt_datatable_update_site_desc_info_for_site_id($site_id) {
  $success = FALSE;
  $num_rows_updated = 0;

  // Get site description node ID
  $site_desc_nid = dirt_get_most_recent_survey_of_type_for_user($site_id, 'dirt_site_description_survey');

  // If site description survey exists, get its column values to update.
  // Otherwise set $row equal to empty array to clear out site
  // description columns.
  $row = (!empty($site_desc_nid)) ? dirt_datatable_get_survey_columns_from_database($site_id, $site_desc_nid, 'dirt_site_description_survey') : array();

  // Update all other rows with these (possibly empty) values
  if (dirt_datatable_update_site_desc_columns_in_data_table($row, $site_id, $num_rows_updated))
    $success = TRUE;
  else
    watchdog('dirt_datatable', 'Error updating site description survey in DataTable for site id %site',
      array('%site' => $site_id), WATCHDOG_INFO);

  return $success;
}


/**
 * Updates site description filter columns in all rows for the given Site ID.
 * Similar to dirt_datatable_update_rows_in_data_table() but updates site
 * description columns specifically for all rows with the same site ID.
 *
 * @param[in] $row Assocative array where keys are column names and values are
 *   column values to be updated. If $row is empty, site description columns
*    will be cleared out in all rows.
 * @param[in] $site_id Numeric Site ID.
 * @param[in,out] $num_rows_updated This integer is incremented by the number of
 *   rows updated when the database update function is called.
 *
 * @retval Boolean Whether the update operation was successful (TRUE if so,
 *   otherwise FALSE).
 */
function dirt_datatable_update_site_desc_columns_in_data_table($row, $site_id, &$num_rows_updated) {
  $success = FALSE;
  $updated_row_count = 0;

  // First of all if $site_id is empty then something went wrong, report error and don't bother (btw 0 counts as undefined)
  if (empty($site_id)) {
    watchdog('dirt_datatable', 'Cannot update site description columns for an empty site ID.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // We won't update ALL site description columns, only those belonging
  // to the site description filter group.

  // Get the filter group ID (fid) for the site description filter group
  $fid = dirt_datatable_get_site_desc_fid();

  // Get the column names from the fid
  $cols_to_include = (!empty($fid)) ? dirt_datatable_get_columns_in_filter_group($fid) : array();

  // Add habitat type fields as a special case (grouped with common fields, but still
  // have to have their values pulled from site desc survey)
  $habitat_cols = array('dirt_site_habitat_type', 'dirt_site_type_of_crop', 'dirt_site_cropping_system');
  foreach($habitat_cols as $habitat_col) {
    if (db_field_exists(DIRT_SEARCH_TABLE_DATA, $habitat_col)) $cols_to_include[] = $habitat_col;
  }

  // Create row that only includes these columns and their values. If values are missing,
  // set to NULL.
  $site_desc_row = array();
  foreach($cols_to_include as $column_name) {
    $site_desc_row[$column_name] = (array_key_exists($column_name, $row)) ? $row[$column_name] : NULL;
  }

  // Special case: format habitat type to include "other: " prefix for search
  // filter. Do it here to change NULLs to "Other: (none entered)" for instance
  // if no site desc survey submitted yet.
  if (array_key_exists('dirt_site_habitat_type', $site_desc_row))
    $site_desc_row['dirt_site_habitat_type'] = _dirt_datatable_format_habitat_field($site_desc_row['dirt_site_habitat_type']);

  // Update all site desc fields that appear in all surveys for this site id
  // Wrap in try/catch to not break entire site, log any errors
  try {
    $conditions = array(0 => array(
      'field' => 'site_id',
      'value' => $site_id,
      'operator' => '='),
    );
    $updated_row_count = dirt_update_database_table(DIRT_SEARCH_TABLE_DATA, $site_desc_row, $conditions);
    if (!is_null($updated_row_count)) $num_rows_updated += $updated_row_count;
    $success = TRUE;
  }
  catch (Exception $e) {
    $error_text = print_r($e, TRUE);
    watchdog('dirt_datatable', 'Error when updating site description columns in DataTable: %error.',
      array('%error' => $error_text), WATCHDOG_ERROR);
    unset($error_text);
  }
  unset($updated_row_count);

  return $success;
}


/**
 * Populate common fields in a row for the given survey node ID (nid), Site ID,
 * and survey type.
 *
 * @param[in,out] $row Associative array where keys are column names and values
 *   are the field values.
 * @param[in] $nid Numeric Node ID (nid).
 * @param[in] $site_id Numeric Site ID.
 * @param[in] $type String of survey type as its machine name.
 */
function dirt_datatable_populate_common_fields_in_row(&$row, $nid, $site_id, $type) {
  // Store Node ID (nid), Site ID, and type:
  $row['nid'] = $nid;
  $row['site_id'] = $site_id;
  $row['survey_type'] = $type;

  // Get info from uid
  $sharing_permissions = 0; // default public
  $collection_status = 'active'; // default active
  $username = $first_name = $last_name = '';

  try {
    $user_wrapper = entity_metadata_wrapper('user', $site_id);
    $sharing_perms_str = $user_wrapper->field_dirt_user_sharing_perms->value();
    $sharing_permissions = dirt_datatable_get_sharing_perms_integer_value($sharing_perms_str);
    $collection_status = $user_wrapper->field_dirt_user_collection_state->value();
    $username = $user_wrapper->name->value();
    $first_name = $user_wrapper->field_dirt_user_first_name->value();
    $last_name = $user_wrapper->field_dirt_user_last_name->value();
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt_datatable', $e);
  }

  // Store user fields
  $row['username'] = $username;
  $row['dirt_user_first_name'] = $first_name;
  $row['dirt_user_last_name'] = $last_name;
  $row['dirt_user_sharing_perms'] = $sharing_permissions;
  $row['dirt_user_collection_state'] = ucfirst($collection_status);

  // Get included/excluded from database query
  $include = dirt_datatable_get_standard_field($nid, 'field_dirt_datatable_in_portal');
  $include = (empty($include)) ? 'include' : $include;   // default to 'include' if empty
  $row['include'] = $include;

  // Get survey date from database query
  $date_field_lookup = dirt_datatable_get_standard_field($nid, 'field_dirt_date_of_survey');

  if (!empty($date_field_lookup)) {
    // Get survey year
    $date_field = date('Y', strtotime($date_field_lookup));
    $row['survey_year'] = render($date_field);

    // Get survey month
    $date_field = date('m', strtotime($date_field_lookup));

    // Convert to integer (specify base 10 because some month strings begin with 0s)
    $row['survey_month'] = intval(render($date_field), 10);

    // Also store month abbreviation
    $row['survey_month_abbr'] = date('M', strtotime($date_field_lookup));   
  }
  else {
    $row['survey_year'] = '';
    $row['survey_month'] = '';
    $row['survey_month_abbr'] = '';
  }
}


/**
 * After a user profile is saved, update user fields for all rows
 * in data table.
 *
 * @param[in] $account The user object on which the operation is performed.
 */
function dirt_datatable_update_data_table_after_user_save($account) {
  // Get the user profile columns from table col metadata.
  // They all belong to 'common' group and have a standard
  // query type. There will be some common columns that
  // meet this criteria but are not user fields, but
  // we'll check that the fields exist in the user profile
  // later. Also some columns have to be handled as a
  // special case.

  // Get table col metadata:
  $tablecols = dirt_datatable_get_table_column_metadata();

  // Get common standard columns (includes some columns that aren't user fields)
  $cols = array();
  foreach($tablecols as $col_info) {
    if (($col_info->survey_type === 'common') && ($col_info->query_type === 'standard')) {
      if ($col_info->field_name != '') $cols[$col_info->column_name] = $col_info->field_name;
    }
  }

  // Attempt to get column field values from newly updated account.
  // (Check first that field is a user field.)
  try {
    $user_wrapper = entity_metadata_wrapper('user', $account);
    foreach($cols as $column_name => $field_name) {
      if (array_key_exists($field_name, $account)) {
        $value = $user_wrapper->$field_name->value();
        $row[$column_name] = $value;
      }
    }
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt_datatable', $e);

    // If something goes wrong, fill columns with empty strings
    foreach($cols as $column_name => $field_name) $row[$column_name] = '';
  }

  // Special cases:
  // Collection status needs to be capitalized (active vs. closed)
  if (array_key_exists('dirt_user_collection_state', $row))
    $row['dirt_user_collection_state'] = ucfirst($row['dirt_user_collection_state']);

  // Sharing permissions field:
  $sharing_permissions = 0; // default public
  try {
    $sharing_perms_str = $user_wrapper->field_dirt_user_sharing_perms->value();
    $sharing_permissions = dirt_datatable_get_sharing_perms_integer_value($sharing_perms_str);
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt_datatable', $e);
  }
  $row['dirt_user_sharing_perms'] = $sharing_permissions;

  // Update rows in DataTable for this site ID:
  $num_rows_updated = 0;
  $site_id = $row['site_id'];
  if (dirt_datatable_update_rows_in_data_table($row, $site_id, $num_rows_updated)) {
    watchdog('dirt_datatable', 'Updated %num rows in DataTable after user profile saved for Site ID %id.',
      array('%num' => $num_rows_updated, '%id' => $site_id), WATCHDOG_INFO);
  }
  else {
    watchdog('dirt_datatable', 'Error updating user profile fields in DataTable for Site ID %id.',
      array('%id' => $site_id), WATCHDOG_ERROR);
  }
}


/**
 * Adds a row to the column metadata table for a new survey field being added
 * to the data search table.
 * Assumes that the data table schema has already been updated with a column
 * representing this survey field.
 *
 * @param[in] $column_info Array keyed by table field with values to insert into
 *  database, either created manually or retrieved from calling the function
 *  dirt_datatable_get_column_metadata_from_active_node_field().
 */
function _dirt_datatable_add_column_metadata_for_field($column_info) {
  // Add row to column metadata table.
  $fields_arr = array();

  $table_columns = drupal_get_schema(DIRT_SEARCH_TABLE_COLUMNS)['fields'];

  foreach ($table_columns as $column => $spec) {
    if (array_key_exists($column, $column_info))  $fields_arr[$column] = $column_info[$column];
  }

  dirt_insert_into_database_table(DIRT_SEARCH_TABLE_COLUMNS, $fields_arr);
}


//------------------------------------------------------------------------------
// Database deletions.
//------------------------------------------------------------------------------/

/**
 * Deletes a single row from the data table given by Node ID (nid).
 *
 * @param[in] $nid Numeric Node ID (nid) of row to delete from the data table.
 *
 * @retval Boolean Whether the database deletion operation was successful.
 */
function dirt_datatable_delete_row_from_data_table($nid) {
  $success = FALSE;

  // First of all if nid is empty then delete won't work, indicate failure and don't bother
  if (empty($nid)) {
    watchdog('dirt_datatable', 'Error: nid undefined; cannot delete row with no nid.',
      array(), WATCHDOG_ERROR);
    return FALSE;
  }

  // Wrap in try/catch to not break entire site, log any errors
  try {
    db_delete(DIRT_SEARCH_TABLE_DATA)->condition('nid', $nid)->execute();
    $success = TRUE;
  }
  catch (Exception $e) {
    $error_text = print_r($e, TRUE);
    watchdog('dirt_datatable', 'Error when deleting row with nid %nid in database: %error.',
      array('%nid' => $nid, '%error' => $error_text), WATCHDOG_ERROR);
    unset($error_text);
  }

  return $success;
}


/**
 * Forces an update of the data search database table.
 * Uses Batch API.
 *
 * @retval Array containing keys:
 *   'inserted' => Number of rows inserted into the table,
 *   'updated' => Number of rows updated in the table,
 *   'deleted' => Number of rows deleted from the table.
 *   Returns empty array if an error occurred along the way.
 */
function dirt_datatable_force_update_data_table() {
  // Need to loop over all data entry users in the system and process all of
  // their surveys.
  // Get array of data entry user IDs:
  $data_entry_user_rid = variable_get('dirt_data_entry_user_role');
  $query = db_select('users_roles', 'u');
  $query->condition('u.rid', intval($data_entry_user_rid));
  $query->fields('u', array('uid'));
  $uids = $query->execute()->fetchCol();

  $batch = array(
    'operations' => array(
      array('dirt_datatable_batch_update_data_table', array()),
    ),
    'finished' => 'dirt_datatable_batch_update_data_table_finished',
    'title' => t('Updating data table'),
    'init_message' => t('Beginning data table update'),
    'progress_message' => t('Updating data table'),
    'error_message' => t('An error occurred while attempting to update data table'),
    'file' => drupal_get_path('module', 'dirt_datatable') . '/includes/dirt_datatable.search_table_db_ops.inc',
  );

  batch_set($batch);
}


/**
 * Batch operation callback.
 */
function dirt_datatable_batch_update_data_table(&$context) {
  // Get data entry user role ID
  $rid = variable_get('dirt_data_entry_user_role');

  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['uids'] = array();
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = db_query('SELECT COUNT(DISTINCT uid) FROM {users_roles} WHERE rid = :rid', array(':rid' => $rid))->fetchField();
  }
 
  // Initialize counts if not yet done
  $counts = array('updated', 'inserted', 'deleted');
  foreach($counts as $count) {
    if (!isset($context['results'][$count])) $context['results'][$count] = 0;
  }

  // Initialize success to TRUE (may be set to FALSE at some point if update fails)
  if (!isset($context['results']['success'])) $context['results']['success'] = TRUE;

  // Process 5 users at a time
  $limit = 5;

  // Retrieve the next group of users.
  $query = db_select('users_roles', 'u');
  $query->condition('u.rid', intval($rid));
  $query->fields('u', array('uid'));
  $query->orderBy('u.uid');
  $query->range($context['sandbox']['progress'], $limit);
  $uids = $query->execute()->fetchCol();

  foreach($uids as $uid) {
    // Update data table for this user's surveys.
    $status = dirt_datatable_update_data_table_for_uid($uid);

    // Update counts
    if (!empty($status)) {
      foreach($counts as $count) {
        $context['results'][$count] += $status[$count];
      }
    }
    else {
      // Something went wrong
      $context['results']['success'] = FALSE;
    }

    // Update progress info
    $context['sandbox']['progress']++;
    $context['message'] = t('Processing @current of @total sites',
      array(
        '@current' => $context['sandbox']['progress'],
        '@total' => $context['sandbox']['max'],
      )
    );

    if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
      $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
    }
  }
}


/**
 * Batch finished callback.
 */
function dirt_datatable_batch_update_data_table_finished($success, $results, $operations) {
  if ($success && $results['success']) {
    $form_state['store']['success'] = TRUE;

    $inserted = $results['inserted'];
    $updated = $results['updated'];
    $deleted = $results['deleted'];

    $message = t('Successfully updated data table: inserted @insert rows, ' .
      'updated @updated rows, deleted @deleted rows.', array(
        '@insert' => $inserted,
        '@updated' => $updated,
        '@deleted' => $deleted,
      ));
    drupal_set_message($message);

    watchdog('dirt_datatable', 'DIRT Data Search Portal table updated in ' .
      'database: inserted %insert rows, updated %updated rows, deleted ' .
      '%deleted rows.', array(
        '%insert' => $inserted,
        '%updated' => $updated,
        '%deleted' => $deleted,
      ), WATCHDOG_INFO);
  }
  else {
    $message = t('An error occurred while updating the data table.');
    drupal_set_message($message, 'error');

    watchdog('dirt_datatable', 'Error while force updating DIRT Data ' .
      'Search Portal table.', array(), WATCHDOG_ERROR);
  }
}


/**
 * Update the data table with one user's surveys.
 *
 * Note: update count might be off if any survey has "other" selected as 
 * habitat type because this will update the table to set the habitat type as
 * entered by the user then a second update process for site desc surveys will
 * add back in "Other: " prefix for habitat type (required formatting for data
 * search table). This could be fixed in the future by adding the "Other: "
 * prefix to habitat type when the survey is saved, though would need to be
 * careful of hitting database storage limits if prefix + text >255 characters.
 *
 * @param[in] $uid User ID.
 *
 * @retval Array containing keys:
 *   'inserted' => Number of rows inserted into the table,
 *   'updated' => Number of rows updated in the table,
 *   'deleted' => Number of rows deleted from the table.
 *   Returns empty array if an error occurred along the way.
 */
function dirt_datatable_update_data_table_for_uid($uid) {
  // Initialize return array
  $status = array();

  // Initialize update counts
  $num_rows_inserted = 0;
  $num_rows_updated = 0;
  $num_rows_deleted = 0;

  // Initialize whether any errors found in updating database to FALSE;
  // this may get set to TRUE later as updates are attempted below.
  $error_detected = FALSE;

  // Begin manual update of table in database
  // These are the general steps:
  // 1) Query the database table dirt_datatable_table_data for the
  //    data already stored in the table.
  // 2) Query the Drupal tables to get all survey data in the system.
  // 3) Compare the two, and update the database table as needed so
  //    that they match.

  $types = dirt_get_survey_types();

  // Initialize array to store a data table row
  $row = array();

  // Set site id as uid
  $site_id = $uid;

  // Get all rows in data table for this user for each survey type
  // and all node IDs in the system for this user for each type
  $form_nids_in_table = array();
  $form_nids_in_system = array();
  $form_types = array();  // Associative array nid => content type

  // First loop over all survey types and get list of survey node IDs per type
  // in system and in data table for comparison
  foreach($types as $type) {
    $table_rows = dirt_datatable_get_rows_from_data_table($site_id, array('nid'), $type);
    if (!empty($table_rows)) {
      $nids_in_table = array_keys($table_rows);
      $form_nids_in_table = array_unique(array_merge($form_nids_in_table, $nids_in_table));
    }

    $survey_nids = _dirt_datatable_get_survey_nids($type, $site_id);
    foreach($survey_nids as $nid) {
      $form_types[$nid] = $type;
    }
    $form_nids_in_system = array_unique(array_merge($form_nids_in_system, $survey_nids));
  }

  // Compare $form_nids_in_table to $form_nids_in_system
  // For rows in both system and table, update
  // For rows in system but not in table, insert
  // For rows in table but not in system, delete

  // Update:
  $nids_to_process = array_intersect($form_nids_in_table, $form_nids_in_system);
  foreach($nids_to_process as $nid) {
    // Get column values to update based on form type
    $row = dirt_datatable_get_survey_columns_from_database($site_id, $nid, $form_types[$nid]);

    // Update columns
    if (!(dirt_datatable_update_row_in_data_table($row, $nid, $num_rows_updated)))
       $error_detected = TRUE;

    unset($row);
  }

  // Insert:
  $nids_to_process = array_diff($form_nids_in_system, $form_nids_in_table);
  foreach($nids_to_process as $nid) {
    // Get column values to update based on form type
    $row = dirt_datatable_get_survey_columns_from_database($site_id, $nid, $form_types[$nid]);

    // Insert columns
    $id = dirt_insert_into_database_table(DIRT_SEARCH_TABLE_DATA, $row, FALSE);

    // Must check if $id is NULL because here $id will be 0 upon success 
    // (no auto-increment field).
    if (is_null($id)) $error_detected = TRUE;
    else $num_rows_inserted++;

    unset($row);
  }

  // Delete:
  $nids_to_process = array_diff($form_nids_in_table, $form_nids_in_system);
  foreach($nids_to_process as $nid) {
    if (dirt_datatable_delete_row_from_data_table($nid))
      $num_rows_deleted++;
    else
      $error_detected = TRUE;
  }

  // Lastly, as a special case, populate all rows with the site description
  // (one time) variables (e.g., site address, etc.) as it makes it easier
  // to cross-reference vars at other collection frequencies with site
  // habitat, address, etc.

  if (!dirt_datatable_update_site_desc_info_for_site_id($site_id))
    $error_detected = TRUE;

  unset($nids_to_process);
  unset($form_types);
  unset($survey_nids);
  unset($form_nids_in_system);
  unset($form_nids_in_table);
  unset($table_rows);

  if (!$error_detected) {
    $status = array(
      'inserted' => $num_rows_inserted,
      'updated' => $num_rows_updated,
      'deleted' => $num_rows_deleted,
    );
  }

  return $status;
}


/**
 * Helper function that returns largest value in 'weight' column
 * of database table.
 *
 * @param[in] $table_name String database table name. Assumes this
 *  table has a column named 'weight'.
 *
 * @retval Integer largest value in 'weight' column, or
 *  0 if none found.
 */
function dirt_datatable_get_largest_weight_value($table_name) {
  $weight = 0; // Default to zero.

  if (db_table_exists($table_name) && db_field_exists($table_name, 'weight')) {
    $query = db_select($table_name, 't');
    $query->fields('t', array('weight'));
    $query->orderBy('t.weight', 'DESC');
    $result = $query->execute()->fetchAssoc();
    if ($result) $weight = $result['weight'];
  }

  return $weight;
}


/**
 * Helper function to set a value for a single column in a row.
 * Includes check if input field value is null or empty string
 * and if so sets column value to NULL.
 *
 * @param[in,out] $row Array where keys are column names and values
 *   are the field values.
 * @param[in] $column_name Column name to set.
 * @param[in] $column_value Column value to set.
 */
function _dirt_datatable_set_column_value(&$row, $column_name, $column_value) {
  // If column value is nonempty, set value, otherwise set NULL to be sure to clear out any existing values
  // that may need to be overwritten in the column
  if ( isset($column_value) && ($column_value !== '') ) {
    $row[$column_name] = $column_value;
  }
  else {
    $row[$column_name] = NULL;
  }
}


/**
 * Helper function to get select field labels from
 * key values that were returned from database query.
 *
 * @param[in] $keys String key value(s) returned from database query, with
 *  multiple values comma-separated within the string.
 * @param[in] $field_name String field machine name.
 *
 * @retval String label(s) corresponding to key(s), with multiple values
 *  comma-separated within the string.
 */
function _dirt_datatable_get_select_labels_from_keys($keys, $field_name) {
  $labels_arr = array();

  // Get key(s) in array
  $keys_arr = explode(', ', $keys);

  // Get field info and all label options
  $field = field_info_field($field_name);
  $allowed_values= list_allowed_values($field);
  foreach($keys_arr as $key) {
    if (array_key_exists($key, $allowed_values)) $labels_arr[] = $allowed_values[$key];
  }

  $labels = implode(', ', $labels_arr);

  return $labels;
}


/**
 * Helper function to format habitat type field value.
 * Adds "Other: " in front of user-entered habitat types.
 *
 * @param[in] $habitat Habitat type string value from database
 *  query, possibly NULL if not yet entered for a site.
 *
 * @retval String Habitat type formatted for data search table.
 */
function _dirt_datatable_format_habitat_field($habitat) {
  $formatted_habitat = $habitat;

  // Get drop-down habitat type options
  $habitat_type_options = dirt_get_habitat_type_options();

  if ($habitat) {
    if (!in_array($habitat, $habitat_type_options)) {
      // If not one of the drop-down options, indicate with
      // "Other: " prefix
      $formatted_habitat = 'Other: ' . $habitat;
    }
  }
  else {
    $formatted_habitat = 'Other: (none entered)';
  }

  return $formatted_habitat;
}


/**
 * Update data table after new survey submitted.
 *
 * @param[in] $nid Numeric Node ID of new survey.
 * @param[in] $site_id Site ID corresponding to survey.
 * @param[in] $type String of survey type (machine name).
 */
function _dirt_datatable_post_insert($nid, $site_id, $type) {
  // Get row to insert from node ID, site ID, and survey type
  $row = dirt_datatable_get_survey_columns_from_database($site_id, $nid, $type);

  $id = NULL;
  try {
    $id = dirt_insert_into_database_table(DIRT_SEARCH_TABLE_DATA, $row);
  }
  catch (Exception $e) {
    // Fail silently so participants don't see an error
  }

  // Must check if $id is NULL because here $id will be 0 upon success
  // (no auto-increment field).
  if (is_null($id)) {
    watchdog('dirt_datatable', 'Error inserting new row into data table for ' .
      'survey type %type with nid %nid for site id %site',
      array('%nid' => $nid, '%type' => $type, '%site' => $site_id), WATCHDOG_ERROR);
  }
  else {
     watchdog('dirt_datatable', 'Inserted new row into data table for survey ' .
       'type %type with nid %nid for site id %site',
       array('%nid' => $nid, '%type' => $type, '%site' => $site_id), WATCHDOG_INFO);
  }

  // Also need to update site description fields for surveys in table (either
  // for this row only if not a site desc survey, or all other rows if this
  // is a site desc survey).
  dirt_datatable_update_site_desc_info_for_site_id($site_id);

  // Check if this survey is a duplicate (same survey month, year, and type as
  // another survey in system submitted for same site ID).
  _dirt_datatable_check_for_duplicate_surveys($nid, $site_id, $type, $row['survey_year'], $row['survey_month']);
}


/**
 * Update data table after existing survey updated.
 *
 * @param[in] $nid Numeric Node ID of new survey.
 * @param[in] $site_id Site ID corresponding to survey.
 * @param[in] $type String of survey type (machine name).
 */
function _dirt_datatable_post_save($nid, $site_id, $type) {
  // Update survey row in table
  $row = dirt_datatable_get_survey_columns_from_database($site_id, $nid, $type);

  $num_rows_updated = 0;
  if (dirt_datatable_update_row_in_data_table($row, $nid, $num_rows_updated)) {
    watchdog('dirt_datatable', 'Updated %num rows in data table for survey type %type with nid %nid for site id %site',
      array('%num' => $num_rows_updated, '%nid' => $nid, '%type' => $type, '%site' => $site_id), WATCHDOG_INFO);
  }
  else {
    watchdog('dirt_datatable', 'Error updating survey in data table with type %type and nid %nid for site id %site',
      array('%nid' => $nid, '%type' => $type, '%site' => $site_id), WATCHDOG_ERROR);
  }

  // If survey being updated is not a site description survey, it's enough
  // to update the survey alone and that's it.
  // However if the survey being updated IS a site description survey, we need
  // to update all other rows in the table, too.
  if ($type === 'dirt_site_description_survey')
    dirt_datatable_update_site_desc_info_for_site_id($site_id);

  // Check if this survey is a duplicate (same survey month, year, and type as
  // another survey in system submitted for same site ID).
  _dirt_datatable_check_for_duplicate_surveys($nid, $site_id, $type, $row['survey_year'], $row['survey_month']);
}


/**
 * Delete survey from data table when survey deleted from system.
 *
 * @param[in] $nid Numeric Node ID of new survey.
 * @param[in] $site_id Site ID corresponding to survey.
 * @param[in] $type String of survey type (machine name).
 */
function _dirt_datatable_post_delete($nid, $site_id, $type) {
  // Delete row from table
  if (dirt_datatable_delete_row_from_data_table($nid)) {
    watchdog('dirt_datatable', 'Deleted row from data table corresponding to survey type %type with nid %nid for site id %site',
      array('%nid' => $nid, '%type' => $type, '%site' => $site_id), WATCHDOG_INFO);
  }
  else {
    watchdog('dirt_datatable', 'Error deleting row from data table corresponding to survey type %type with nid %nid for site id %site',
      array('%nid' => $nid, '%type' => $type, '%site' => $site_id), WATCHDOG_ERROR);
  }

  // If deleted survey was any type other than site description,
  // nothing more to do.
  // However, if deleted survey was the site description survey,
  // we need to clear out the site description fields in all other rows.
  if ($type === 'dirt_site_description_survey')
    dirt_datatable_update_site_desc_info_for_site_id($site_id);

  // No need to check for potential duplicates since deleting a survey cannot
  // introduce a duplicate survey into the system.
}


/**
 * Helper function that queries database for all nids of the input form $node_type by user $uid.
 *
 * @param[in] $type String of survey type as its machine name.
 * @param[in] $uid User ID (uid), also doubles as Site ID.
 *
 * @retval Array Array of node IDs (nids).
 */
function _dirt_datatable_get_survey_nids($type, $uid) {
  $survey_nids = array();

  // Set up database query:
  // Select on type node
  $query = db_select('node', 'n');
  // Get nid field
  $query->fields('n', array('nid'));
  // Restrict to form type
  $query->condition('n.type', $type);
  // Restrict to surveys by $uid
  $query->condition('n.uid', $uid);
  $query->leftJoin('field_data_field_dirt_date_of_survey', 'd', 'n.nid = d.entity_id');
  // Order by most recent date of survey
  $query->orderBy('field_dirt_date_of_survey_value', 'DESC');
  $survey_nids = $query->execute()->fetchCol();

  return $survey_nids;
}


/**
 * Checks for duplicate surveys matching site ID, type, and survey
 * date (year and month) given as input. If duplicate(s) found,
 * logs and sends e-mail notification if notifications enabled
 * on module admin config page.
 *
 * @param[in] $nid Node ID.
 * @param[in] $site_id Site ID.
 * @param[in] $type Survey type string.
 * @param[in] $survey_year Survey year in form YYYY.
 * @param[in] $survey_month Survey month in form MM.
 */
function _dirt_datatable_check_for_duplicate_surveys($nid, $site_id, $type, $survey_year, $survey_month) {
  // Cases where we don't need to check for duplicates:
  // 1) Survey type is submitted only once per site ID (enforced via DIRT Core module)
  // 2) Survey being added/modified is to be excluded from data search portal.

  // Default to include
  $include = 'include';

  // Get include/exclude value
  try {
    $node_wrapper = entity_metadata_wrapper('node', $nid);
    $include = $node_wrapper->field_dirt_datatable_in_portal->value();
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt_datatable', $e);
  }

  $is_one_time_survey = (dirt_get_collection_frequency_of_survey_type($type) == DIRT_ONE_TIME_COLLECTION);
  $is_excluded = ($include === 'exclude');

  if (!$is_one_time_survey && !$is_excluded) {
    // No need to check one-time surveys since by design they can't be submitted
    // more than once.

    $duplicates = dirt_datatable_get_duplicate_surveys_in_data_table($site_id, $type, $survey_year, $survey_month, $nid);

    if (!empty($duplicates)) {
      // There's already another survey in the system for this site ID, survey
      // year, survey month, and survey type.
      // Minimally, add a record of the duplicate to the site log.
      $type_name = dirt_get_name_of_survey_type($type);

      watchdog('dirt_datatable', 'Duplicate found: A survey of type %type ' .
        'collected in %month/%year already exists in the system for site ID ' .
        '%site.<br><br>This survey nid: %thisnid.<br><br>Duplicate nid(s): %dupnid.',
        array(
          '%type' => $type_name,
          '%month' => $survey_month,
          '%year' => $survey_year,
          '%site' => $site_id,
          '%thisnid' => $nid,
          '%dupnid' => implode(', ', $duplicates),
        ),
        WATCHDOG_WARNING);

      // If admins request to be notified of duplicates by e-mail, generate and
      // send e-mail notification.

      if (variable_get('dirt_datatable_config_send_duplicate_surveys_email', 0)) {
        $to = variable_get('dirt_datatable_config_notify_duplicate_emails');

        $email_subject = t('@site Duplicate Survey Entered',
          array('@site' => variable_get('site_name')));

        $email_body = array(
          t('A duplicate @type survey with survey date @month/@year was ' .
            'submitted for site ID @id.', array(
            '@type' => $type_name,
            '@month' => $survey_month, '@year' => $survey_year, '@id' => $site_id)
          ),
        );

        foreach($duplicates as $duplicate_nid) {
          $email_body[] = l(t('View the duplicate survey #@num in the system.',
            array('@num' => $duplicate_nid)), 'node/' . $duplicate_nid);
        }

        $email_body[] = l(t('View the survey just submitted (#@num).',
          array('@num' => $nid)), 'node/' . $nid);

        dirt_datatable_send_email_notification($to, $email_subject, $email_body, 'duplicate_survey');
      }
    }
  }
}


/**
 * Returns two letter abbreviation for survey type to display with
 * links to surveys in search table.
 *
 * @param[in] $survey_type String survey type machine name.
 *
 * @retval String Abbreviation for survey type.
 */
function _dirt_datatable_get_survey_type_abbr($survey_type) {
  $abbr = '';

  // Form abbreviation by taking first letter of survey type name
  // (minus the <module_name>_ prefix) and adding 'F' for 'form'.
  // Note that for the pre-installed survey types, this will result
  // in unique abbreviations. However if any survey types have been
  // added, there may be conflicts.

  $type_without_prefix = preg_replace('/^[a-zA-Z0-9]+_/', '', $survey_type);
  $first_letter = strtoupper($type_without_prefix[0]);
  $abbr = $first_letter . 'F';

  return $abbr;
}

