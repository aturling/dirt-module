<?php

/**
 * @file
 * This file includes functions handling the admin interface for the DIRT module.
 */

/**
 * Callback for admin data entry portal config form.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_admin_data_entry_portal_config_form($form, &$form_state) {
  // Add CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt') . '/css/dirt_admin.css';

  // Add info on data entry portal nodes created upon installation
  $form['dirt_data_entry_portal_nodes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Entry Portal Pages'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );

  // Build table rows
  $rows = array();

  form_load_include($form_state, 'inc', 'dirt', 'includes/dirt.nodes');

  // Get info on nodes created
  $type = 'dirt_data_entry_portal_page'; 
  $nodes = node_load_multiple(array(), array('type' => $type)); 
  foreach($nodes as $node) {
    $title = $node->title;
    $nid = $node->nid;

    $links_markup  = '<ul class="links inline">';
    $links_markup .= '<li class="first">';
    $links_markup .= l(t('view'), 'node/' . $nid, array('attributes' => array('target'=>'_blank')));
    $links_markup .= '</li><li class="last">';
    $links_markup .= l(t('edit'), 'node/' . $nid . '/edit', array('attributes' => array('target'=>'_blank')));
    $links_markup .= '</li></ul>';

    $rows[] = array(
      'data' => array(
        array(
          'data' => $title,
        ),
        array(
          'data' => $links_markup,
        ),
      ),
      'class' => array('dirt-table-row'),
    );
  }

  $form['dirt_data_entry_portal_nodes']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('The following pages were automatically created upon ' .
                   'installation of the DIRT Citizen Science Module.<br>' .
                   'These are suggested pages for your Citizen Science ' .
                   'project.<br>Review each page and enter content for your ' .
                   'project if applicable. Feel free to delete any pages ' .
                   'that you will not need for your project.<br><br>'),
    '#weight' => 0,
  );

  $form['dirt_data_entry_portal_nodes']['table2'] = array(
    '#type' => 'markup',
    '#theme' => 'table',
    '#weight' => 1,
    '#header' => array(t('Title'), t('Links')),
    '#rows' => $rows,
    '#empty' => t('No suggested pages found.'),
  );

  // Add info on soil health surveys menu
  $form['dirt_data_entry_menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Entry Portal Menu'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  $form['dirt_data_entry_menu']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('The Soil Health Surveys menu was also created upon ' .
      'module installation. This menu appears at the top of every page ' .
      'listed in the table above.') . '<br><br>' . t('The links may be ' .
      'customized by editing the menu ' .
      l(t('here'), 'admin/structure/menu/manage/dirt-menu-soil-health-surveys') .
      '. The menu can be moved to another region on the page (e.g., first ' .
      'sidebar) by configuring the menu block ' .
      l(t('here'), 'admin/structure/block')) . '.',
    '#weight' => 0,
  );

  // Add option to change project start date
  $form['dirt_project_start_date'] = array(
    '#type' => 'fieldset',
    '#title' => t('DIRT Project Start Year'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 2,
  );

  $form['dirt_project_start_date']['start_year'] = array(
    '#type' => 'textfield',
    '#title' => t('Project Start Year'),
    '#description' => t('Defaults to year DIRT module installed. Use this ' .
      'form to set the project start date to another year.'),
    '#default_value' => variable_get('dirt_project_start_year'),
    '#required' => TRUE,
    '#weight' => 0,
  );

  $form['dirt_project_start_date']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Start Year'),
    '#attributes' => array(
       'class' => array('form-actions', 'form-wrapper'),
    ),
    '#submit' => array('dirt_admin_update_project_start_year_submit'),
    '#weight' => 1,
  );

  // Add additional validation
  $form['#validate'][] = 'dirt_start_year_form_validate';

  return $form;
}


/**
 * Validation function for the project start year form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_start_year_form_validate($form, &$form_state) {
  $start_year = $form_state['values']['start_year'];

  // Check that start year is 4 digits, where the first digit is 1 or 2,
  // and the next three digits are 0-9.
  if ((strlen($start_year) > 0) && !preg_match('/^[1-2]{1}[0-9]{3}$/', $start_year)) {
    form_set_error('start_year', t('Invalid project start year.'));
  }
}


/**
 * Submit handler for updating project start year.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_admin_update_project_start_year_submit($form, &$form_state) {
  // Update project start year
  variable_set('dirt_project_start_year', $form_state['values']['start_year']);

  drupal_set_message(t('Configuration saved.'));
}


/**
 * Callback for site ID lookup form.
 *
 * This form has two stages:
 * 1) Lookup fields form.
 * 2) Display lookup results.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_admin_site_id_lookup_form($form, &$form_state) {
  // Add custom CSS and JS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt') . '/css/dirt_admin.css';

  // Get current stage of form (default 1)
  $stage = (isset($form_state['store']['stage'])) ? $form_state['store']['stage'] : 1;

  // Set the form stage
  $form_state['store']['stage'] = $stage;

  // Two stages:
  switch($stage) {
    case 1:
      // Stage 1: Lookup fields form
      return dirt_admin_site_id_lookup_fields_form($form, $form_state);

    case 2:
      // Stage 2: Present lookup results
      return dirt_admin_site_id_lookup_results_form($form, $form_state);
  }
}


/**
 * Callback for lookup fields portion of site ID lookup form (stage 1).
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_admin_site_id_lookup_fields_form($form, &$form_state) {
  // Header and instructions
  $form['dirt_header_and_instructions'] = array(
    '#type' => 'markup',
    '#markup' => t('Use the following form to look up a Site ID by its ' .
      'habitat type and full name of the participant collecting data at the ' .
      'site.'),
    '#weight' => -3,
  );

  // Site lookup fieldset and fields:
  $form['dirt_site_lookup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site ID lookup'),
    '#description' => t('All three of the following fields are required.') .
      '<br><em>' . t('First and last name fields are case-insensitive.') .
      '</em>',
    '#weight' => 0,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Habitat type
  // Get habitat options, convert string to array:
  $habitat_options = dirt_get_habitat_type_options();
  $form['dirt_site_lookup']['habitat_type_select'] = array(
    '#prefix' => '<div id="habitat-type-select">',
    '#suffix' => '</div>',
    '#type' => 'select',
    '#title' => t('Habitat type'),
    '#options' => $habitat_options,
    '#weight' => -1,
    '#required' => TRUE,
  );

  // First name and last name
  $name_fields = array(
    'first_name' => t('First name'),
    'last_name' => t('Last name'),
  );
  foreach($name_fields as $name_field => $label) {
    $form['dirt_site_lookup'][$name_field] = array(
      '#prefix' => '<div id="' . $name_field . '-textfield">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#title' => $label,
      '#size' => 40,
      '#weight' => array_search($label, $name_fields),
      '#required' => TRUE,
    );
  }

  // Form submit
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#submit' => array('dirt_process_site_id_lookup_submit'),
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#weight' => 2,
  );

  return $form;
}


/**
 * Submit handler for Site ID lookup form (stage 1).
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_process_site_id_lookup_submit($form, &$form_state) {
  if (!form_get_errors()) {
    $form_state['rebuild'] = TRUE;

    // Increment to next stage
    $form_state['store']['stage']++;

    // Store submitted values
    $form_state['store']['habitat_type_select'] = $form_state['values']['habitat_type_select'];
    $form_state['store']['first_name'] = $form_state['values']['first_name'];
    $form_state['store']['last_name'] = $form_state['values']['last_name'];
  }
}


/**
 * Callback for displaying results of Site ID lookup form (stage 2).
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_admin_site_id_lookup_results_form($form, &$form_state) {
  // Initialize lookup results
  $form_info = array();

  // Get habitat type string from select idx
  $habitat_options = dirt_get_habitat_type_options();
  $habitat_type = $habitat_options[$form_state['store']['habitat_type_select']];

  // Store the fields in an array
  $results = array(
    'full_name' => $form_state['store']['first_name'] . ' ' . $form_state['store']['last_name'],
    'first_name' => $form_state['store']['first_name'],
    'last_name' => $form_state['store']['last_name'],
    'habitat_type' => $habitat_type,
  );

  // Find user/site id info belongs to
  $form_info = dirt_admin_find_site_ids_from_user_input($results);

  // Display results of search
  dirt_admin_add_site_id_lookup_results($form, $form_info);

  // Reset form - next time will begin at stage 1:
  $form_state['rebuild'] = TRUE;
  unset($form_state['store']['stage']);

  $form['dirt_reset_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Perform another lookup'),
    '#prefix' => '<div id="dirt-perform-another-lookup-button">',
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#weight' => 100,
  );

  return $form;
}


/**
 * Search for Site ID that matches user input.
 *
 * @param[in] $results Array of user input from form with keys:
 *   'full_name', 'first_name', 'last_name', 'habitat_type'.
 *
 * @retval Array with same keys as $results plus key 'site_ids' for
 *   matching Site ID(s), if found.
 */
