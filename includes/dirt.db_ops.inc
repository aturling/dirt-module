<?php

/**
 * @file
 * This file includes wrappers for db_insert, db_update, and db_delete used in
 * the DIRT Core module and any submodules.
 */

/**
 * Helper function for database insertions.
 *
 * Wrap database insertions with try/catch to roll back db transactions if
 * needed.
 *
 * See: https://www.drupal.org/docs/7/api/database-api/error-handling
 *
 * @param[in] $table_name Table name string to insert row into.
 * @param[in] $fields Array of field_name => value for the row insertion.
 *
 * @retval Integer auto-increment ID field value of inserted row, if
 *   applicable; returns 0 for tables with no auto-increment field.
 */
function _dirt_insert_into_database_table($table_name, $fields) {
  $txn = db_transaction();

  try {
    $id = db_insert($table_name)
      ->fields($fields)
      ->execute();

    return $id;
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now
    $txn->rollback();

    // Log the exception
    watchdog_exception('dirt', $e);

    // Throw exception
    throw $e;
  }
}


/**
 * Helper function for database updates.
 *
 * Wrap database updates with try/catch to roll back db transactions if
 * needed.
 *
 * See: https://www.drupal.org/docs/7/api/database-api/error-handling
 *
 * @param[in] $table_name Table name string to insert row into.
 * @param[in] $fields Array of field_name => value for the row update.
 * @param[in] $conditions Array of conditions with keys 'field',
 *   'value', 'operator'.
 *
 * @retval Integer number of rows updated in table.
 */
function _dirt_update_database_table($table_name, $fields, $conditions) {
  $txn = db_transaction();

  try {
    $query = db_update($table_name);
    $query->fields($fields);
    foreach($conditions as $condition) {
      $query->condition($condition['field'], $condition['value'], $condition['operator']);
    }
    $num_updated = $query->execute();
    return $num_updated;
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now
    $txn->rollback();

    // Log the exception
    watchdog_exception('dirt', $e);

    // Throw exception
    throw $e;
  }
}


/**
 * Helper function for database deletions.
 *
 * Wrap database deletions with try/catch to roll back db transactions if
 * needed.
 *
 * See: https://www.drupal.org/docs/7/api/database-api/error-handling
 *
 * @param[in] $table_name Table name string to delete row(s) from.
 * @param[in] $conditions Array of conditions with keys 'field',
 *   'value', 'operator', used to identify row(s) to delete.
 *
 * @retval Integer number of rows deleted from table.
 */
function _dirt_delete_from_database_table($table_name, $conditions) {
  $txn = db_transaction();

  try {
    $query = db_delete($table_name);
    foreach($conditions as $condition) {
      $query->condition($condition['field'], $condition['value'], $condition['operator']);
    }
    $num_deleted = $query->execute();

    return $num_deleted;
  }
  catch (Exception $e) {
    // Something went wrong somewhere, so roll back now
    $txn->rollback();

    // Log the exception
    watchdog_exception('dirt', $e);

    // Throw exception
    throw $e;
  }
}

