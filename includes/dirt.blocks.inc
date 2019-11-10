<?php

/**
 * @file
 * This file includes functions handling DIRT module custom blocks.
 */


/**
 * Returns block info for DIRT module custom blocks.
 *
 * @retval Array of block info.
 */
function dirt_get_installed_blocks() {
  $blocks = array();

  // Access Data Entry Portal Block
  $blocks['dirt_access_data_entry_portal'] = array(
    'info' => t('DIRT Access Data Entry Portal Block'),
  );

  // Data Entry Site Info Block
  $blocks['dirt_data_entry_site_info'] = array(
    'info' => t('DIRT Data Entry Site Info Block'),
  );

  return $blocks;
}


/**
 * Returns form items for custom block.
 *
 * @param[in] $delta *Optional* Block delta string.
 *
 * @retval Form array.
 */
function dirt_get_block_form($delta='') {
  $form = array();

  switch($delta) {
    case 'dirt_access_data_entry_portal':
      $form['access_data_portal_text'] = array(
        '#type' => 'text_format',
        '#format' => 'full_html',
        '#title' => t('Block Body', array(), array('context' => 'Drupal Block body')),
        '#default_value' => variable_get('dirt_access_data_entry_portal_text_variable'),
      );      
      break;

    case 'dirt_data_entry_site_info':
      $form['site_info_options'] = array(
        '#type' => 'checkboxes',
        '#options' => array(
          'Site ID' => t('Site ID'),
          'Location' => t('Location'),
          'Habitat Type' => t('Habitat Type')
        ),
        '#title' => t('Select site information to display in this block.'),
        '#default_value' => variable_get('dirt_site_info_options_variable'),
      );
      break;
  }

  return $form;
}


/**
 * Updates site variables upon saving custom block.
 *
 * @param[in] $delta *Optional* Block delta string.
 * @param[in] $edit *Optional* Edit array.
 */
function dirt_save_block_form_variables($delta = '', $edit = array()) {
  switch ($delta) {
    case 'dirt_access_data_entry_portal':
      variable_set('dirt_access_data_entry_portal_text_variable', $edit['access_data_portal_text']['value']);
      break;

    case 'dirt_data_entry_site_info':
      variable_set('dirt_site_info_options_variable', $edit['site_info_options']);
      break;
  }
}


/**
 * Sets custom block view.
 *
 * @param[in] $delta *Optional* Block delta string.
 */
function dirt_set_block_view($delta='') {
  $block = array();

  switch ($delta) {
    case 'dirt_access_data_entry_portal':
      $block['subject'] = '';
      $block['content'] = dirt_access_data_entry_portal_block_view();
      break;

    case 'dirt_data_entry_site_info':
      $block['subject'] = t('Site Information');
      $block['content'] = dirt_site_info_block_view();
      break;
  }

  return $block;
}


/**
 * Callback for access data entry portal block view.
 */
function dirt_access_data_entry_portal_block_view() {
  $block = array();

  // Don't display link if we're already at the data entry portal page
  if (current_path() != variable_get('dirt_data_entry_portal_path', DIRT_DATA_ENTRY_PATH_DEFAULT)) {
    global $user;

    $text = variable_get('dirt_access_data_entry_portal_text_variable');
 
    $block['welcome_user'] = array(
      '#type' => 'markup',
      '#markup' => t('Welcome, @username',
        array('@username' => $user->name)),
      '#prefix' => '<h4 align="center">',
      '#suffix' => '</h4>',
    );

    $block['access_data_entry_portal_text'] = array(
      '#type' => 'markup',
      '#markup' => $text,
      '#prefix' => '<div><h1 align="center">',
      '#suffix' => '</h1></div>',
      '#weight' => 1,
    );
  }

  return $block;
}


/**
 * Callback for site info block view.
 */
function dirt_site_info_block_view() {
  $block = array();

  global $user;
  $uid = $user->uid;

  $site_info_options = variable_get('dirt_site_info_options_variable');

  // Add each site info variable (if option selected)

  // Site ID:
  if ($site_info_options && in_array('Site ID', $site_info_options, true)) {
    $site_id_markup = t('Site ID: @id', array('@id' => $uid));

    $block['dirt_site_info_site_id'] = array(
      '#type' => 'markup',
      '#markup' => $site_id_markup,
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
  }

  // Get location and habitat type from site description form, if present
  $nid = dirt_get_most_recent_survey_of_type_for_user($uid, 'dirt_site_description_survey');
  $county = NULL;
  $state = NULL;
  $zip = NULL;
  $location = NULL;
  $habitat = NULL;

  if ($nid) {
    try {
      $node_wrapper = entity_metadata_wrapper('node', $nid);

      if (!empty(field_info_field('field_dirt_site_county')))
        $county = $node_wrapper->field_dirt_site_county->value();

      if (!empty(field_info_field('field_dirt_site_state')))
        $state = $node_wrapper->field_dirt_site_state->value();

      if (!empty(field_info_field('field_dirt_site_zip_code')))
        $zip = $node_wrapper->field_dirt_site_zip_code->value();

      if (!empty(field_info_field('field_dirt_site_habitat_type')))
        $habitat = $node_wrapper->field_dirt_site_habitat_type->value();
    }
    catch (EntityMetadataWrapperException $e) {
      watchdog_exception('dirt', $e);
    }
  }

  // Location:
  $display_location_selected = ($site_info_options && in_array('Location', $site_info_options, true));
  $one_location_field_exists = !(is_null($county) && is_null($state) && is_null($zip));

  if ($display_location_selected && $one_location_field_exists) {
    if ($county) $location .= t('@county county', array('@county' => $county));
    if ($county && $state) $location .= ', ';
    if ($state) $location .= $state . ' ';
    if ($zip) $location .= $zip;

    $location = ($location) ? $location : 'Not yet entered';

    $location_markup = t('Location: @loc', array('@loc' => $location));

    $block['dirt_site_info_site_location'] = array(
      '#type' => 'markup',
      '#markup' => $location_markup,
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
  }

  // Habitat Type:
  $display_habitat_selected = ($site_info_options && in_array('Habitat Type', $site_info_options, true));
  $habitat_field_exists = !(is_null($habitat));

  if ($display_habitat_selected && $habitat_field_exists) {
    $habitat = ($habitat) ? $habitat : 'Not yet entered';

    $habitat_markup = t('Habitat: @habitat', array('@habitat' => $habitat));

    $block['dirt_site_info_site_habitat'] = array(
      '#type' => 'markup',
      '#markup' => $habitat_markup,
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
  }

  return $block;
}