function dirt_admin_find_site_ids_from_user_input($results) {
  // Initialize return
  $form_info = array();

  // Set info from user input
  $keys = array('full_name', 'first_name', 'last_name', 'habitat_type');
  foreach($keys as $field) {
    $form_info[$field] = $results[$field];
  }

  // First run database query to find users matching first and last name
  // (possibly multiple results).
  $uids = dirt_admin_query_db_for_user_ids($results['first_name'], $results['last_name']);
  
  // Next get habitat type for each uid and compare to user input.
  $site_ids = array();
  foreach($uids as $uid) {
    // Get site description survey for uid/site id
    $nid = dirt_get_most_recent_survey_of_type_for_user($uid, 'dirt_site_description_survey');

    if ($nid) {
      // Get habitat type from survey
      $habitat_type = '';

      try {
        $node_wrapper = entity_metadata_wrapper('node', $nid);
        $habitat_type = $node_wrapper->field_dirt_site_habitat_type->value();
      }
      catch (EntityMetadataWrapperException $e) {
        watchdog_exception('dirt', $e);
      }

      // Check if habitat type matches user input. If so, add site ID as a
      // match.
      if ($habitat_type == $results['habitat_type']) $site_ids[] = $uid;

      unset($node_wrapper);
      unset($habitat_type);
    }

    unset($nid);
  }

  $form_info['site_ids'] = $site_ids;

  return $form_info;
}


