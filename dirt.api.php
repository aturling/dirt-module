<?php

/**
 * @file
 * Hooks provided by the DIRT Core module.
 */

/**
 * Respond to the addition of a new survey type.
 *
 * This hook allows modules to define additional behavior or requirements after
 * a new survey type is added (i.e., an existing content type is designated as
 * a survey type).
 *
 * @param[in] $type Machine name of content type being added as a survey type.
 */
function hook_dirt_add_survey_type($type) {
  // Perform additional operations when a new survey type is added.
}


/**
 * Respond to the deletion of a survey type.
 *
 * This hook allows modules to define additional behavior or requirements after
 * a survey type is deleted (i.e., a content type is no longer designated as a
 * survey type).
 *
 * @param[in] $type String type machine name.
 */
function dirt_delete_survey_type($type) {
  // Perform additional operations when a survey type is deleted.
}

