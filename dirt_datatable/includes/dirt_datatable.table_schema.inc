<?php

/**
 * @file
 * This file specifies the definitions for custom database tables created by this module.
 */

/**
 * Returns database table schema for this module
 */
function dirt_datatable_get_table_schema() {
  /**
   * This table stores information from data access request forms so that
   * site admins may review them later and decide to approve the account
   * or delete the request.
   */
  $schema[DIRT_DATA_ACCESS_REQUESTS_TABLE] = array(
    'description' => 'The table for data access account requests.',
    'fields' => array(
      'fid' => array(
        'description' => t('The primary identifier for the form (form ID).'),
        'type' => 'serial',
        'unsigned' => TRUE,
        'size' => 'small',
        'not null' => TRUE,
      ),
      'first_name' => array(
        'description' => t('First name'),
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'last_name' => array(
        'description' => t('Last name'),
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'email' => array(
        'description' => t('E-mail address'),
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'reason' => array(
        'description' => t('Reason for account request'),
        'type' => 'text',
        'size' => 'big',
        'not null' => FALSE,
      ),
    ),
    'primary key' => array('fid'),
    'indexes' => array(
      'fid' => array('fid'),
    ),
  );

  /**
   * This table stores information on the variable filter groups. The data
   * search portal will allow the user to filter the search table by variable
   * group, i.e., a set of variables collected within a survey type. The
   * variable group filter options will be based on column groups (specified
   * in the next table definition following this one). 
   */
  $schema[DIRT_SEARCH_TABLE_FILTER_GROUPS] = array(
    'description' => 'The table for data search portal variable filter groups.',
    'fields' => array(
      'fid' => array(
        'description' => 'The primary identifier for a filter group.',
        'type' => 'serial',
        'unsigned' => TRUE,
        'size' => 'small',
        'not null' => TRUE,
      ),
      'label' => array(
        'description' => 'The human-readable label for this filter group which will be displayed in the data search portal.',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'element_id' => array(
        'description' => 'The CSS id for this filter group (actual element id will be this value prefixed with "col-visibility-")',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'survey_type' => array(
        'description' => 'The survey content type machine name for all fields in this group.',
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'default' => '',
      ),
      'weight' => array(
        'description' => 'The weight of this filter group (determines filter order in the search filters section).',
        'type' => 'int',
        'size' => 'small',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array('fid'),
    'indexes' => array(
      'fid' => array('fid'),
    ),
  );

  /**
   * This table stores information on the data search portal table column
   * groups. Fields (table columns) will be assigned to one of these column
   * groups.
   * 
   * Rules for column groups:
   * 1. Every field must be assigned to a column group. Fields that are not
   *    assigned to a group will not be displayed in the search table.
   * 2. Every column group must contain fields from only one survey type,
   *    as it is assumed that all fields within a column group are 
   *    collected at the same frequency.
   * 3. (Optional) A column group may be assigned to a filter group. Any
   *    column group which is not assigned to a filter group will always
   *    be displayed in the search table, e.g., the common group fields.
   *    Assigning a column group to a filter group allows the user to
   *    search the table for variables of interest, e.g., soil
   *    respiration.
   *    Multiple column groups may be assigned to the same filter group,
   *    but only if the fields within the column groups are all of the 
   *    same survey type (due to the way the filter is implemented).
   */
  $schema[DIRT_SEARCH_TABLE_COLUMN_GROUPS] = array(
    'description' => 'The table for data search portal table column groups.',
    'fields' => array(
      'gid' => array(
        'description' => 'The primary identifier for a column group.',
        'type' => 'serial',
        'unsigned' => TRUE,
        'size' => 'small',
        'not null' => TRUE,
      ),
      'filter_group' => array(
        'description' => 'Reference to filter group ID (fid) that this column group belongs to.',
        'type' => 'int',
        'size' => 'small',
        'unsigned' => TRUE,
        'default' => 0,
      ),
      'label' => array(
        'description' => 'The human-readable label for this column group, as it appears in the table.',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'class' => array(
        'description' => 'The CSS class for this column group (multiple classes separated by space).',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'survey_type' => array(
        'description' => 'The survey content type machine name for all fields in this group, or "common" if they belong to all surveys.',
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'default' => '',
      ),
      'weight' => array(
        'description' => 'The weight of this column group (determines group order in the table).',
        'type' => 'int',
        'size' => 'small',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array('gid'),
    'indexes' => array(
      'gid' => array('gid'),
    ),
    'foreign keys' => array(
      'filter_group' => array(
        'table' => DIRT_SEARCH_TABLE_FILTER_GROUPS,
        'columns' => array('filter_group' => 'fid'),
      ),
    ),
  );

  /**
   * This table stores information on the columns of the data search portal
   * table. Note that it does not store the table data itself, but rather
   * metadata about the table columns (e.g., which column group the column
   * belongs to, which database field does it correspond to, whether it is
   * visible to admins only, the order to display the columns in the table,
   * etc.)
   */
  $schema[DIRT_SEARCH_TABLE_COLUMNS] = array(
    'description' => 'The table for data search portal table column information.',
    'fields' => array(
      'cid' => array(
        'description' => 'The primary identifier for a table column.',
        'type' => 'serial',
        'unsigned' => TRUE,
        'size' => 'small',
        'not null' => TRUE,
      ),
      'colgroup' => array(
        'description' => 'Reference to column group ID (gid) that this column belongs to.',
        'type' => 'int',
        'size' => 'small',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'column_name' => array(
        'description' => 'The name of the column in the dirt_datatable_table_data table.',
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
        'default' => '',
      ),
      'label' => array(
        'description' => 'The human-readable label for this column header, as it appears in the table.',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'field_name' => array(
        'description' => 'The machine name of the field that provides the data for this column.',
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
        'default' => '',
      ),
      // Storing the field label for convenience because it's needed for
      // field collection fields, where field label != column label, and it's
      // a lot easier to have it here than use field_info_instance() each time.
      'field_label' => array(
        'description' => 'The human-readable label for the field this column corresponds to (usually same as column label).',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'class' => array(
        'description' => 'The CSS class for this column (multiple classes separated by space).',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      ),
      'survey_type' => array(
        'description' => 'The survey content type machine name for this field, or "common" if it belongs to all surveys.',
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'default' => '',
      ),
      'weight' => array(
        'description' => 'The weight of this column within its column group (if applicable).',
        'type' => 'int',
        'size' => 'small',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'visible' => array(
        'description' => 'Boolean whether this column is visible in the table; 1 = visible, 0 = not visible.',
        'type' => 'int',
        'size' => 'tiny',
        'not null' => TRUE,
        'default' => 1,
      ),
      /**
       * The following field denotes whether this table column is also a column
       * in the dirt_datatable_table_data table (defined below) or its value is
       * derived from other columns in some way.
       * For example, "nids" is a table column that collects all nids when the
       * table rows are grouped together by survey and month and date. It is
       * not in the dirt_datatable_table_data table itself but derived from the
       * nid field, hence nids is denoted a non-primary field.
       *
       * primary_field = 1 (TRUE): this column is present in the table 
       *   dirt_datatable_table_data.
       * primary_field = 0 (FALSE): this column is not present in the table 
       *   dirt_datatable_table_data.
       */
      'primary_field' => array(
        'description' => 'Boolean whether this column is primary, i.e., is also a column in dirt_datatable_table_data.',
        'type' => 'int',
        'size' => 'tiny',
        'not null' => TRUE,
        'default' => 1,
      ),
      /**
       * The following field helps to group columns together based on database
       * query type (prior to performing the query to populate the table).
       *
       * The types are:
       * Standard: The value for this column will come straight from the
       *   database query on the corresponding field.
       * Special: Additional formatting or other manipulation required while or
       *  after performing the database query, before the final value is stored
       *  in the table column (e.g., deriving the survey month from the date of
       *  survey).
       *  Most survey fields will be "standard"; the "special" fields are
       *    primarily the fields belonging to the common column group that
       *    are handled separately.
       * Float: The value for this column is a float, so additional formatting
       *   will be added to the database query (e.g., rounding to two
       *   decimal places).
       */
      'query_type' => array(
        'description' => 'Query type (special, standard, float).',
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
        'default' => '',
      ),
      /**
       * Similar to the fields above, the following field helps to group columns
       * based on the field type (prior to performing the query to populate the
       * table). For instance, if the field is single-valued, only one result
       * will be pulled from the database.
       * In the case case of field collection fields, additional info is needed
       * to perform the database query: fc_name (field collection machine name),
       * delta (delta to query), max_delta (overall number of values expected).
       */
      'field_type' => array(
        'description' => 'Field type (e.g., single_valued, multi_valued, field_collection).',
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
        'default' => '',
      ),
      'field_select' => array(
        'description' => 'Boolean whether this field is a select field. Will need to get field label, not key, for table.',
        'type' => 'int',
        'size' => 'tiny',
        'not null' => TRUE,
        'default' => 0,
      ),
      'fc_name' => array(
        'description' => 'Field collection name (if field_type == field_collection).',
        'type' => 'varchar',
        'length' => 32,
      ),
      'delta' => array(
        'description' => 'Field collection field delta being queried for this column (if field_type == field_collection).',
        'type' => 'int',
        'size' => 'tiny',
      ),
      'max_delta' => array(
        'description' => 'Field collection cardinality, i.e., max value possible for delta (if field_type == field_collection).',
        'type' => 'int',
        'size' => 'tiny',
        'unsigned' => TRUE,
      ),
      'admin_only' => array(
        'description' => 'Boolean whether this table column is visible to only administrators.',
        'type' => 'int',
        'size' => 'tiny',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'foreign keys' => array(
      'colgroup' => array(
        'table' => DIRT_SEARCH_TABLE_COLUMNS,
        'columns' => array('colgroup' => 'gid'),
      ),
    ),
    'primary key' => array('cid'),
  );

  /**
   * This table stores the data search table data. Each row represents a survey
   * node in the system.
   * When the data search portal page is loaded, the table data is pulled from
   * this table with a single database query.
   * This table is dynamically updated every time a survey is created, edited,
   * or deleted, so that the most up-to-date version is always loaded upon page
   * refresh.
   * While this goes against the spirit of an SQL database, it speeds up the
   * page load time considerably to pull all of the information from one table,
   * vs. joining in 100+ different tables to get the survey values for all
   * surveys in the system upon page load.
   *
   * For now, define the columns common to all surveys (e.g., survey node ID,
   * site ID, survey date, etc.)
   * Later we'll fill in the rest of the columns based on which fields are
   * active.
   */
  $schema[DIRT_SEARCH_TABLE_DATA] = array(
    'description' => 'The table containing the data search portal table data for all surveys in the system.',
    'fields' => array(
      'nid' => array(
        'description' => 'The Node ID of the survey.',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'small',
        'not null' => TRUE,
      ),
      'site_id' => array(
        'description' => 'The site ID of the survey.',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'small',
        'not null' => TRUE,
      ),
      'username' => array(
        'description' => 'The username of the survey author.',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      // For the following field,
      // 0 = public,
      // 1 = semi-public,
      // 2 = private.
      'dirt_user_sharing_perms' => array(
        'description' => 'The sharing permissions that the user has selected, stored as an integer between 0-2.',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'tiny',
        'not null' => TRUE,
      ),
      'dirt_user_collection_state' => array(
        'description' => 'The collection status of the user (active vs. closed).',
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'default' => 'Active',
      ),
      'survey_type' => array(
        'description' => 'The survey content type machine name.',
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'default' => '',
      ),
      'survey_year' => array(
        'description' => 'The survey year in the format YYYY.',
        'mysql_type' => 'YEAR(4)',
        'not null' => TRUE,
      ),
      'survey_month' => array(
        'description' => 'The survey month (as a number 1-12).',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'tiny',
        'not null' => TRUE,
      ),
      'survey_month_abbr' => array(
        'description' => 'The survey month (as an abbreviation, e.g., Jan).',
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
        'default' => '',
      ),
      'include' => array(
        'description' => 'Whether to include/exclude survey from data search table. Defaults to include.',
        'type' => 'varchar',
        'length' => 32,
        'not null' => TRUE,
        'default' => 'include',
      ),
    ),
    'primary key' => array('nid'),
    'indexes' => array(
      'nid' => array('nid'),
      'survey_year' => array('survey_year'),
      'survey_month' => array('survey_month'),
      'dirt_user_sharing_perms' => array('dirt_user_sharing_perms'),
      'survey_type' => array('survey_type'),
    ),
  );

  // Get the rest of the schema dynamically from survey fields
  $schema_info = array();

  // Some special cases:

  // Add first and last name fields if present
  $fields = array('field_dirt_user_first_name', 'field_dirt_user_last_name');
  foreach($fields as $field_name) {
    $schema_info = dirt_datatable_get_schema_info_from_field($field_name);
    dirt_datatable_add_column_to_data_table_schema($schema_info, $schema);
  }

  // Add the habitat type fields
  $fields = array('field_dirt_site_habitat_type', 'field_dirt_site_type_of_crop', 'field_dirt_site_cropping_system');
  foreach($fields as $field_name) {
    $schema_info = dirt_datatable_get_schema_info_from_field($field_name);
    dirt_datatable_add_column_to_data_table_schema($schema_info, $schema);
  }

  // Handle the geographic coordinates field collection, if present
  // Although 5 replicates are collected, only the first will be displayed in the table
  $fields = array('field_dirt_site_geo_coords_lat', 'field_dirt_site_geo_coords_lon');
  foreach($fields as $fc_field_name) {
    $schema_info = dirt_datatable_get_schema_info_from_field($fc_field_name);
    dirt_datatable_add_column_to_data_table_schema($schema_info, $schema);
  }

  // Add the rest of the survey fields

  $columns = dirt_datatable_get_column_metadata_from_active_node_fields(TRUE); 
  foreach($columns as $column_name => $schema_info) {
    dirt_datatable_add_column_to_data_table_schema($schema_info, $schema);
  }

  return $schema;
}


/**
 * Returns an array of all active fields attached to survey types.
 * These fields are potential columns in the data search table.
 *
 * Note that the assumption is that each field (other than special
 * cases, such as date of survey) is assigned to only one survey type.
 * Fields attached to more than one survey must be handled separately.
 *
 * Also, group information survey fields are excluded, as they are not
 * shown in the data search table.
 *
 * @retval Array of field names.
 */
function dirt_datatable_get_table_survey_fields() {
  // Get survey types
  $survey_types = dirt_get_survey_types();

  // Exclude fields from the group info survey - not relevant to data collection
  $survey_types = array_diff($survey_types, array('dirt_group_information_survey'));

  $survey_fields = array();

  // Get info on all currently active fields
  $fields = field_info_field_map();

  // Get the field names and survey type attached to.
  // Since the assumption is one field per type, can just look at the first item in the node array.
  // (Other fields such as date of survey are handled separately.)
  foreach ($fields as $field_name => $field_info) {
    $is_survey_type = (array_key_exists('node', $field_info['bundles']) && in_array($field_info['bundles']['node'][0], $survey_types));
    if ($is_survey_type) {
      // Store names in keys too for easy lookup
      $survey_fields[$field_name] = $field_name;
    }
  }

  // Remove some fields that are special cases to be handled separately
  // or are to be left out of the table completely:

  // Special cases: date of survey, geo coords, include/exclude survey in table,
  // and habitat type fields.
  unset($survey_fields['field_dirt_date_of_survey']);
  unset($survey_fields['field_dirt_site_geo_coords']);
  unset($survey_fields['field_dirt_datatable_in_portal']);
  unset($survey_fields['field_dirt_site_habitat_type']);
  unset($survey_fields['field_dirt_site_type_of_crop']);
  unset($survey_fields['field_dirt_site_cropping_system']);

  // Exclude from table: land management updates, habitat photos, drawing scale,
  // N, P, K drop-downs (we'll use the separate value/category/ppm fields instead),
  // pH (we'll use separate pH number/category fields), and active carbon
  // (we'll use separate value/soil quality fields)
  unset($survey_fields['field_dirt_site_manage_updates']);
  unset($survey_fields['field_dirt_site_habitat_photos']);
  unset($survey_fields['field_dirt_site_drawing_scale']);
  unset($survey_fields['field_dirt_soil_n']);
  unset($survey_fields['field_dirt_soil_p']);
  unset($survey_fields['field_dirt_soil_k']);
  unset($survey_fields['field_dirt_soil_ph']);
  unset($survey_fields['field_dirt_soil_active_carbon']);

  return $survey_fields;
}


/**
 * Returns schema information for a field.
 *
 * @param[in] $field_name String field machine name.
 * @param[in] $delta *Optional* Integer representing delta in case of field
 *   collection fields (optional, defaults to NULL).
 *
 * @retval Array of schema info.
 */
function dirt_datatable_get_schema_info_from_field($field_name, $delta = NULL) {
  // Initialize return
  $schema_info = array();

  // Schema info includes: column name, description, and sql storage info (integer, varchar, etc.).
  // Can get this info from field info:
  $field = field_info_field($field_name);

  if ($field) {
    // Get column name from field name
    $column_name = _dirt_datatable_get_column_name_from_field_name($field_name, $delta);
    $schema_info['column_name'] = $column_name;

    // Get description from column name
    $schema_info['description'] = ucfirst(str_replace('_', ' ', $column_name)) . '.';

    // Get sql info from field info
    $schema_info['sql_info'] = $field['columns']['value'];

    if ($field['cardinality'] != 1) {
      // Special case of multi-valued fields: since they are concatenated and
      // comma-separated, may need to increase max length.
      // For built-in fields, have suggested lengths that are a bit more
      // optimized.

      $length = dirt_datatable_get_suggested_length_for_field($field_name);

      // In the general case, use cardinality as multiplier (will end up being
      // an overestimate in most cases). If cardinality unlimited, arbitrarily
      // choose 10 as multiplier.
      if (empty($length)) {
        $card = ($field['cardinality'] > 1) ? $field['cardinality'] : 10;
        $length = ($field['columns']['value']['length'] + 2) * $card;
      }

      $schema_info['sql_info']['length'] = $length;
    }
  }

  return $schema_info;
}


/**
 * Returns information about the field given as input, as an output array,
 * specifying fields for the column metadata database table and the
 * data search table database schema.
 *
 * @param[in] $entity_type String entity type for field instance.
 * @param[in] $field_name String field machine name to add as table column.
 * @param[in] $bundle_name String bundle name for field instance.
 * @param[in] $survey_type *Optional* If $entity_type=='field_collection_item',
 *   this field specifies the overall survey type the field belongs to
 *   (optional, defaults to NULL).
 * @param[in] $delta *Optional* Integer representing delta in case of field
 *   collection fields (optional, defaults to NULL).
 * @param[in] $max_delta *Optional* Integer representing max delta value, i.e.,
 *   cardinality of overall field collection (optional, defaults to NULL).
 *
 * @retval Array of column info.
 */
function dirt_datatable_get_column_info_from_field($entity_type, $field_name, $bundle_name, $survey_type = NULL, $delta = NULL, $max_delta = NULL) {
  // Initialize return
  $column_info = array();

  // Get field info
  $field = field_info_field($field_name);

  if ($field) {
    // Set column name and field name
    $column_name = _dirt_datatable_get_column_name_from_field_name($field_name, $delta);
    $column_info['column_name'] = $column_name;
    $column_info['field_name'] = $field_name;

    // Get class name from column name
    $column_info['class'] = str_replace('_', '-', $column_name);

    // Get suggested column group
    $column_info['colgroup'] = dirt_datatable_get_suggested_group_for_field($field_name);

    // Get admin-only status, if set.
    // Admin-only to view fields and private fields should be set admin-only in the data table.
    if ($field['settings']['dirt_admin_only']['view'] || $field['settings']['dirt_private_field']) {
      $column_info['admin_only'] = 1;
      $column_info['class'] .= ' admin-only';
    }
    else {
      $column_info['admin_only'] = 0;
    }

    // Query type default to standard
    $column_info['query_type'] = 'standard';
    // If float, set query type to float
    if ($field['type'] == 'number_float') $column_info['query_type'] = 'float';

    // Set field type
    $field_type = '';
    if ($entity_type == 'field_collection_item') {
      // Set type as field collection
      $field_type = 'field_collection';
      // Set field collection name and delta
      // (re-index beginning at 0)
      $column_info['fc_name'] = $bundle_name;
      if ($delta > 0) $column_info['delta'] = $delta - 1;
      else $column_info['delta'] = 0;
      if ($max_delta > 0) $column_info['max_delta'] = $max_delta - 1;
      else $column_info['max_delta'] = 0;
    }
    else {
      // Single vs. multi-valued: depends on cardinality
      $field_type = ($field['cardinality'] == 1) ? 'single_valued' : 'multi_valued';
    }
    $column_info['field_type'] = $field_type;

    // Note whether field is select or not - type begins with "list_"
    $select_field_prefix = 'list_';
    if (substr($field['type'], 0, strlen($select_field_prefix)) == $select_field_prefix) {
      $column_info['field_select'] = 1;
    }

    // Set the survey type
    // If $entity_type is node, survey type will be bundle name
    // But if $entity_type is field_collection_item, we need to get the type for $bundle_name
    // Finally, user fields are considered 'common' survey type
    $type = '';
    if ($entity_type == 'node') {
      $type = $bundle_name;
    }
    elseif ($entity_type == 'user') {
      $type = 'common';
    }
    elseif ($entity_type == 'field_collection_item') {
      $type = $survey_type;
    }
    $column_info['survey_type'] = $type;

    $instance = field_info_instance($entity_type, $field_name, $bundle_name);

    // Set the label and field label
    // Default to the field instance label itself
    $column_info['label'] = $instance['label'];
    $column_info['field_label'] = $instance['label'];

    // Some special cases for column labels:
    // If delta present, set label to indicate Replicate # according to field collection
    // Also for bulk density fields, add in the letters and equations on the next line
    // of the label
    if (isset($delta)) {
      $column_info['label'] = dirt_datatable_get_suggested_label($column_info['label'], $field_name, $bundle_name, $delta);
    }

    unset($instance);
  }

  unset($field);

  return $column_info;
}


/**
 * This function returns column metadata information for every active
 * survey type field (i.e., field belonging to a node type that is 
 * designated as a survey type) that is a supported field type.
 *
 * @sa dirt_datatable_get_column_metadata_from_active_node_field()
 *
 * @param[in] $schema_only *Optional* Whether to return schema info only, or
 *   return all column metadata fields.
 *
 * @retval Array of column information keyed by column name. If $schema_only is
 *   TRUE, each subarray contains schema info. Otherwise the subarrays contain
 *   column metadata fields.
 */
function dirt_datatable_get_column_metadata_from_active_node_fields($schema_only = FALSE) {
  $all_columns = array();

  $fields = dirt_datatable_get_table_survey_fields();

  foreach($fields as $field_name) {
    $columns = dirt_datatable_get_column_metadata_from_active_node_field($field_name, $schema_only);
    if (!empty($columns)) {
      $all_columns += $columns;
    }
  }

  return $all_columns;
}


/**
 * Returns column metadata information for an active survey type field. This
 * information is used to set/update the table schema for the data table and
 * populate the table column metadata table.
 *
 * In the case of field collection fields, multiple columns may be returned,
 * each keyed by their column name (to handle all of the fields belonging to
 * the field collection).
 *
 * This function only handles supported field types. Furthermore, it is assumed
 * that each survey field is only attached to one survey type (bundle). Any
 * fields attached to more than one survey (e.g., date of survey field) must be
 * handled as special cases.
 *
 * @param[in] $field_name Field name string.
 * @param[in] $schema_only Whether to return schema info only, or return
 *   all column metadata fields.
 * 
 * @retval Array of column information keyed by column name. If $schema_only is TRUE,
 *   this array contains schema info. Otherwise it contains column metadata fields.
 *   Returns an empty array if no information could be found or the field type
 *   is not supported.
 */
function _dirt_datatable_get_column_metadata_from_active_node_field($field_name, $schema_only) {
  $columns = array();

  // Get field info
  $field_info = field_info_field($field_name);

  // If no info found, exit early.
  if (empty($field_info)) return $columns;

  // Check if field attached to node entity. If not, exit early.
  if (!array_key_exists('node', $field_info['bundles'])) return $columns;

  // Next, verify that the field is attached to a survey type node.
  // Here we only look at the first item in the array because it is
  // assumed that each survey variable is only collected in one
  // survey type (special cases are handled separately).
  // If not survey type, exit early.
  $node_type = $field_info['bundles']['node'][0];
  if (!dirt_node_type_is_survey_type($node_type)) return $columns;

  // Next, check that the field is a supported type. If not, exit early.
  $field_type = $field_info['type'];
  if (!dirt_datatable_field_type_is_supported_type($field_type)) return $columns;

  // Handle node entity field. If not a field collection, simply 
  // return the column information from the field instance.
  // Field collections require additional work to split the field
  // collection fields into multiple columns.

  if ($field_type !== 'field_collection') {
    if ($schema_only)
      $column_info = dirt_datatable_get_schema_info_from_field($field_name);
    else
      $column_info = dirt_datatable_get_column_info_from_field('node', $field_name, $node_type);

    $column_name = $column_info['column_name'];
    $columns[$column_name] = $column_info;
    return $columns;
  }
  else {
    // Need to get all of the field collection fields.
    // Normally avoid calling field_info_instances() but it's necessary here.
    $fc_fields = field_info_instances('field_collection_item', $field_name);
      
    // Also need the overall cardinality of the field collection, which we can
    // get from field_info_field().
    $fc_bundle_info = field_info_field($field_name);
    $cardinality = $fc_bundle_info['cardinality'];
    unset($fc_bundle_info);

    // We begin with the first field in the field collection.
    $first_field = TRUE;

    foreach ($fc_fields as $fc_field_name => $fc_info) {
      // Skip first field collection field since that's assumed to be the
      // replicate number, layer number, etc.
      if (!($first_field)) {
        if ($cardinality == 1) {
          // If cardinality is 1, can treat each field collection field like a
          // single column.
          if ($schema_only)
            $column_info = dirt_datatable_get_schema_info_from_field($fc_field_name);
          else
            $column_info = dirt_datatable_get_column_info_from_field('field_collection_item', $fc_field_name, $field_name, $node_type);

          $column_name = $column_info['column_name'];
          $columns[$column_name] = $column_info;
        }
        else {
          // Otherwise we need to add $cardinality number of columns for each
          // field collection field, counting them off so that the column
          // names will be unique.
          for ($idx = 1; $idx <= $cardinality; ++$idx) {
            if ($schema_only)
              $column_info = dirt_datatable_get_schema_info_from_field($fc_field_name, $idx);
            else
              $column_info = dirt_datatable_get_column_info_from_field('field_collection_item', $fc_field_name, $field_name, $node_type, $idx, $cardinality);

            $column_name = $column_info['column_name'];
            $columns[$column_name] = $column_info;
          } // end for
        } // end else
      } // end if

      $first_field = FALSE;
    } // end foreach

    unset($fc_fields);
    unset($cardinality);
    unset($first_field);
  } // end if

  return $columns;
}


/**
 * Adds a field as a column in the data search table schema by examining
 * its field info and instance info.
 *
 * Assumes DIRT_SEARCH_TABLE_DATA already exists as a key in $schema and
 * the key $schema[DIRT_SEARCH_TABLE_DATA]['fields'] also exists before
 * this function is called.
 *
 * @param[in] $schema_info Array containing column name, description, and sql
 *   data for table schema
 * @param[in,out] $schema Array representing table schema.
 */
function _dirt_datatable_add_column_to_data_table_schema($schema_info, &$schema) {
  if (!empty($schema_info)) {
    // Get column name
    $column_name = $schema_info['column_name'];

    // Get description
    $schema[DIRT_SEARCH_TABLE_DATA]['fields'][$column_name] = array(
      'description' => $schema_info['description'],
    );

    // Set the column format
    $schema[DIRT_SEARCH_TABLE_DATA]['fields'][$column_name] = array_merge($schema[DIRT_SEARCH_TABLE_DATA]['fields'][$column_name], $schema_info['sql_info']);
  }
}


//------------------------------------------------------------------------------
// The following functions provide default assignments for column groups, filter
// groups, admin-only status, etc. for DIRT Core survey fields.
//
// They assume that very little or no changes were made to the fields that are
// installed with the DIRT Core module.
//------------------------------------------------------------------------------/

/**
 * Returns suggested length for database storage.
 *
 * @param[in] $field_name String field machine name.
 *
 * @retval Integer suggested length for field name, or 0 if no suggested length
 *   found.
 */
function dirt_datatable_get_suggested_length_for_field($field_name) {
  // Of the default DIRT fields, only two require more than length 255;
  // going up to the next power of two minus one is sufficient.

  switch($field_name) {
    case 'field_dirt_site_present_manage':
      return 511;

    case 'field_dirt_site_past_manage':
      return 511;
  }

  // Otherwise, nothing to suggest, so return empty value.

  return 0; 
}


/**
 * Returns suggested label for cases where the table column label differs from
 * the field instance label (e.g., adding replicate numbers or hour numbers).
 *
 * @param[in] $label String the field instance label that column label is based
 *   off of.
 * @param[in] $field_name String field machine name.
 * @param[in] $bundle_name String bundle machine name.
 * @param[in] $delta Integer delta.
 * 
 * @retval String suggested label.
 */
function dirt_datatable_get_suggested_label($label, $field_name, $bundle_name, $delta) {
  $col_label = '';

  switch ($bundle_name) {
    case 'field_dirt_site_layer_data':
      $col_label = t('Layer ') . $delta . ' ' .  $label;
      break;

    case 'field_dirt_color_number_by_hour':
      $hour = $delta-1;
      if ($delta == 7) $hour = '10';
      elseif ($delta == 8) $hour = '24';
      $col_label = $label . ' ' . $hour . 'h';
      break;

    case 'field_dirt_sample_before_drying':
      if ($field_name == 'field_dirt_weight_wet_soil_bag') $col_label = $label . '<br>A';
      elseif ($field_name == 'field_dirt_weight_bag') $col_label = $label . '<br>B';
      elseif ($field_name == 'field_dirt_weight_wet_soil') $col_label = $label . '<br>C = A - B';
      break;

    case 'field_dirt_sample_after_drying':
      if ($field_name == 'field_dirt_weight_dry_soil_bowl') $col_label = $label . '<br>D';
      elseif ($field_name == 'field_dirt_weight_bowl') $col_label = $label . '<br>E';
      elseif ($field_name == 'field_dirt_weight_dry_soil') $col_label = $label . '<br>F = D - E';
      break;

    case 'field_dirt_bulk_density_calcs':
      if ($field_name == 'field_dirt_volume_soil_sample') $col_label = $label . '<br>G';
      elseif ($field_name == 'field_dirt_soil_bulk_density') $col_label = $label . '<br>H = F / G';
      elseif ($field_name == 'field_dirt_water_gravimetric') $col_label = $label . '<br>I = [(C - F) / F] x 100';
      elseif ($field_name == 'field_dirt_water_volumetric') $col_label = $label . '<br>J = I x H';
      elseif ($field_name == 'field_dirt_total_soil_porosity') $col_label = $label . '<br>K = [1 - (H/2.65 g/cm<sup>3</sup>)] x 100';
      elseif ($field_name == 'field_dirt_water_filled_pores') $col_label = $label . '<br>L = J/K x 100';
      break;

    default:
      $col_label = t('Replicate ') . $delta . '<br>' . $label;
      break;
  }

  return $col_label;
}


/**
 * Returns suggested column group for input field; assumes few (if any)
 * changes have been made to the pre-installed DIRT fields. Group columns
 * may be altered later via the admin interface.
 *
 * @param[in] $field_name Field name (machine name).
 *
 * @retval Column group number (gid).
 */
function dirt_datatable_get_suggested_group_for_field($field_name) {
  $short_name = '';

  $common_fields = array(
    'field_dirt_user_first_name',
    'field_dirt_user_last_name',
    'field_dirt_user_collection_state',
    'field_dirt_user_sharing_perms',
    'field_dirt_site_habitat_type',
    'field_dirt_site_type_of_crop',
    'field_dirt_site_cropping_system',
  );

  $site_address_fields = array(
    'field_dirt_site_address',
    'field_dirt_site_city',
    'field_dirt_site_county',
    'field_dirt_site_state',
    'field_dirt_site_zip_code',
  );

  $geo_coords_fields = array(
    'field_dirt_site_geo_coords_lat',
    'field_dirt_site_geo_coords_lon',
  );

  $site_desc_fields = array(
    'field_dirt_site_mean_annual_temp',
    'field_dirt_site_annual_precip',
    'field_dirt_site_elevation',
    'field_dirt_site_slope',
    'field_dirt_site_aspect',
    'field_dirt_site_map_unit_name',
    'field_dirt_site_map_unit_symbol',
    'field_dirt_site_landscape_posit',
    'field_dirt_site_topography',
    'field_dirt_site_present_manage',
    'field_dirt_site_past_manage',
  );

  $soil_core_fields = array(
    'field_dirt_site_layer_texture',
    'field_dirt_site_layer_color',
  );

  $temp_cloud_fields = array(
    'field_dirt_air_temp',
    'field_dirt_soil_temp_surface',
    'field_dirt_soil_temp_5cm',
    'field_dirt_soil_temp_10cm',
    'field_dirt_avg_soil_temp_5cm',
    'field_dirt_cloud_cover',
  );

  $soil_resp_fields = array(
    'field_dirt_soil_resp_hour',
    'field_dirt_soil_resp_time',
    'field_dirt_soil_resp_color',
    'field_dirt_co2c_room_temp_range',
    'field_dirt_co2c_room_temp_lower',
    'field_dirt_co2c_room_temp_upper',
    'field_dirt_co2c_room_temp_avg',
    'field_dirt_co2c_field_temp_range',
    'field_dirt_co2c_field_temp_lower',
    'field_dirt_co2c_field_temp_upper',
    'field_dirt_co2c_field_temp_avg',
  );

  $bulk_density_fields = array(
    'field_dirt_weight_wet_soil_bag',
    'field_dirt_weight_bag',
    'field_dirt_weight_wet_soil',
    'field_dirt_weight_dry_soil_bowl',
    'field_dirt_weight_bowl',
    'field_dirt_weight_dry_soil',
    'field_dirt_volume_soil_sample',
    'field_dirt_soil_bulk_density',
    'field_dirt_water_gravimetric',
    'field_dirt_water_volumetric',
    'field_dirt_total_soil_porosity',
    'field_dirt_water_filled_pores',
  );

  $chem_props_fields = array(
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
    'field_dirt_soil_organic_matter',
  );

  // Check which group field belongs to.
  // If none of the above, field belongs to "empty" group.
  if (in_array($field_name, $common_fields)) { $short_name = 'common'; }
  elseif (in_array($field_name, $site_address_fields)) { $short_name = 'site_address'; }
  elseif (in_array($field_name, $geo_coords_fields)) { $short_name = 'geo_coords'; }
  elseif (in_array($field_name, $site_desc_fields)) { $short_name = 'site_desc'; }
  elseif (in_array($field_name, $soil_core_fields)) { $short_name = 'soil_core'; }
  elseif (in_array($field_name, $temp_cloud_fields)) { $short_name = 'temp_cloud'; }
  elseif (in_array($field_name, $soil_resp_fields)) { $short_name = 'soil_resp'; }
  elseif (in_array($field_name, $bulk_density_fields)) { $short_name = 'bulk_density'; }
  elseif (in_array($field_name, $chem_props_fields)) { $short_name = 'chem_props'; }
  else { $short_name = 'empty'; }

  unset($common_fields);
  unset($site_address_fields);  
  unset($geo_coords_fields);
  unset($site_desc_fields);
  unset($soil_core_fields);
  unset($temp_cloud_fields);
  unset($soil_resp_fields);
  unset($bulk_density_fields);
  unset($chem_props_fields);

  // Look up group ID (gid) by short name
  $gid = variable_get('dirt_datatable_colgroup_gid_' . $short_name, 0);

  return $gid;
}


/**
 * Returns suggested variable filter groups for DIRT project; assumes
 * few (if any) changes have been made to the pre-installed DIRT
 * fields.
 *
 * @retval Array of filter group info.
 */
function dirt_datatable_get_suggested_filter_groups() {
  return array(
    0 => array(
      'label' => t('Site Description'),
      'element_id' => 'site-desc',
      'short_name' => 'site_desc',
      'survey_type' => 'dirt_site_description_survey',
    ),
    1 => array(
      'label' => t('Soil Core Variables Data'),
      'element_id' => 'soil-core',
      'short_name' => 'soil_core',
      'survey_type' => 'dirt_site_description_survey',
    ),
    2 => array(
      'label' => t('Temperature and Cloud Cover'),
      'element_id' => 'temp-cloud',
      'short_name' => 'temp_cloud',
      'survey_type' => 'dirt_monthly_data_survey',
    ),
    3 => array(
      'label' => t('Soil Respiration'),
      'element_id' => 'soil-resp',
      'short_name' => 'soil_resp',
      'survey_type' => 'dirt_monthly_data_survey',
    ),
    4 => array(
      'label' => t('Soil Water Content'),
      'element_id' => 'bulk-density',
      'short_name' => 'bulk_density',
      'survey_type' => 'dirt_monthly_data_survey',
    ),
    5 => array(
      'label' => t('Chemical Soil Properties'),
      'element_id' => 'chem-props',
      'short_name' => 'chem_props',
      'survey_type' => 'dirt_twice_year_data_survey',
    ),
  );
}


/**
 * Returns suggested group columns for DIRT project; assumes few (if any)
 * changes have been made to the pre-installed DIRT fields. Group columns
 * may be altered later via the admin interface.
 *
 * @retval Array of group info.
 */
function dirt_datatable_get_suggested_column_groups() {
  // Here we provide the short name for the filter group. After the
  // filter groups have been inserted into the table, we'll get the
  // filter group IDs (fids) and store the fid in the 'filter_group'
  // column instead of the short name.
  return array(
    0 => array(
      'label' => t('Common'),
      'class' => 'common',
      'short_name' => 'common',
      'survey_type' => 'common',
      'filter_group' => '',
    ),
    1 => array(
      'label' => t('Site Address'),
      'class' => '',
      'short_name' => 'site_address',
      'survey_type' => 'dirt_site_description_survey',
      'filter_group' => 'site_desc',
    ),
    2 => array(
      'label' => t('Geographical Coordinates in Decimal Degrees'),
      'class' => '',
      'short_name' => 'geo_coords',
      'survey_type' => 'dirt_site_description_survey',
      'filter_group' => 'site_desc',
    ),
    3 => array(
      'label' => t('Site Description'),
      'class' => '',
      'short_name' => 'site_desc',
      'survey_type' => 'dirt_site_description_survey',
      'filter_group' => 'site_desc',
    ),
    4 => array(
      'label' => t('Soil Core Variable Data'),
      'class' => '',
      'short_name' => 'soil_core',
      'survey_type' => 'dirt_site_description_survey',
      'filter_group' => 'soil_core',
    ),
    5 => array(
      'label' => t('Temperatures and Cloud Cover'),
      'class' => '',
      'short_name' => 'temp_cloud',
      'survey_type' => 'dirt_monthly_data_survey',
      'filter_group' => 'temp_cloud',
    ),
    6 => array(
      'label' => t('Soil Respiration'),
      'class' => '',
      'short_name' => 'soil_resp',
      'survey_type' => 'dirt_monthly_data_survey',
      'filter_group' => 'soil_resp',
    ),
    7 => array(
      'label' => t('Calculations for Bulk Density, Water Content, and Soil Water-Filled Pore Space (%WFP)'),
      'class' => '',
      'short_name' => 'bulk_density',
      'survey_type' => 'dirt_monthly_data_survey',
      'filter_group' => 'bulk_density',
    ),
    8 => array(
      'label' => t('Chemical Soil Properties'),
      'class' => '',
      'short_name' => 'chem_props',
      'survey_type' => 'dirt_twice_year_data_survey',
      'filter_group' => 'chem_props',
    ),
  );
}


/**
 * Helper function to retrieve column name (in data search table) from field
 * name.
 *
 * @param[in] $field_name String field machine name.
 * @param[in] $delta *Optional* Integer representing delta in case of field
 *   collection fields (optional, defaults to NULL).
 */
function _dirt_datatable_get_column_name_from_field_name($field_name, $delta = NULL) {
  // Get column name from field name by removing the "field_" prefix
  $column_name = str_replace('field_', '', $field_name);

  // If delta present, append to column name as column names must be unique.
  if ($delta > 0) $column_name .= '_' . $delta;

  return $column_name;
}

