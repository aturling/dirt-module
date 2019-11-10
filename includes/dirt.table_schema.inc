<?php

/**
 * @file
 * This file specifies the definitions for custom database tables created by this
 * module. Includes initial database table values for module installation.
 */

/**
 * Returns database table schema for this module.
 *
 * @retval Array containing schema definition whose keys are table names and
 *   values are table structure definitions. 
 */
function dirt_get_table_schema() {
  /**
   * This table stores the custom survey types and their collection
   * frequencies and months (if applicable).
   */
  $schema['dirt_survey_types'] = array(
    'description' => 'The table for storing survey type information.',
    'fields' => array(
      'type' => array(
        'description' => t('Survey type machine name'),
        'type' => 'varchar',
        'length' => 32,
        'default' => NULL,
      ),
      'collection_frequency' => array(
        'description' => 'Number of times per year survey is collected',
        'type' => 'int',
        'unsigned' => FALSE,
        'size' => 'tiny',
        'default' => NULL,
      ),
      'collection_month' => array(
        'description' => 'Month of the year (1-12) that survey is to be collected',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'tiny',
        'default' => NULL,
      ),
      'delta' => array(
        'description' => 'Index for multiple collection months, if applicable',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'normal',
        'default' => 0,
      ),
      'admin_only' => array(
        'description' => 'Whether this survey is to be filled out by admins only.',
        'type' => 'int',
        'unsigned' => TRUE,
        'size' => 'tiny',
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
    'primary key' => array('type', 'collection_frequency', 'collection_month', 'delta'),
    'indexes' => array(
      'type' => array('type'),
      'collection_frequency' => array('collection_frequency'),
      'collection_month' => array('collection_month'),
    ),
  );

  return $schema;
}


/**
 * Returns list of machine names of custom survey types installed in this
 * module.
 *
 * @retval Array of survey type machine names.
 */
function dirt_get_installed_survey_types() {
  return array_keys(dirt_get_installed_survey_type_info());
}


/**
 * Returns array of survey types installed in this module, keyed by survey type
 * machine name, with values being an array of survey info with keys:
 * - name: Type machine name,
 * - description: Type description (same as in node_info()),
 * - collection_frequency: Number of times per year survey is to be
 *   collected, or DIRT_ONE_TIME_COLLECTION if only collected one time.
 * - collection_months: (optional) Specifies the months of collection. For
 *   instance, monthly surveys are expected to be collected every month except
 *   December and January (too cold), and twice a year surveys are expected to
 *   be collected in May and August. If collection_months not specified,
 *   surveys will be collected at evenly spaced intervals throughout the year
 *   according to their collection frequency.
 *
 * @retval Array with keys as survey type machine names and values as array of
 *  survey type information.
 */
function dirt_get_installed_survey_type_info() {
  return array(
    'dirt_group_information_survey' => array(
      'name' => t('Group Information Survey'),
      'description' => t('This is the <em>group information</em> portion of ' .
        'the soil survey form.'),
      'collection_frequency' => DIRT_ONE_TIME_COLLECTION,
    ),
    'dirt_site_description_survey' => array(
      'name' => t('Site Description Survey'),
      'description' => t('This is the <em>site description</em> portion of ' .
        'the soil survey form.'),
      'collection_frequency' => DIRT_ONE_TIME_COLLECTION,
    ),
    'dirt_monthly_data_survey' => array(
      'name' => t('Monthly Data Collection Survey'),
      'description' => t('This is the <em>monthly data collection</em> ' .
        'portion of the soil survey form.'),
      'collection_frequency' => 12,
      'collection_months' => array(
        '2', '3', '4', '5', '6', '7', '8', '9', '10', '11',
       ),
    ),
    'dirt_twice_year_data_survey' => array(
      'name' => t('Twice a Year Data Collection Survey'),
      'description' => t('This is the <em>twice a year data collection</em> ' .
        'portion of the soil survey form.'),
      'collection_frequency' => 2,
      'collection_months' => array('5', '8'),
    ),
  );
}