/**
 * Display Site ID lookup results on form (stage 2).
 *
 * @param[in,out] $form Form array.
 * @param[in] $form_info Array of user input from form with keys:
 *   'full_name', 'first_name', 'last_name', 'habitat_type', and 'site_ids',
 *   where 'site_ids' field contains array of matching Site IDs from
 *   lookup.
 */
function dirt_admin_add_site_id_lookup_results(&$form, $form_info) {
  // Add markup for explanation of findings
  $form['dirt_explanation'] = array(
    '#type' => 'markup',
    '#markup' => t('The information entered was used to locate the Site ID ' .
      'in the system; below are the results.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#weight' => -20,
  );

  // Add markup for results
  $fields = array(
    'first_name' => t('First Name'),
    'last_name' => t('Last Name'),
    'habitat_type' => t('Habitat Type'),
  );
  $results_markup = '';
  foreach($fields as $field => $label) {
    $results_markup .= '<strong>' . $label . ':</strong> ';
    $results_markup .= ucfirst(strtolower($form_info[$field])) . '<br>';
  }

  // Site IDs (array)
  $results_markup .= '<strong>' . t('Site ID') . ':</strong> ';
  $site_ids = $form_info['site_ids'];
  if (!empty($site_ids)) {
    dirt_admin_create_site_id_links($site_ids);
    $results_markup .= (count($site_ids) > 1) ? t('Multiple matches found: ') : '';
    $results_markup .= implode(', ', $site_ids);
  }
  else {
    $results_markup .= t('No results found.');
  }

  $form['dirt_lookup_results'] = array(
    '#type' => 'markup',
    '#markup' => $results_markup,
    '#prefix' => '<div id="individual-lookup-results">',
    '#suffix' => '</div>',
    '#weight' => -10,
  );
}


/**
 * Query database for DIRT participants with matching first and last name
 * given as input.
 *
 * @param[in] $first_name First name string.
 * @param[in] $last_name Last name string.
 *
 * @retval Array of user IDs of matching participants.
 */
function dirt_admin_query_db_for_user_ids($first_name, $last_name) {
  // Initialize return
  $uids = array();

  // Get DIRT Data Entry Participant role ID
  $rid = variable_get('dirt_data_entry_user_role', 0);

  if ($first_name && $last_name) {
    // Set up query
    $query = db_select('users', 'u');

    // Limit to DIRT Data Entry Participants
    $query->leftJoin('users_roles', 'roles', 'u.uid = roles.uid');
    $query->condition('roles.rid', $rid);  // Limit to DIRT participants

    // Search on first and last names
    $query->leftJoin('field_data_field_dirt_user_first_name', 'first_name', 'u.uid = first_name.entity_id');
    $query->leftJoin('field_data_field_dirt_user_last_name', 'last_name', 'u.uid = last_name.entity_id');
    $query->condition('first_name.field_dirt_user_first_name_value', $first_name);
    $query->condition('last_name.field_dirt_user_last_name_value', $last_name);

    // Get matching site IDs (uids)
    $query->fields('u', array('uid'));

    // Order by Site ID
    $query->orderBy('u.uid', 'ASC');
    
    // Get all results
    $result = $query->execute()->fetchAll();

    foreach($result as $result_obj) {
      $uids[] = $result_obj->uid;
    } 
  }

  return $uids;
}


/**
 * Replaces an array of Site IDs with links to the corresponding user account.
 *
 * @param[in,out] $site_ids Array of Site IDs, each to be converted to a link to
 *   that account.
 */
function dirt_admin_create_site_id_links(&$site_ids) {
  foreach($site_ids as $site_idx => $site_id) {
    $site_ids[$site_idx] = l($site_id, 'user/' . $site_id, array('attributes' => array('target'=>'_blank')));
  }
}


/**
 * Callback for admin survey config page form.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_admin_survey_config_form($form, &$form_state) {
  // Add CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt') . '/css/dirt_admin.css';

  // Add checkbox on whether to allow participants to change site description survey answers
  $form['dirt_site_desc_changes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site Description Survey Changes'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );

  $form['dirt_site_desc_changes']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('Participants will always have the option of updating ' .
                   'their site description surveys to add answers to blank ' .
                   'fields (fields they have not yet filled in). This allows ' .
                   'participants to fill out the surveys in multiple ' .
                   'sessions and land management updates as they occur.<br>' .
                   '<br>If the box below is checked, participants will be ' .
                   'also able to edit site description surveys to <strong>' .
                   'CHANGE</strong> their answers for fields they have ' .
                   'already filled in. This option is <strong>NOT</strong> ' .
                   'enabled by default; check the box below to enable it.<br>' .
                   '<br>If the box below remains unchecked, participants ' .
                   'will not be able to change their answers on site ' .
                   'description surveys without contacting an administrator ' .
                   'to approve the change.<br><br><em>Since site ' .
                   'description fields are not expected to change over time ' .
                   '(aside from any land management updates), admins may ' .
                   'wish to disallow participants to change their site ' .
                   'description survey answers (i.e., leave the box below ' .
                   'unchecked). This may be the case, for example. if the ' .
                   'site description responses are manually validated at ' .
                   'some point after submission; then the participants ' .
                   'should not be able to make changes to already-validated ' .
                   'data.</em><br><br>'),
    '#weight' => 0,
  );

  $form['dirt_site_desc_changes']['site_desc_changes_checkbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow participants to change their site description survey responses'),
    '#description' => '',
    '#default_value' => variable_get('dirt_config_site_desc_changes_allowed'),
    '#weight' => 1,
  );

  // Submit button
  $form['dirt_submit_admin_survey_config'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#attributes' => array(
       'class' => array('form-actions', 'form-wrapper'),
    ),
    '#submit' => array('dirt_admin_survey_config_submit'),
    '#weight' => 10,
  );   

  return $form;
}


/**
 * Submit handler to update survey configuration.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_admin_survey_config_submit($form, &$form_state) {
  variable_set('dirt_config_site_desc_changes_allowed', $form_state['values']['site_desc_changes_checkbox']);
}


/**
 * Callback for admin survey types form.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_admin_set_survey_types_form($form, &$form_state) {
  // Add CSS and JS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt') . '/css/dirt_admin.css';
  drupal_add_js(drupal_get_path('module', 'dirt') . '/js/dirt_admin.js');

  $survey_types = dirt_get_survey_types();

  $form['#tree'] = TRUE;

  // First display a summary of current survey types in system
  $form['dirt_survey_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Current survey types'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );

  $header = array(
    t('Name'),
    t('Collection Frequency'),
    t('Collection Months'),
  );

  $rows = dirt_get_survey_type_table_rows($form_state, $survey_types);

  $form['dirt_survey_types']['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No survey types found'),
  );

  // Add option to designate an existing content type as a survey type.
  // Will need to collect the frequency and months (if applicable).

  $form['dirt_add_survey_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a survey type'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 1,
    '#description' => t('Select an existing content type to add as a survey ' .
      'type. To add a survey type not in this list, first ') .
      l(t('create a new content type'), 'admin/structure/types/add') . '.',
  ); 

  $type_options = array();

  // Eligible types to be added as survey types are a subset of all node types.
  // Remove any content types added in this module (either already a survey
  // type or not eligible to become one):
  $all_node_type_names = node_type_get_names();
  $installed_types = dirt_get_all_custom_content_types();
  $eligible_types = array_diff(array_keys($all_node_type_names), $installed_types);

  // Also remove any current survey types:
  $eligible_types = array_diff($eligible_types, $survey_types);

  // Finally, also remove the two types that come with Drupal core, page and
  // article (forces admin to create a custom content type specifically for new
  // survey type):
  $core_types = array('page', 'article');
  $eligible_types = array_diff($eligible_types, $core_types);

  foreach($eligible_types as $type) {
    $type_options[$type] = $all_node_type_names[$type];
  }

  // Add type select
  _dirt_add_survey_type_select_form_field($form['dirt_add_survey_type'], $form_state, $type_options);

  // Add collection frequency field
  _dirt_add_collection_frequency_select_form_field($form['dirt_add_survey_type'], $form_state, 'add_survey_type');

  // Add collection months field (optional)
  _dirt_add_collection_months_checkboxes_form_field($form['dirt_add_survey_type'], $form_state, 'add_survey_type');

  // Add form submit
  $form['dirt_add_survey_type']['add_survey_type_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add survey type',
    '#attributes' => array(
       'class' => array('form-actions', 'form-wrapper'),
    ),
    '#submit' => array('dirt_save_new_survey_type_form_submit'),
  );

  // Add option to remove survey type designation from content type.
  $form['dirt_delete_survey_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delete a survey type'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 2,
    '#description' => t('Select a survey type to remove. Note that this does ') .
      '<strong>' . t('not') . '</strong>' . t(' delete the content type ' .
      'itself; it only removes its designation as a survey type in the system.'),
  );

  $type_options = array();
  foreach($survey_types as $type) {
    $type_options[$type] = $all_node_type_names[$type];
  }

  // Add type select
  _dirt_add_survey_type_select_form_field($form['dirt_delete_survey_type'], $form_state, $type_options);

  // Add form submit
  $form['dirt_delete_survey_type']['delete_survey_type_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete survey type',
    '#attributes' => array(
       'class' => array('form-actions', 'form-wrapper'),
    ),
    '#submit' => array('dirt_delete_survey_type_form_submit'),
  );

  // Add additional validation
  $form['#validate'][] = 'dirt_set_survey_types_form_validate';

  return $form;
}


/**
 * Validation handler for set survey types form.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_set_survey_types_form_validate($form, &$form_state) {
  // Multiple submits so check op value
  $op = (array_key_exists('op', $form_state['values'])) ? $form_state['values']['op'] : '';
  // Shorten op to 'add' vs. 'delete':
  if ($op === 'Add survey type') $op = 'add';
  elseif ($op === 'Delete survey type') $op = 'delete';

  if ($op) {
    // Send op to JS
    drupal_add_js(array('dirt' => array('op' => $op)), array('type' => 'setting'));

    // Require content type
    if (!$form_state['values']['dirt_' . $op . '_survey_type']['type_select'])
      form_set_error('dirt_' . $op . '_survey_type][type_select', t('Content type required.'));

    // Collection frequency already required by drop-down
  }
}


/**
 * Submit handler for adding a new survey type.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_save_new_survey_type_form_submit($form, &$form_state) {
  $type = $form_state['values']['dirt_add_survey_type']['type_select'];
  $collection_frequency = $form_state['values']['dirt_add_survey_type']['type_frequency'];
  $collection_months = array_filter($form_state['values']['dirt_add_survey_type']['type_months']);
  // TODO: no form option for admin-only surveys at the moment

  try {
    // Add survey type
    dirt_add_survey_type($type, $collection_frequency, $collection_months);

    drupal_set_message('New survey type saved.');
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while attempting to add survey ' .
      'type. See ') .
      l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/**
 * Submit handler for deleting survey type.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_delete_survey_type_form_submit($form, &$form_state) {
  $type = $form_state['values']['dirt_delete_survey_type']['type_select'];

  try {
    dirt_delete_survey_type($type);

    drupal_set_message('Survey type deleted.');
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while attempting to delete survey ' .
      'type. See ') .
      l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/**
 * Returns rows of all survey types with their collection information
 * for survey type summary table.
 *
 * @param[in,out] $form_state Form state array.
 * @param[in] $survey_types Array of survey type machine names.
 *
 * @retval Array of rows formatted for form element with 'table' theme.
 */
function dirt_get_survey_type_table_rows(&$form_state, $survey_types) {
  $rows = array();

  foreach($survey_types as $type) {
    // Get collection frequency as integer
    $frequency_num = dirt_get_collection_frequency_of_survey_type($type);

    $months = t('N/A', array(), array('context' => 'Abbreviation for "not applicable"'));
    if ($frequency_num !== DIRT_ONE_TIME_COLLECTION) {
      // Determine collection months: if specified, use those. Otherwise
      // collection months will be evenly spaced, beginning with January.
      $months = dirt_get_collection_months_of_survey_type($type);
      if (empty($months)) {
        $month_skip = 12 / $frequency_num;
        $months = ($frequency_num === 1) ? array(1) : range(1, 12, $month_skip);
      }

      // Display month names, not numbers
      foreach($months as $idx => $month) {
        $months[$idx] = format_date(mktime(0, 0, 0, $month, 1), 'custom', 'M');
      }
      $months = implode(', ', $months);
    }

    $frequency = _dirt_get_collection_frequency_string_from_number($frequency_num);

    $rows[] = array(
      'data' => array(
        array(
          'data' => dirt_get_name_of_survey_type($type),
        ),
        array(
          'data' => $frequency,
        ),
        array(
          'data' => $months,
        ),
      ),
      'class' => array('summary-table-row'),
    );
  }

  return $rows;
}


/**
 * Helper function to add a drop down form field for selecting survey types.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 * @param[in] $type_options Array of drop down options.
 */
function _dirt_add_survey_type_select_form_field(&$form, &$form_state, $type_options) {
  $form['type_select'] = array(
    '#type' => 'select',
    '#title' => t('Select content type'),
    '#empty_value' => '',
    '#empty_option' => t('- Select content type -'),
    '#options' => $type_options,
  );
}


/**
 * Helper function to add a drop down form field for selecting collection frequency.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 * @param[in] $parent Parent element string in form.
 * @param[in] $default_frequency *Optional* Default frequency value (optional).
 */
function _dirt_add_collection_frequency_select_form_field(&$form, &$form_state, $parent, $default_frequency = '') {
  $frequency_options[DIRT_ONE_TIME_COLLECTION] = _dirt_get_collection_frequency_string_from_number(DIRT_ONE_TIME_COLLECTION);
  foreach(range(1,12) as $i) {
    $frequency_options[$i] =  _dirt_get_collection_frequency_string_from_number($i);
  }

  $form['type_frequency'] = array(
    '#type' => 'select',
    '#title' => t('Select collection frequency'),
    '#options' => $frequency_options,
    '#description' => t('Select the collection frequency for this survey type.'),
    '#states' => array(
      'invisible' => array(
        'select[name="dirt_' . $parent . '[type_select]"]' => array('value' => '')
       ),
    ),
  );
}


/**
 * Helper function to add a checkboxes form field for selecting collection months.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 * @param[in] $parent Parent element string in form.
 * @param[in] $default_months *Optional* Default collection months (optional).
 */
function _dirt_add_collection_months_checkboxes_form_field(&$form, &$form_state, $parent, $default_months = array()) {
  $months_options = array();
  foreach(range(1,12) as $i) {
    $months_options[$i] = format_date(mktime(0, 0, 0, $i, 1), 'custom', 'M');
  }

  $form['type_months'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select collection months (optional)'),
    '#description' => t('Optionally, select specific months for the survey ' .
      'to be collected. This does not need to agree exactly with the ' .
      'selected collection frequency (for example, the DIRT Core "monthly" ' .
      'surveys exclude December and January). If no months are specified, ' .
      'the collection months will be determined by the collection frequency ' .
      '(evenly spaced throughout the year, beginning in January).'),
    '#options' => $months_options,
    '#states' => array(
      'invisible' => array(
        array(
          // Don't display field until content type has been selected...
          'select[name="dirt_' . $parent . '[type_select]"]' => array('value' => ''),
        ),
        array(
          // ...OR content type has been selected but frequency is "one time"
          // (doesn't make sense to specify collection months for a one-time
          // survey).
          'select[name="dirt_' . $parent . '[type_frequency]"]' => array('value' => '-1'),
        ),
      ),
    ),
  );
}


/**
 * Helper function that returns a natural language string for collection
 * frequency number.
 *
 * @param[in] $collection_frequency Collection frequency integer.
 *
 * @retval String collection frequency string.
 */
function _dirt_get_collection_frequency_string_from_number($collection_frequency) {
  switch ($collection_frequency) {
    case 1:
      return t('Once a year');
 
    case 2:
      return t('Twice a year');

    case 12:
      return t('Monthly');

    case DIRT_ONE_TIME_COLLECTION:
      return t('One time');

    default:
      return t('@count times a year', array('@count' => $collection_frequency));
  }
}

