<?php

/**
 * @file
 * This file includes functions handling the admin interface for the DIRT DataTable module.
 */

/*----- Summary -----*/

/**
 * Callback for admin data search table config form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_config_form($form, &$form_state) {
  // Add custom CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt_datatable') . '/css/dirt_datatable_admin_page.css';

  // Add the summary sections
  dirt_datatable_add_summary_sections($form, $form_state);

  return $form;
}


/**
 * Adds the summary sections to the form object.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_add_summary_sections(&$form, &$form_state) {
  // Summary of collection sites table
  $header = array(
    t('Public Sites'),
    t('Semipublic Sites'),
    t('Semiprivate Sites'),
    t('Total Sites'),
  );

  $form['dirt_datatable_site_summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary of sites currently in data search table'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );

  $form['dirt_datatable_site_summary']['container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'dirt-datatable-summary-table-container',
      'class' => array('table-container'),
    ),
  );

  $form['dirt_datatable_site_summary']['container']['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => dirt_datatable_get_summary_table_rows(),
    '#empty' => t('No collection sites found.'),
    '#prefix' => '<div class="dirt-datatable-summary-table">',
    '#suffix' => '</div>',
    '#weight' => 1,
  );

  // Summary of data search portal nodes created upon installation
  $form['dirt_datatable_data_search_portal_nodes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Search Portal Pages'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  // Build table rows
  $rows = array();

  form_load_include($form_state, 'inc', 'dirt_datatable', 'includes/dirt_datatable.nodes');

  // Get info on nodes created
  $type = 'dirt_datatable_data_search_page';
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
      'class' => array('dirt-datatable-summary-table-row'),
    );
  }

  $form['dirt_datatable_data_search_portal_nodes']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('The following pages were automatically created upon ' .
                   'installation of the DIRT DataTable Module.<br>' .
                   'These contain instructions on how to use the data ' .
                   'search portal. Suggested instructions have been ' .
                   'provided as a starting point.<br>' .
                   'Review each page and enter content for your ' .
                   'project if applicable.<br><br>'),
    '#weight' => 0,
  );

  $form['dirt_datatable_data_search_portal_nodes']['table'] = array(
    '#type' => 'markup',
    '#theme' => 'table',
    '#weight' => 1,
    '#header' => array(t('Title'), t('Links')),
    '#rows' => $rows,
    '#empty' => t('No suggested pages found.'),
  );
}


/**
 * Returns array of summary of sites table rows.
 */
function dirt_datatable_get_summary_table_rows() {
  // Initialize row array
  $rows = array();

  // Query database for current table state and display summary table
  $query = db_select(DIRT_SEARCH_TABLE_DATA, 't');
  $query->fields('t', array('site_id', 'dirt_user_sharing_perms'));
  $query->groupBy('t.site_id'); // count up number of unique site IDs
  $results = $query->execute()->fetchAll();

  // Initialize counts of public, semipublic, and semiprivate sites
  $public_site_count = 0;
  $semipublic_site_count = 0;
  $semiprivate_site_count = 0;

  // Loop through table rows and update counts
  foreach ($results as $result_obj) {
    if ($result_obj->dirt_user_sharing_perms == 0) {
      $public_site_count++;
    }
    elseif ($result_obj->dirt_user_sharing_perms == 1) {
      $semipublic_site_count++;
    }
    else {
      $semiprivate_site_count++;
    }
  }
  $total_count = $public_site_count + $semipublic_site_count + $semiprivate_site_count;

  if ($total_count > 0) {
    // Begin table row
    $rows[0] = array(
      'data' => array(
        array(
          'data' => $public_site_count,
          'class' => 'dirt-datatable-summary-table-cell',
        ),
        array(
          'data' => $semipublic_site_count,
          'class' => 'dirt-datatable-summary-table-cell',
        ),
        array(
          'data' => $semiprivate_site_count,
          'class' => 'dirt-datatable-summary-table-cell',
        ),
        array(
          'data' => $total_count,
          'class' => 'dirt-datatable-summary-table-cell',
        ),
      ),
      'class' => array('dirt-datatable-summary-table-row'),
    );
  }

  unset($total_count);
  unset($semiprivate_site_count);
  unset($semipublic_site_count);
  unset($public_site_count);

  return $rows;
}


/*----- Map config -----*/

/**
 * Callback for map config form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_map_config_form($form, &$form_state) {
  // Add fieldset container
  $form['dirt_datatable_map_config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Search Portal Map Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );

  // Add checkbox whether to display map on data search portal page
  $form['dirt_datatable_map_config']['dirt_datatable_config_display_map'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display map on data search portal page'),
    '#description' => '',
    '#default_value' => variable_get('dirt_datatable_config_display_map'),
    '#weight' => 0,
  );

  $checkbox_checked = array(
    ':input[name="dirt_datatable_config_display_map"]' => array(
      'checked' => TRUE,
    ),
  );

  // Add map API key field
  $form['dirt_datatable_map_config']['dirt_datatable_config_map_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Google Maps API Key'),

    '#description' => t('Enter your Google Maps API Key. View ' .
                        l(t('this page'), 'https://developers.google.com/maps/documentation/javascript/get-api-key',
      array('attributes' => array('target'=>'_blank'))) .
                        ' for more information.'),
    '#states' => array(
      'required' => $checkbox_checked,
    ),
    '#default_value' => variable_get('dirt_datatable_config_map_api_key'),
    '#weight' => 1,
  );

  // Add coordinates for map center fields
  $form['dirt_datatable_map_config']['dirt_map_center'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map Center'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Enter the map center coordinates (latitude and ' .
      'longitude) and initial zoom level. Defaults to the center of the ' .
      'United States with zoom level 5.'),
    '#weight' => 2,
  );

  $form['dirt_datatable_map_config']['dirt_map_center']['dirt_datatable_config_map_lat'] = array(
    '#type' => 'textfield',
    '#title' => t('Latitude'),
    '#default_value' => variable_get('dirt_datatable_config_map_lat'),
    '#weight' => 0,
    '#states' => array(
      'required' => $checkbox_checked,
    ),
    '#description' => t('Enter the latitude in the range -90 to 90.'),
  );

  $form['dirt_datatable_map_config']['dirt_map_center']['dirt_datatable_config_map_lon'] = array(
    '#type' => 'textfield',
    '#title' => t('Longitude'),
    '#default_value' => variable_get('dirt_datatable_config_map_lon'),
    '#weight' => 1,
    '#states' => array(
      'required' => $checkbox_checked,
    ),
    '#description' => t('Enter the longitude in the range -180 to 180.'),
  );

  $form['dirt_datatable_map_config']['dirt_map_center']['dirt_datatable_config_map_zoom'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial Map Zoom Level'),
    '#default_value' => variable_get('dirt_datatable_config_map_zoom'),
    '#weight' => 2,
    '#states' => array(
      'required' => $checkbox_checked,
    ),
    '#description' => t('Enter the initial map zoom level (number between ' .
      '0-20).<br><br>' .
      'Ideally, the zoom level should be small enough to view all ' .
      'collection sites on the map, but large enough to be able ' .
      'differentiate between site markers.<br><br>View ' .
      l(t('this page'), 'https://developers.google.com/maps/documentation/javascript/tutorial#zoom-levels',
        array('attributes' => array('target'=>'_blank'))) .
      ' for more information.'),
  );

  // Add additional validation
  $form['#validate'][] = 'dirt_datatable_duplicates_form_map_config_validate';

  return system_settings_form($form);
}


/**
 * Validation function for the map config form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_duplicates_form_map_config_validate($form, &$form_state) {
  $checkbox = $form_state['values']['dirt_datatable_config_display_map'];
  $map_api_key = $form_state['values']['dirt_datatable_config_map_api_key'];
  $lat = $form_state['values']['dirt_datatable_config_map_lat'];
  $lon = $form_state['values']['dirt_datatable_config_map_lon'];
  $zoom = $form_state['values']['dirt_datatable_config_map_zoom'];

  if ($checkbox) {
    // Map API key required if checkbox checked
    if (empty($map_api_key))
      form_set_error('dirt_datatable_config_map_api_key', t('Map API Key is required.'));

    // Lat required if checkbox checked
    if (empty($lat))
      form_set_error('dirt_datatable_config_map_lat', t('Map Center Latitude is required.'));

    // Lon required if checkbox checked
    if (empty($lon))
      form_set_error('dirt_datatable_config_map_lon', t('Map Center Longitude is required.'));

    // Zoom level required if checkbox checked
    if (empty($zoom))
      form_set_error('dirt_datatable_config_map_zoom', t('Map Zoom Level is required.'));
  }
}


/*----- Data access requests -----*/


/**
 * Callback for data access request form settings.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_data_access_config_form($form, &$form_state) {
  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);

  // Add checkbox on whether to allow data access requests via web form
  $form['access_requests_allowed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data access requests'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  $form['access_requests_allowed']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('Check the box below to allow users to request access ' .
                   'to all of the survey data (including surveys marked ' .
                   'as "semi-private") via a ') .
                 l(t('web form'), $citizen_sci_path . '/account-request-form') .
                 '. <br>' . 
                 t('If checked, a link to the form will be displayed on ' .
                   'the Data Search Portal page in the "About" section. ' .
                   '<br>Uncheck the box to remove the link from the ' .
                   'Data Search Portal page and disable the form ' .
                   'entirely.') . '<br><br>',
    '#weight' => 0,
  );

  $form['access_requests_allowed']['allow_requests_checkbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow data access requests'),
    '#description' => '',
    '#default_value' => variable_get('dirt_datatable_config_access_requests_allowed'),
    '#weight' => 1,
  );

  $requests_allowed = array(
    ':input[name="allow_requests_checkbox"]' => array(
      'checked' => TRUE,
    ),
  );

  $header = array(
    'name' => t('Full Name'),
    'email' => t('E-mail'),
    'reason' => t('Data Access Request Reason'),
    'actions' => t('Actions'),
  );

  $form['access_requests_allowed']['container'] = array(
    '#type' => 'container',
    '#states' => array(
      'visible' => $requests_allowed,
    ),
    '#weight' => 2,
  );

  $form['access_requests_allowed']['container']['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => dirt_datatable_get_data_access_requests(),
    '#empty' => t('No data access requests found.'),
    '#prefix' => '<div><h3>Current data access account requests</h3>',
    '#suffix' => '</div>',
    '#weight' => 0,
  );

  // Add checkbox on whether to send e-mail notifications
  $form['access_requests_email_notification'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail notifications'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 2,
  );

  $form['access_requests_email_notification']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('Check the box below to receive an e-mail notification ' .
                   'whenever a data access request form has been submitted.') .
                   '<br><br>',
    '#weight' => 0,
  );

  $form['access_requests_email_notification']['notification_checkbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Receive e-mail notification of data access request form submissions'),
    '#description' => '',
    '#default_value' => variable_get('dirt_datatable_config_data_access_requests_email'),
    '#weight' => 1,
  );

  $checkbox_checked = array(
    ':input[name="notification_checkbox"]' => array(
      'checked' => TRUE,
    ),
  );

  $form['access_requests_email_notification']['notify_emails'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail addresses to receive notifications of data access_requests'),
    '#description' => t('Enter multiple addresses as comma-separated values.'),
    '#states' => array(
      'required' => $checkbox_checked,
    ),
    '#default_value' => variable_get('dirt_datatable_config_notify_access_request_emails'),
    '#weight' => 2,
   );

  $form['submit']  = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#attributes' => array(
       'class' => array('form-actions', 'form-wrapper'),
    ),
    '#submit' => array('dirt_datatable_admin_access_request_submit'),
    '#weight' => 10,
  );

  // Add additional validation
  $form['#validate'][] = 'dirt_datatable_access_request_form_validate';

  return $form;
}


/**
 * Submit handler to update data access request e-mail notification preferences.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_access_request_submit(&$form, &$form_state) {
  // Update whether to allow data access requests based on checkbox value
  variable_set('dirt_datatable_config_access_requests_allowed', $form_state['values']['allow_requests_checkbox']);

  // Update whether to send notification e-mails based on checkbox value
  variable_set('dirt_datatable_config_data_access_requests_email', $form_state['values']['notification_checkbox']);

  // Update notification e-mails
  variable_set('dirt_datatable_config_notify_access_request_emails', $form_state['values']['notify_emails']);

  drupal_set_message(t('Notification preferences saved.'));
}


/**
 * Returns the rows of the table of data access requests
 * for admin review.
 *
 * @retval Array of table rows.
 */
function dirt_datatable_get_data_access_requests() {
  $rows = array();

  $requests = db_select(DIRT_DATA_ACCESS_REQUESTS_TABLE, 't')->fields('t') ->execute()->fetchAll();

  foreach($requests as $request_obj) {
    $links_markup  = '<ul class="links inline">';
    $links_markup .= '<li class="first">';
    $links_markup .= l(t('Approve account'), 'admin/config/dirt/data-search/data-access/approve/' . $request_obj->fid);
    $links_markup .= '</li><li class="last">';
    $links_markup .= l(t('Delete request'), 'admin/config/dirt/data-search/data-access/delete/' . $request_obj->fid);
    $links_markup .= '</li></ul>';

    $rows[] = array(
      'data' => array(
        array(
          'data' => check_plain($request_obj->first_name) . ' ' . check_plain($request_obj->last_name),
        ),
        array(
          'data' => check_plain($request_obj->email),
        ),
        array(
          'data' => check_plain($request_obj->reason),
        ),
        array(
          'data' => $links_markup,
        ),
      ),
    );
  }

  return $rows;
}


/**
 * Validation function for the data access request e-mail
 * notification preferences form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_access_request_form_validate($form, &$form_state) {
  $email_text = $form_state['values']['notify_emails'];
  $checkbox = $form_state['values']['notification_checkbox'];

  if ($checkbox) {
    if (empty($email_text)) {
      // E-mails field required if checkbox checked
      form_set_error('emails', t('E-mail field is required.'));
    }
    else {
      // Verify e-mails have correct format
      $emails = explode(',', $email_text);

      foreach($emails as $email) {
        $email = trim($email); // trim whitespace if present
        if (!valid_email_address($email))
          form_set_error('emails', t('Invalid e-mail address format.'));
      }
    }
  }
}


/**
 * Callback for confirm data access account approval form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_confirm_approve_data_access_account($form, &$form_state) {
  $fid = arg(6);

  if (!is_numeric($fid))  $fid = 0;

  $request = db_select(DIRT_DATA_ACCESS_REQUESTS_TABLE, 't')
    ->condition('t.fid', $fid)
    ->fields('t')
    ->execute()
    ->fetchAssoc(); // expect one result

  if (!empty($request)) {
    // Display fields and ask admin to confirm account creation.
    $header  = '<h3>' . t('Confirm account creation') . '</h3><p>';
    $header .= t('Review the data access request fields below. Click ');
    $header .= '<strong>' . t('Confirm') . '</strong> ';
    $header .= t('to approve the account.') . '</p>';

    _dirt_datatable_add_access_account_review_form_fields($request, $form, $header);

    // Add confirm submit
    $form['approve_account_submit'] = array(
      '#type' => 'submit',
      '#attributes' => array(
        'class' => array('form-actions', 'form-wrapper'),
      ),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#value' => t('Confirm'),
      '#submit' => array('dirt_datatable_create_data_access_account_submit'),
    );

    $form_state['store']['request_info'] = $request;
  }
  else {
    drupal_set_message(t('Invalid account ID. Return to ' .
      l(t('the data access request configuration page'), 'admin/config/dirt/data-search/data-access') .
      ' and select an account to create.'), 'error');
  }

  return $form;
}


/**
 * Submit handler for creating new data access account.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_create_data_access_account_submit($form, &$form_state) {
  // Check that e-mail isn't already in use by another account.
  // We do that here rather than on the account request sign-up form
  // to avoid leaking info to the public about which e-mail addresses
  // have accounts attached.
  // If the account is valid but the e-mail address is already in use,
  // a site admin will need to contact the person to ask for
  // another e-mail address, or create the Data Viewing account manually.

  // Sanitize everything before passing to various functions
  $email = check_plain($form_state['store']['request_info']['email']);
  $first_name = check_plain($form_state['store']['request_info']['first_name']);
  $last_name = check_plain($form_state['store']['request_info']['last_name']);
  $reason = check_plain($form_state['store']['request_info']['reason']);

  if (_dirt_datatable_field_already_in_users_database('mail', $email)) {
    drupal_set_message(t('Error: unable to create account because the e-mail ' .
      '@mail is already in use. Contact the person who submitted the request ' .
      'to ask for an alternate e-mail address.',
      array('@mail' => $email)), 'error');
  }
  else {
    // Create account
    $account = dirt_datatable_create_user_account($first_name, $last_name, $email, $reason);

    if (empty($account)) {
      drupal_set_message(t('An error occurred while creating the account.'), 'error');
    }
    else {
      // Notify user of new account
      // E-mail template taken from "Welcome (new user created by administrator)" on
      // Account settings page (admin/config/people/accounts).
      $params['account'] = $account;
      $language = user_preferred_language($account);
      $mail = drupal_mail('user', 'register_admin_created', $account->mail, $language, $params);

      drupal_set_message(t('Successfully created new data viewing account. View the ' .
        'new account on the ') . l(t('People page'), 'admin/people') . '.<br>' .
        t('An e-mail with login instructions was automatically sent to @mail.',
        array('@mail' => $email)));

      // Delete request from database now that it's been handled.
      $fid = $form_state['store']['request_info']['fid'];

      dirt_datatable_delete_data_access_request($fid);

      // Go back to admin page
      drupal_goto('admin/config/dirt/data-search/data-access');
    }
  }
}


/**
 * Creates a new account from input fields with the Data
 * Viewing Account role.
 *
 * @param[in] $first_name String first name.
 * @param[in] $last_name String last name.
 * @param[in] $email String e-mail address.
 * @param[in] $reason String reason for account.
 *
 * @retval Object Newly created user object (empty if unable to add user).
 */
function dirt_datatable_create_user_account($first_name, $last_name, $email, $reason) {
  $new_user = array();

  // Create a valid Drupal username from full name provided on form
  $username = _dirt_datatable_create_username_from_name_field($first_name, $last_name);

  if ($username) {
    // Assign data viewing account role (look up role ID)
    $data_viewing_rid = variable_get('dirt_data_viewing_user_role');

    $new_user = array(
      'name' => $username,
      'pass' => user_password(),
      'mail' => $email,
      'status' => 1,
      'init' => $email,
      'roles' => array(
        DRUPAL_AUTHENTICATED_RID => 'authenticated user',
        $data_viewing_rid => TRUE,
      ),
    );

    $new_user = user_save(NULL, $new_user);

    // Add rest of fields via entity metadata wrapper
    try {
      $user_wrapper = entity_metadata_wrapper('user', $new_user);
      $user_wrapper->field_dirt_user_first_name = $first_name;
      $user_wrapper->field_dirt_user_last_name = $last_name;
      $user_wrapper->field_dirt_user_account_reason = $reason;
      $user_wrapper->save();
    }
    catch (EntityMetadataWrapperException $e) {
      watchdog_exception('dirt_datatable', $e);
    }
  }
  else {
    drupal_set_message(t('Error: could not generate a valid username for ' .
      '@first @last', array('@first' => $first_name, '@last' => $last_name)), 'error');

    watchdog('dirt_datatable', 'Error: could not generate a valid username for %first %last',
      array('%first' => $first_name, '%last' => $last_name), WATCHDOG_ERROR);
  }

  return $new_user;
}


/**
 * Callback for confirm data access account request deletion form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_confirm_delete_data_access_request($form, &$form_state) {
  $fid = arg(6);

  if (!is_numeric($fid))  $fid = 0;

  $request = db_select(DIRT_DATA_ACCESS_REQUESTS_TABLE, 't')
    ->condition('t.fid', $fid)
    ->fields('t')
    ->execute()
    ->fetchAssoc(); // expect one result

  if (!empty($request)) {
    // Display fields and ask admin to confirm request deletion.
    $header  = '<h3>' . t('Confirm data access request deletion') . '</h3><p>';
    $header .= t('Review the data access request fields below. Click ');
    $header .= '<strong>' . t('Confirm') . '</strong> ';
    $header .= t('to deny the request and delete the record from the system.') . '</p>';

    _dirt_datatable_add_access_account_review_form_fields($request, $form, $header);

    // Add confirm submit
    $form['deny_account_submit'] = array(
      '#type' => 'submit',
      '#attributes' => array(
        'class' => array('form-actions', 'form-wrapper'),
      ),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#value' => t('Confirm'),
      '#submit' => array('dirt_datatable_delete_data_access_request_submit'),
    );

    $form_state['store']['request_info'] = $request;
  }
  else {
    drupal_set_message(t('Invalid account ID. Return to ' .
      l(t('the data access request configuration page'), 'admin/config/dirt/data-search/data-access') .
      ' and select an account to create.'), 'error');
  }

  return $form;
}


/**
 * Submit handler for deleting access request record.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_delete_data_access_request_submit($form, &$form_state) {
  // Get ID to delete
  $fid = $form_state['store']['request_info']['fid'];

  if (dirt_datatable_delete_data_access_request($fid))
    drupal_set_message('Data access account request record deleted.');
  else
    drupal_set_message(t('Error: unable to delete data access request record.'), 'error');

  // Go back to admin page
  drupal_goto('admin/config/dirt/data-search/data-access');
}


/**
 * Deletes a data access request form entry from the database table.
 *
 * @param[in] $fid Form ID to delete.
 *
 * @retval Boolean TRUE if deletion successful, FALSE otherwise.
 */
function dirt_datatable_delete_data_access_request($fid) {
  $success = FALSE;

  // Wrap in try/catch to not break entire site, log any errors
  try {
    db_delete(DIRT_DATA_ACCESS_REQUESTS_TABLE)->condition('fid', $fid)->execute();
    $success = TRUE;
  }
  catch (Exception $e) {
    $error_text = print_r($e, TRUE);
    watchdog('dirt_datatable', 'Error when deleting data access request with fid %fid in database: %error.',
      array('%fid' => $fid, '%error' => $error_text), WATCHDOG_ERROR);
    unset($error_text);
  }

  return $success;
}


/*----- Duplicate surveys -----*/
/**
 * Added this to DIRT survey config instead of DataTable config because
 * it fits better there.
 */

/**
 * Callback for admin duplicate survey form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_duplicate_surveys_form($form, &$form_state) {
  // Add custom CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt_datatable') . '/css/dirt_datatable_admin_page.css';

  // Add the duplicate surveys summary
  dirt_datatable_add_duplicates_summary($form);

  // Add the e-mail notification of duplicates fields
  dirt_datatable_add_duplicate_notification_fields($form);

  // Add additional validation
  $form['#validate'][] = 'dirt_datatable_duplicates_form_validate';

  return $form;
}


/**
 * Adds the e-mail notification of duplicate surveys fields
 * to the form object.
 *
 * @param[in,out] $form Form array.
 */
function dirt_datatable_add_duplicate_notification_fields(&$form) {
  // Add checkbox on whether to send e-mail notifications
  $form['duplicate_surveys_email_notification'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail notifications'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  $form['duplicate_surveys_email_notification']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('Check the box below to receive an e-mail notification ' .
                   'whenever a duplicate survey has been submitted.<br><br>'),
    '#weight' => 0,
  );

  $form['duplicate_surveys_email_notification']['checkbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Receive e-mail notification of duplicate surveys'),
    '#description' => '',
    '#default_value' => variable_get('dirt_datatable_config_send_duplicate_surveys_email'),
    '#weight' => 1,
  );

  $checkbox_checked = array(
    ':input[name="checkbox"]' => array(
      'checked' => TRUE,
    ),
  );

  $form['duplicate_surveys_email_notification']['emails'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail addresses to receive notifications of duplicate surveys'),
    '#description' => t('Enter multiple addresses as comma-separated values.'),
    '#states' => array(
      'required' => $checkbox_checked,
    ),
    '#default_value' => variable_get('dirt_datatable_config_notify_duplicate_emails'),
    '#weight' => 2,
   );

  $form['duplicate_surveys_email_notification']['submit']  = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#attributes' => array(
       'class' => array('form-actions', 'form-wrapper'),
    ),
    '#submit' => array('dirt_datatable_admin_duplicates_notification_submit'),
    '#weight' => 10,
  );
}


/**
 * Adds the duplicate surveys summary table to the form object.
 *
 * @param[in,out] $form Form array.
 */
function dirt_datatable_add_duplicates_summary(&$form) {
  // Table of duplicate surveys
  $form['dirt_datatable_duplicates'] = array(
    '#type' => 'fieldset',
    '#title' => t('Duplicate Surveys'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  $form['dirt_datatable_duplicates']['container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('table-container'),
    ),
  );

  $form['dirt_datatable_duplicates']['container']['note'] = array(
    '#type' => 'markup',
    '#markup' => t('The following tables display duplicate surveys in the ' .
                   'system, i.e., two or more surveys submitted for the same ' .
                   'collection site, of the same survey type, with the same ' .
                   'survey date month and year.<br>' .
                   'The system requires that no more than one survey per ' .
                   'type per collection site be submitted each month, as ' .
                   'surveys from the same collection site, month, and year ' .
                   'are grouped together in the data search table.<br>' .
                   'Use the tables below to examine the duplicate surveys ' .
                   'make corrections as needed. Typical causes of ' .
                   'duplicate surveys include:<br>' .
                   '<ul><li>Participant accidentally double-clicks ' .
                   '"Submit" button.</li>' .
                   '<li>Participant submits one survey at the beginning of ' .
                   'the month and the next survey at the end of the month ' .
                   'instead of waiting until the next calendar month. In ' .
                   'these cases, one of the survey dates will likely need ' .
                   'to be changed slightly so that the surveys match up with ' .
                   'the calendar months.</li>' .
                   '<li>Participant fills out a second survey with updated ' .
                   'responses instead of editing a previously submitted ' .
                   'survey. Usually the first survey may be deleted in this ' .
                   'case, but you may need to check with the participant ' .
                   'first.</li></ul>' .
                   'Note that the table below only checks for multiple ' .
                   'surveys of the same type in the same month for a ' .
                   'collection site; it does not check for surveys ' .
                   'submitted beyond the expected collection frequency ' .
                   '(e.g., three twice-a-year surveys submitted in a ' .
                   'year instead of the expected two).'),
    '#weight' => 0,
  );

  // Get survey types
  $survey_types = dirt_get_survey_types();

  // We won't check group or site description since those are one-time surveys
  $exclude = array('dirt_group_information_survey', 'dirt_site_description_survey');
  $survey_types = array_diff($survey_types, $exclude);

  // Initialize table header for each table
  $header = array(
    t('Username'),
    t('Site ID'),
    t('Number of Duplicate Surveys'),
    t('Survey #s and Links'),
    t('Survey Year'),
    t('Survey Month'),
  );

  foreach($survey_types as $idx => $type) {
    // Set up table container
    $form['dirt_datatable_duplicates']['container'][$type . '_table_container'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('table-container'),
      ),
      '#weight' => $idx,
    );

    // Survey type header
    // Get human-readable type name
    $name = dirt_get_name_of_survey_type($type);
    $form['dirt_datatable_duplicates']['container'][$type . '_table_container']['header'] = array(
      '#type' => 'markup',
      '#markup' => t('<h5>Duplicate Surveys of Type: @type</h5>', array('@type' => $name)),
      '#weight' => 0,
    );

    // Table of duplicates
    $form['dirt_datatable_duplicates']['container'][$type . '_table_container']['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => dirt_datatable_get_duplicate_survey_table_rows_for_type($type),
      '#prefix' => '<div class="dirt-datatable-summary-table">',
      '#suffix' => '</div>',
      '#weight' => 1,
      '#empty' => t('None found!'),
    );
  }
}


/**
 * Validation function for the duplicate survey e-mail notification
 * preferences form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_duplicates_form_validate($form, &$form_state) {
  $email_text = $form_state['values']['emails'];
  $checkbox = $form_state['values']['checkbox'];

  if ($checkbox) {
    if (empty($email_text)) {
      // E-mails field required if checkbox checked
      form_set_error('emails', t('E-mail field is required.'));
    }
    else {
      // Verify e-mails have correct format
      $emails = explode(',', $email_text);

      foreach($emails as $email) {
        $email = trim($email); // trim whitespace if present
        if (!valid_email_address($email))
          form_set_error('emails', t('Invalid e-mail address format.'));
      }
    }
  }
}


/**
 * Submit handler to update duplicate e-mail notification preferences.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_duplicates_notification_submit(&$form, &$form_state) {
  // Update whether to send notification e-mails based on checkbox value
  variable_set('dirt_datatable_config_send_duplicate_surveys_email', $form_state['values']['checkbox']);

  // Update notification e-mails
  variable_set('dirt_datatable_config_notify_duplicate_emails', $form_state['values']['emails']);

  drupal_set_message(t('Notification preferences saved.'));
}


/**
 * Returns table rows for duplicate survey table of type passed in as input.
 *
 * @param[in] $type String machine name of survey content type.
 *
 * @retval Array of table rows.
 */
function dirt_datatable_get_duplicate_survey_table_rows_for_type($type) {
  module_load_include('inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  // Initialize return
  $rows = array();

  $results = dirt_datatable_get_duplicate_surveys_in_data_table_for_type($type);

  foreach ($results as $result_obj) {
    // Get markup for nids
    $nid_markup = '';
    $nid_array = explode(', ', $result_obj->nid_list);
    foreach ($nid_array as $nid) {
      // Add link to node
      $nid_markup .= l('#' . $nid, 'node/' . $nid) . ' ';
    }

    // Build row
    $rows[] = array(
      'data' => array(
        0 => array(
          'data' => $result_obj->username,
          'class' => array('dirt-datatable-summary-table-cell'),
        ),
        1 => array(
          'data' => $result_obj->site_id,
          'class' => array('dirt-datatable-summary-table-cell', 'site-id'),
        ),
        2 => array(
          'data' => $result_obj->nid_count,
          'class' => array('dirt-datatable-summary-table-cell', 'nid-count'),
        ),
        3 => array(
          'data' => $nid_markup,
          'class' => array('dirt-datatable-summary-table-cell', 'nid-links'),
        ),
        4 => array(
          'data' => $result_obj->survey_year,
          'class' => array('dirt-datatable-summary-table-cell', 'year'),
        ),
        5 => array(
          'data' => $result_obj->survey_month,
          'class' => array('dirt-datatable-summary-table-cell', 'month'),
        ),
      ),
      'class' => array('dirt-datatable-summary-table-row')
    );
  }

  return $rows;
}


/*----- Search table groups config -----*/

/**
 * Callback for admin data search table filters and groups
 * settings form.
 *
 * Sets up a table with one row per DataTable column group and
 * options to edit the column group header label, change its
 * filter group, or re-order the column groups in the table.
 *
 * Modeled after block_admin_display_form() in Block module.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_table_group_settings_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  // Attach CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt_datatable') . '/css/dirt_datatable_admin_page.css';

  // These are already sorted by weight
  $filter_groups = dirt_datatable_get_filter_groups();
  $column_groups = dirt_datatable_get_table_column_groups();

  // Create array of filter group ID => label for filter group select
  $filter_group_options = array();
  // Lookup array to store survey type of each filter group (indexed by fid)
  $survey_type_of = array();
  foreach($filter_groups as $group_info) {
    $label = $group_info->label;
    if ($label) {
      $filter_group_options[$group_info->fid] = $label;
      $survey_type_of[$group_info->fid] = $group_info->survey_type;
    }
  }

  $form['edit_column_groups_header'] = array(
    '#type' => 'markup',
    '#markup' => t('Table Column Groups'),
    '#prefix' => '<h3 class="dirt-datatable-draggable-table-header">',
    '#suffix' => '</h3>',
  );

  $form['filter_groups_value'] = array(
    '#type' => 'value',
    '#value' => $filter_group_options,
  );

  $form['column_groups'] = array();
  $form['#tree'] = TRUE;
  
  // Since initial weights began with 1, use count rather than count / 2
  // Then add some wiggle room
  $weight_delta = count($column_groups) + 10;

  // Pass weight delta to submit
  $form['weight_delta'] = array(
    '#type' => 'value',
    '#value' => $weight_delta,
  );

  $exclude_gids = array();
  // Remove common group - should not be altered in any way
  $common_gid = variable_get('dirt_datatable_colgroup_gid_common', 0);
  if ($common_gid) $exclude_gids[] = $common_gid;
  // Also remove empty group - nothing to do with it
  $empty_gid = variable_get('dirt_datatable_colgroup_gid_empty', 0);
  if ($empty_gid) $exclude_gids[] = $empty_gid;

  // Build the form elements for each column group: label, weight, filter group
  foreach($column_groups as $idx => $group_info) {
    if (!in_array($group_info->gid, $exclude_gids)) {
      $label = $group_info->label;

      // Pass along group ID (gid) for db update in submit handler.
      $form['column_groups'][$idx]['gid'] = array(
        '#type' => 'value',
        '#value' => $group_info->gid,
      );

      // Create text field for label
      $form['column_groups'][$idx]['label'] = array(
        '#type' => 'textfield',
        '#default_value' => $label,
        '#size' => 60,
        '#maxlength' => 255,
        '#title_display' => 'invisible',
        '#title' => t('Label for ') . $label,
      );

      // Weight field
      $form['column_groups'][$idx]['weight'] = array(
        '#type' => 'weight',
        '#delta' => $weight_delta,
        '#default_value' => $group_info->weight,
        '#title_display' => 'invisible',
        '#title' => t('Weight for ') . $label,
      );

      // Filter group select
      // Limit select options to filter groups of same survey type.
      $filter_group_select_options = array();
      foreach($filter_group_options as $fid => $label) {
        $filter_survey_type = $survey_type_of[$fid];
        $group_survey_type = $group_info->survey_type;
        if ($filter_survey_type === $group_survey_type)
          $filter_group_select_options[$fid] = $label;
      }
      $form['column_groups'][$idx]['filter_group'] = array(
        '#type' => 'select',
        '#options' => $filter_group_select_options,
        '#required' => FALSE,
        '#empty_value' => NULL,
        '#empty_option' => t('- None -'),
        '#title_display' => 'invisible',
        '#title' => t('Filter group for ') . $label,
        '#default_value' => $group_info->filter_group,    
      );
    }
  }

  // Add submit handler for column groups
  $form['save_column_group_info'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => 'Save column groups',
    '#submit' => array('dirt_datatable_admin_search_table_group_settings_form_submit'),
    '#weight' => 1,
  );

  $form['filter_groups'] = array();

  // Since initial weights began with 1, use count rather than count / 2
  // Then add some wiggle room
  $weight_delta = count($filter_groups) + 10;

  $form['edit_filter_groups_header'] = array(
    '#type' => 'markup',
    '#markup' => t('Variable Filter Groups'),
    '#prefix' => '<h3 class="dirt-datatable-draggable-table-header">',
    '#suffix' => '</h3>',
  );

  // Build the form elements for each filter group: label, weight
  foreach($filter_groups as $idx => $group_info) {
    $label = $group_info->label;

    // Pass along filter group ID (fid) for db update in submit handler.
    $form['filter_groups'][$idx]['fid'] = array(
      '#type' => 'value',
      '#value' => $group_info->fid,
    );

    // Create text field for label
    $form['filter_groups'][$idx]['label'] = array(
      '#type' => 'textfield',
      '#default_value' => $label,
      '#size' => 60,
      '#maxlength' => 255,
      '#title_display' => 'invisible',
      '#title' => t('Label for ') . $label,
    );

    // Weight field
    $form['filter_groups'][$idx]['weight'] = array(
      '#type' => 'weight',
      '#delta' => $weight_delta,
      '#default_value' => $group_info->weight,
      '#title_display' => 'invisible',
      '#title' => t('Weight for ') . $label,
    );
  }

  // Add submit handler for filter groups
  $form['save_filter_group_info'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => 'Save filter groups',
    '#submit' => array('dirt_datatable_admin_search_filter_group_settings_form_submit'),
  );

  // Next field: option to add new column group.
  $form['add_column_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create New Column Group'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('To create a new column group, fill out the form ' .
      'fields below, then click the "Add new column group" button. Use the ') .
      l(t('Data Search Portal table columns configuration page'), 'admin/config/dirt/data-search/table-columns') .
      t(' to add columns to any newly created column groups.'),
    '#weight' => 1,
  );

  $form['add_column_group']['column_group_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Column group label'),

    '#description' => t('Enter the column group label to be displayed in the ' .
      'Data Search Portal table.'),
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 0,
  );

  $survey_types = dirt_get_survey_types();
  $survey_type_options = array();
  foreach($survey_types as $machine_name) {
    $survey_type_options[$machine_name] = dirt_get_name_of_survey_type($machine_name);
  }

  $form['add_column_group']['survey_type'] = array(
    '#type' => 'select',
    '#options' => $survey_type_options,
    '#title' => t('Survey type'),
    '#empty_value' => '',
    '#empty_option' => t('- Select survey type -'),
    '#description' => t('Select the survey type that all column fields in ' .
      'this column group will belong to.'),
    '#ajax' => array(
      'callback' => 'dirt_datatable_survey_type_form_field_callback',
      'wrapper' => 'add-column-group-to-filter-group',
      'method' => 'replace',
    ),
    '#weight' => 1,
  );

  // Filter group select (optional)
  // Limit options to filter groups of same survey type
  $survey_type_selected = (array_key_exists('values', $form_state)) ? $form_state['values']['add_column_group']['survey_type'] : '';
  $filter_group_select_options = array();
  foreach($filter_group_options as $fid => $label) {
    $filter_survey_type = $survey_type_of[$fid];
    if ($filter_survey_type === $survey_type_selected)
      $filter_group_select_options[$fid] = $label;
  }

  $form['add_column_group']['filter_group'] = array(
    '#type' => 'select',
    '#title' => t('Filter group'),
    '#required' => FALSE,
    '#prefix' => '<div id="add-column-group-to-filter-group">',
    '#suffix' => '</div>',
    '#empty_value' => '',
    '#empty_option' => '- None -',
    '#options' => $filter_group_select_options,
    '#description' => t('Optionally add this column group to a variable ' .
      'filter group. Available filter groups will depend on survey type ' .
      'selection, since column group survey type and filter group ' .
      'survey type must match.'),
    '#weight' => 2,
  );

  $form['add_column_group']['save_new_column_group'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => 'Add new column group',
    '#submit' => array('dirt_datatable_admin_add_new_column_group_form_submit'),
    '#weight' => 3,
  );

  // Next field: option to delete a column group.
  // All columns belonging to a deleted group will be
  // re-assigned to the "empty" group and will need to 
  // be manually moved to another group by an admin.

  $form['delete_column_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delete a Column Group'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Use the select field below to choose the column ' .
      'group to delete, then click the "Delete column group" button. ' .
      'The column group options include their unique column group IDs ' .
      'to help distinguish between groups with identical labels ' . 
      '(if applicable).') . '<br>' . t('Columns ' .
      'that belonged to the deleted group will no longer belong to any ' .
      'column group and will <strong>not be displayed</strong> in the Data ' .
      'Search Portal table unless they are manually added to a different ' .
      'column group (or groups). Use the ') .
      l(t('Data Search Portal table columns configuration page'), 'admin/config/dirt/data-search/table-columns') .
      t(' to manually add columns to column groups.'),
    '#weight' => 2,
  );

  // Build column group select options for select field.
  $column_group_select_options = array();
  foreach($column_groups as $idx => $group_info) {
    $gid = $group_info->gid;

    if ($group_info->label) {
      // Add the group ID (gid) in parentheses in case labels are not unique
      // (They should be, though...)
      $label = $group_info->label . ' (' . $gid . ')';
      $column_group_select_options[$gid] = $label;
    }
  }

  $form['delete_column_group']['group_to_delete'] = array(
    '#type' => 'select',
    '#options' => $column_group_select_options,
    '#title' => t('Column group'),
    '#empty_value' => '',
    '#empty_option' => t('- Select column group -'),
    '#weight' => 1,
  );

  $form['delete_column_group']['delete_column_group_submit'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => t('Delete column group'),
    '#submit' => array('dirt_datatable_admin_delete_column_group_form_submit'),
    '#weight' => 2,
  );

  // Next field: option to add new filter group.
  $form['add_filter_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create New Variable Filter Group'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('To create a new variable filter group, fill out the ' .
      'form fields below, then click the "Add new filter group" button. The ' .
      'new filter group will then be available to assign column groups to ' .
      'in the table at the top of this page.'),
   '#weight' => 3,
  );

  $form['add_filter_group']['filter_group_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Filter group label'),
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('Enter the filter group label to be displayed in the ' .
      'Data Search Portal table filters section.'),
    '#weight' => 0,
  );

  $form['add_filter_group']['survey_type'] = array(
    '#type' => 'select',
    '#options' => $survey_type_options,
    '#title' => t('Survey type'),
    '#empty_value' => '',
    '#empty_option' => t('- Select survey type -'),
    '#description' => t('Select the survey type that all column fields in ' .
      'this filter group will belong to.'),
    '#weight' => 1,
  );

  $form['add_filter_group']['add_filter_group_submit'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => 'Add new filter group',
    '#submit' => array('dirt_datatable_admin_add_new_filter_group_form_submit'),
    '#weight' => 2,
  );

  // Next field: option to delete a filter group.

  $form['delete_filter_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delete a Filter Group'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Use the select field below to choose the ' .
      'filter group to delete, then click the "Delete filter group" ' .
      'button. The filter group options include their unique filter ' .
      'group IDs in parentheses.'),
    '#weight' => 4,
  );

  $form['delete_filter_group']['filter_group_to_delete'] = array(
    '#type' => 'select',
    '#options' => $filter_group_options,
    '#title' => t('Filter group'),
    '#empty_value' => '',
    '#empty_option' => t('- Select filter group -'),
    '#weight' => 1,
  );

  $form['delete_filter_group']['delete_filter_group_submit'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => t('Delete filter group'),
    '#submit' => array('dirt_datatable_admin_delete_filter_group_form_submit'),
    '#weight' => 2,
  );


  $form['#validate'][] = 'dirt_datatable_admin_search_table_group_settings_form_validate';

  return $form; 
}


/**
 * Ajax callback to force form rebuild to populate filter group options
 * based on survey type selected.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_survey_type_form_field_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['add_column_group']['filter_group'];
}


/**
 * Preprocess search table column group settings form.
 */
function template_preprocess_dirt_datatable_admin_search_table_group_settings_form(&$variables) {
  // Column groups table
  $variables['filter_groups'] = $variables['form']['filter_groups_value']['#value'];
  $variables['edit_column_groups_header'] = drupal_render($variables['form']['edit_column_groups_header']);
  $variables['group_list'] = array();

  foreach(element_children($variables['form']['column_groups']) as $idx) {
    $column_group = &$variables['form']['column_groups'][$idx];
    $column_group['weight']['#attributes']['class'] = array('column-group-weight');
    $variables['group_list'][$idx] = new stdClass();
    $variables['group_list'][$idx]->row_class = !empty($column_group['#attributes']['class']) ? implode(' ', $column_group['#attributes']['class']) : '';
    $variables['group_list'][$idx]->label = drupal_render($column_group['label']);
    $variables['group_list'][$idx]->filter_group_select = drupal_render($column_group['filter_group']);
    $variables['group_list'][$idx]->weight_select = drupal_render($column_group['weight']);
  }
  $variables['group_form_submit'] = drupal_render($variables['form']['save_column_group_info']);

  // Filter groups table
  $variables['filter_group_list'] = array();
  $variables['edit_filter_groups_header'] = drupal_render($variables['form']['edit_filter_groups_header']);

  foreach(element_children($variables['form']['filter_groups']) as $idx) {
    $filter_group = &$variables['form']['filter_groups'][$idx];
    $filter_group['weight']['#attributes']['class'] = array('filter-group-weight');
    $variables['filter_group_list'][$idx] = new stdClass();
    $variables['filter_group_list'][$idx]->row_class = !empty($filter_group['#attributes']['class']) ? implode(' ', $filter_group['#attributes']['class']) : '';
    $variables['filter_group_list'][$idx]->label = drupal_render($filter_group['label']);
    $variables['filter_group_list'][$idx]->weight_select = drupal_render($filter_group['weight']);
  }
  $variables['filter_group_form_submit'] = drupal_render($variables['form']['save_filter_group_info']);

  $variables['form_submit'] = drupal_render_children($variables['form']);  
}


/**
 * Validation handler for search table group settings form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_table_group_settings_form_validate($form, &$form_state) {
  // Multiple submits so check op value
  $op = (array_key_exists('op', $form_state['values'])) ? $form_state['values']['op'] : '';
  switch ($op) {
    case 'Add new column group':
      // Label required
      if (empty($form_state['values']['add_column_group']['column_group_label']))
        form_set_error('add_column_group][column_group_label', t('Column group label required.'));

      // Survey type required
      if (!dirt_node_type_is_survey_type($form_state['values']['add_column_group']['survey_type']))
        form_set_error('add_column_group][survey_type', t('Survey type required.'));

      break;

    case 'Delete column group':
      // Column group to delete required
      if (empty($form_state['values']['delete_column_group']['group_to_delete']))
        form_set_error('delete_column_group][group_to_delete', t('Select a column group.'));
      break;

    case 'Add new filter group':
      // Label required
      if (empty($form_state['values']['add_filter_group']['filter_group_label']))
        form_set_error('add_filter_group][filter_group_label', t('Filter group label required.'));
 
      // Survey type required
      if (!dirt_node_type_is_survey_type($form_state['values']['add_filter_group']['survey_type']))
        form_set_error('add_filter_group][survey_type', t('Survey type required.'));

      break;

    case 'Delete filter group':
      // Filter group to delete required
      if (empty($form_state['values']['delete_filter_group']['filter_group_to_delete']))
        form_set_error('delete_filter_group][filter_group_to_delete', t('Select a filter group.'));
      break;

    case 'Save column groups':
      // Require weights to be unique, or search table may get messed up.
      _dirt_datatable_check_unique_row_weights('column_groups', $form_state['values']['column_groups'], $form, $form_state);
      break;

    case 'Save filter groups':
      // Require weights to be unique, or data search portal filters won't display
      // in a predictable manner.
      _dirt_datatable_check_unique_row_weights('filter_groups', $form_state['values']['filter_groups'], $form, $form_state);
     break;
  }
}


/**
 * Submit handler for search table group settings form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_table_group_settings_form_submit($form, &$form_state) {
  // Need to update the database table from the form values.
  $success = TRUE;

  foreach($form_state['values']['column_groups'] as $idx => $info_arr) {
    // Attempt table update.
    // Updatable columns: label, weight, filter_group
    $fields = array();

    if (array_key_exists('label', $info_arr))
      $fields['label'] = $info_arr['label'];

    // Weight must be integer
    if (array_key_exists('weight', $info_arr) && is_numeric($info_arr['weight']))
      $fields['weight'] = $info_arr['weight'];

    // Filter group must be integer
    if (array_key_exists('filter_group', $info_arr) && is_numeric($info_arr['filter_group']))
      $fields['filter_group'] = $info_arr['filter_group'];

    // Update DataTable column group
    $conditions = array(0 => array(
      'field' => 'gid',
      'value' => $info_arr['gid'],
      'operator' => '=')
    );

    $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_COLUMN_GROUPS, $fields, $conditions);
    if (is_null($updated)) $success = FALSE;
  }

  // Additionally, reset weight of common and empty groups to keep
  // common group first and empty group last.
  $weight_delta = $form_state['values']['weight_delta'];
  $min_weight = -1 - (int)$weight_delta;
  $max_weight = (int)$weight_delta + 1;

  $groups = array();
  $common_gid = variable_get('dirt_datatable_colgroup_gid_common', 0);
  $empty_gid = variable_get('dirt_datatable_colgroup_gid_empty', 0);
  if ($common_gid) {
    $groups[] = array(
      'gid' => $common_gid,
      'weight' => $min_weight,
    );
  }
  if ($empty_gid) {
    $groups[] = array(
      'gid' => $empty_gid,
      'weight' => $max_weight,
    );
  }

  foreach($groups as $info_arr) {
    $fields = array('weight' => $info_arr['weight']);

    $conditions = array(0 => array(
      'field' => 'gid',
      'value' => $info_arr['gid'],
      'operator' => '=')
    );

    $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_COLUMN_GROUPS, $fields, $conditions);
    if (is_null($updated)) $success = FALSE;
  }


  if ($success) {
    drupal_set_message(t('Updated Data Search Portal table column groups.'));
  }
  else {
    drupal_set_message(t('An error occurred while attempting to update Data ' .
      'Search Portal table column groups. See ') . 
      l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/**
 * Submit handler for search table group settings form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_filter_group_settings_form_submit($form, &$form_state) {
  // Need to update the database table from the form values.
  $success = TRUE;

  foreach($form_state['values']['filter_groups'] as $idx => $info_arr) {
    // Attempt table update.

    // Updatable columns: label, weight
    $fields = array();

    if (array_key_exists('label', $info_arr))
      $fields['label'] = $info_arr['label'];

    if (array_key_exists('weight', $info_arr))
      $fields['weight'] = $info_arr['weight'];

    // Update DataTable column group
    $conditions = array(0 => array(
      'field' => 'fid',
      'value' => $info_arr['fid'],
      'operator' => '=')
    );

    $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_FILTER_GROUPS, $fields, $conditions);
    if (is_null($updated)) $success = FALSE;
  }

  if ($success) {
    drupal_set_message(t('Updated Data Search Portal variable filter groups.'));
  }
  else {
    drupal_set_message(t('An error occurred while attempting to update Data ' .
      'Search Portal variable filter groups. See ') .
      l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/**
 * Submit handler for add new column group form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_add_new_column_group_form_submit($form, &$form_state) {
  // Initialize array of fields to add new column group column to
  // colgroups database table.
  $fields = array();

  // Set label and survey type.
  $fields['label'] = $form_state['values']['add_column_group']['column_group_label'];
  $fields['survey_type'] = $form_state['values']['add_column_group']['survey_type'];

  // Set filter group, if selected
  $filter_group = $form_state['values']['add_column_group']['filter_group'];
  if ($filter_group !== '')
    $fields['filter_group'] = $filter_group;

  // Set weight to be at end of list of column groups
  $weight = dirt_datatable_get_largest_weight_value(DIRT_SEARCH_TABLE_COLUMN_GROUPS);
  $fields['weight'] = $weight + 1;

  // Insert new group into table
  try {
    dirt_insert_into_database_table(DIRT_SEARCH_TABLE_COLUMN_GROUPS, $fields);

    drupal_set_message(t('Added new column group @name. Use the ', array('@name' => $fields['label'])) .
      l(t('Data Search Portal table columns configuration page'), 'admin/config/dirt/data-search/table-columns') .
      ' to add columns to this column group. The new column group will not show up ' .
      'in the Data Search Portal table until at least one column has been added to it.');
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while attempting to add new column group. See ') . 
      l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/**
 * Submit handler for delete column group form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_delete_column_group_form_submit($form, &$form_state) {
  try {
    $gid_to_delete = $form_state['values']['delete_column_group']['group_to_delete'];

    // Need to get all columns belonging to this group and re-assign to "empty" group.
    $empty_gid = variable_get('dirt_datatable_colgroup_gid_empty');
    $cids = dirt_datatable_get_columns_in_column_group($gid_to_delete);

    // Update database table
    $fields = array('colgroup' => $empty_gid);
    $conditions = array();
    $conditions[0] = array(
      'field' => 'cid',
      'value' => $cids,
      'operator' => 'IN',
    );

    $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_COLUMNS, $fields, $conditions);
    watchdog('dirt_datatable', 'Moved %num columns from column group #%gid to the empty group.',
      array('%num' => $updated, '%gid' => $gid_to_delete), WATCHDOG_INFO);

    $conditions = array();
    $conditions[0] = array(
      'field' => 'gid',
      'value' => $gid_to_delete,
      'operator' => '=',
    );

    dirt_delete_from_database_table(DIRT_SEARCH_TABLE_COLUMN_GROUPS, $conditions);

    drupal_set_message(t('Deleted column group. Use the ') .
      l(t('Data Search Portal table columns configuration page'), 'admin/config/dirt/data-search/table-columns') .
      t(' to add the columns that used to be in this group to another group.'));
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while attempting to delete the column group.'), 'error');
  }
}


/**
 * Submit handler for add new filter group form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_add_new_filter_group_form_submit($form, &$form_state) {
  // Initialize array of fields to add new filter group to
  // filter_groups database table.
  $fields = array();

  // Set label and survey type.
  $label = $form_state['values']['add_filter_group']['filter_group_label'];
  $fields['label'] = $label;
  $fields['survey_type'] = $form_state['values']['add_filter_group']['survey_type'];

  // Set filter group element ID from label
  // First replace spaces with dashes
  $element_id = str_replace(' ', '-', $label);
  // Then remove all non alphabetical/dash characters
  $element_id = preg_replace("/[^A-Za-z-]/", '', $element_id);
  // Then truncate to 128 chars
  $element_id = substr($element_id, 0, 128);
  // There's a tiny chance that the resulting class name will not be
  // unique, but really the labels shouldn't be that similar to each
  // other.
  $fields['element_id'] = $element_id;

  // Set weight to be at end of list of filter groups
  $weight = dirt_datatable_get_largest_weight_value(DIRT_SEARCH_TABLE_FILTER_GROUPS);
  $fields['weight'] = $weight + 1;

  // Insert new group into table
  try {
    dirt_insert_into_database_table(DIRT_SEARCH_TABLE_FILTER_GROUPS, $fields);

    drupal_set_message(t('Added new filter group @name.', array('@name' => $fields['label'])));
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while attempting to add new filter group. See ') .
      l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/**
 * Submit handler for delete filter group form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_delete_filter_group_form_submit($form, &$form_state) {
  try {
    $fid_to_delete = $form_state['values']['delete_filter_group']['filter_group_to_delete'];

    // Need to get all columns belonging to this group and assign filter group 0
    $gids = dirt_datatable_get_column_groups_in_filter_group($fid_to_delete);

    // Update database table if necessary
    if (!empty($gids)) {
      $fields = array('filter_group' => 0);
      $conditions = array();
      $conditions[0] = array(
        'field' => 'gid',
        'value' => $gids,
        'operator' => 'IN',
      );
      $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_COLUMN_GROUPS, $fields, $conditions);

      watchdog('dirt_datatable', 'Removed %num column groups from filter group #%fid.',
        array('%num' => $updated, '%fid' => $fid_to_delete), WATCHDOG_INFO);
    }

    $conditions = array();
    $conditions[0] = array(
      'field' => 'fid',
      'value' => $fid_to_delete,
      'operator' => '=',
    );

    dirt_delete_from_database_table(DIRT_SEARCH_TABLE_FILTER_GROUPS, $conditions);

    drupal_set_message(t('Deleted filter group.'));
  }
  catch (Exception $e) {
    drupal_set_message(t('An error occurred while attempting to delete the filter group.'), 'error');
  }
}


/*----- Search table columns config -----*/


/**
 * Callback for admin data search table column settings form.
 *
 * Sets up a table with one row per DataTable column and
 * options to edit DataTable column header label, change
 * column group, alter column visibility in table, or 
 *  re-order columns in table.
 *
 * Modeled after block_admin_display_form() in Block module.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_table_column_settings_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  // Attach CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt_datatable') . '/css/dirt_datatable_admin_page.css';

  // These are already sorted by weight
  $column_groups = dirt_datatable_get_table_column_groups(); 

  // Create array of group ID => label for select dropdown
  $column_group_options = array();
  // Lookup array to store survey type of each column group (indexed by gid)
  $survey_type_of = array();

  foreach($column_groups as $group_info) {
    $label = $group_info->label;
    if ($label) {
      $column_group_options[$group_info->gid] = $label;
      $survey_type_of[$group_info->gid] = $group_info->survey_type;
    }
  }
  // Add empty option
  $empty_gid = variable_get('dirt_datatable_colgroup_gid_empty', 0);
  $column_group_options[$empty_gid] = t('No column group assigned');
  $survey_type_of[$empty_gid] = '';

  $visibility_options = array(
    'visible' => t('Visible'),
    'admin' => t('Admin-only'),
    'hidden' => t('Hidden'),
  );

  $form['column_groups'] = array(
    '#type' => 'value',
    '#value' => $column_group_options,
  );

  $form['columns'] = array();
  $form['#tree'] = TRUE;

  // Get columns in DataTable (already sorted by group and weight
  // within each group)
  $tablecols = dirt_datatable_get_table_column_metadata();

  // Since initial weights began with 1, use count rather than count / 2
  // then add some wiggle room
  $weight_delta = count($tablecols) + 10;

  // Pass weight delta to submit
  $form['weight_delta'] = array(
    '#type' => 'value',
    '#value' => $weight_delta,
  );

  // These columns may not be altered (label, group, or visibility)
  // Can't alter label:
  $label_lock_cols = array('username', 'nids', 'survey_year_month');
  // Can't alter group:
  $group_lock_cols = array('username', 'nids', 'dirt_user_first_name',
    'dirt_user_last_name', 'survey_year_month', 'dirt_site_habitat_type',
    'dirt_site_cropping_system', 'dirt_site_type_of_crop', 'dirt_site_geo_coords_lat',
    'dirt_site_geo_coords_lon', 'dirt_user_collection_state', 'survey_year',
    'survey_month', 'survey_month_abbr');
  // Can't alter visibility:
  $visibility_lock_cols = array('username', 'survey_year_month');

  // Don't even display these columns: they can't be altered and are
  // used in aggregation or to implement table filters. It might be
  // confusing to display them as an option.
  $skip_cols = array('nid', 'row_types', 'dirt_user_sharing_perms', 'include', 'site_id', 'survey_type');

  // Build the form elements for each column: label, weight, group, visibility
  foreach($tablecols as $idx => $col_info) {
    $column_name = $col_info->column_name;
   
    if (!in_array($column_name, $skip_cols)) { 
      $label = $col_info->label;

      // Pass along column ID (cid) and class for db update in submit handler.
      $form['columns'][$idx]['cid'] = array(
        '#type' => 'value',
        '#value' => $col_info->cid,
      );
      $form['columns'][$idx]['class'] = array(
        '#type' => 'value',
        '#value' => $col_info->class,
      );

      // Create text field if editable, markup if not
      $form['columns'][$idx]['label'] = array(
        '#title_display' => 'invisible',
        '#title' => t('Label for ') . $label,
      );

      if (in_array($column_name, $label_lock_cols)) {
        $form['columns'][$idx]['label']['#type'] = 'markup';
        $form['columns'][$idx]['label']['#markup'] = $label;
      }
      else {
        $form['columns'][$idx]['label']['#type'] = 'textfield';
        $form['columns'][$idx]['label']['#default_value'] = $label;
        $form['columns'][$idx]['label']['#size'] = 60;
        $form['columns'][$idx]['label']['#maxlength'] = 255;
      }

      // For weight, everything can be changed except first (site_id)
      $form['columns'][$idx]['weight'] = array(
        '#default_value' => $col_info->weight,
        '#title_display' => 'invisible',
        '#title' => t('Weight for ') . $label,
      );
      if ($column_name === 'site_id') {
        $form['columns'][$idx]['weight']['#type'] = 'markup';
        $form['columns'][$idx]['weight']['#markup'] = 'First column (fixed)';
      }
      else {
        $form['columns'][$idx]['weight']['#type'] = 'weight';
        $form['columns'][$idx]['weight']['#delta'] = $weight_delta;
      }
 
      // Create column group drop-down
      $empty_gid = variable_get('dirt_datatable_colgroup_gid_empty');
      $default_gid = ($col_info->colgroup != $empty_gid) ? $col_info->colgroup : $empty_gid;
   
      $form['columns'][$idx]['group_locked'] = array(
        '#type' => 'value',
        '#value' => FALSE, // assume not locked but update later if so
      );

      // Set up column group options for select field
      $group_select_options = array();

      if (in_array($column_name, $group_lock_cols)) {
        $group_select_options[$col_info->colgroup] = $column_group_options[$col_info->colgroup];
      }
      else {
        // Loop through group options and only include those that match the
        // survey type of the column field as possible groups in the drop-down.
        foreach($column_group_options as $gid => $label) {
          $group_survey_type = $survey_type_of[$gid];
          $column_survey_type = $col_info->survey_type;
          if ($group_survey_type === $column_survey_type)
            $group_select_options[$gid] = $label;
  
          // Remove some options:
          // Should not place any column in "empty" column group:
          unset($group_select_options[$empty_gid]);

          // Should not place any column in common group that isn't
          // already there (at least as of now,
          // current implementation doesn't support adding common columns.
          $common_gid = variable_get('dirt_datatable_colgroup_gid_common', 0);
          if ($common_gid) unset($group_select_options[$common_gid]);

          // Also remove geo coords because it doesn't make sense to put anything
          // else in this group.
          $geo_coords_gid = variable_get('dirt_datatable_colgroup_gid_geo_coords', 0);
          if ($geo_coords_gid) unset($group_select_options[$geo_coords_gid]);
        }
      }

      $form['columns'][$idx]['column_group'] = array(
        '#type' => 'select',
        '#options' => $group_select_options,
        '#title_display' => 'invisible',
        '#title' => t('Column group for ') . $label,
        '#default_value' => $default_gid,
      );

      // Set default visibility option
      $default_visibility = 'visible';
      if ($col_info->admin_only)
        $default_visibility = 'admin';
      else
        $default_visibility = ($col_info->visible) ? 'visible' : 'hidden';

      // Create visibility as drop-down if editable, markup if not
      $form['columns'][$idx]['visibility'] = array(
        '#default_value' => $default_visibility,
        '#title_display' => 'invisible',
        '#title' => t('Visibility for ') . $label,
      );
      if (in_array($column_name, $visibility_lock_cols)) {
        $form['columns'][$idx]['visibility']['#type'] = 'markup';
        $form['columns'][$idx]['visibility']['#markup'] = $visibility_options[$default_visibility];
      }
      else {
        $form['columns'][$idx]['visibility']['#type'] = 'select';
        $form['columns'][$idx]['visibility']['#options'] = $visibility_options;
      }
    }
  }

  $form['actions'] = array(
    '#tree' => FALSE,
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save table configuration'),
  );

  $form['#validate'][] = 'dirt_datatable_admin_search_table_column_settings_form_validate';

  return $form;
}


/**
 * Preprocess search table column settings form.
 */
function template_preprocess_dirt_datatable_admin_search_table_column_settings_form(&$variables) {
  $variables['column_groups'] = $variables['form']['column_groups']['#value'];

  foreach($variables['column_groups'] as $gid => $label) {
    $variables['column_list'][$gid] = array();
  }

  foreach(element_children($variables['form']['columns']) as $idx) {
    $column = &$variables['form']['columns'][$idx];
    $column_group = (isset($column['column_group']['#default_value'])) ? $column['column_group']['#default_value'] : variable_get('dirt_datatable_colgroup_gid_empty');

    $column['column_group']['#attributes']['class'] = array('column-group-select', 'column-group-' . $column_group);
    $column['weight']['#attributes']['class'] = array('column-weight', 'column-weight-' . $column_group);

    $variables['column_list'][$column_group][$idx] = new stdClass();
    $variables['column_list'][$column_group][$idx]->row_class = !empty($column['#attributes']['class']) ? implode(' ', $column['#attributes']['class']) : '';
    $variables['column_list'][$column_group][$idx]->column_modified = !empty($column['#attributes']['class']) && in_array('column-modified', $column['#attributes']['class']);
    $variables['column_list'][$column_group][$idx]->label = drupal_render($column['label']);
    $variables['column_list'][$column_group][$idx]->group_locked = $column['group_locked']['#value'];
    $variables['column_list'][$column_group][$idx]->column_group_select = drupal_render($column['column_group']);
    $variables['column_list'][$column_group][$idx]->visibility_select = drupal_render($column['visibility']);
    $variables['column_list'][$column_group][$idx]->weight_select = drupal_render($column['weight']);
  }

  $variables['form_submit'] = drupal_render_children($variables['form']);
}


/**
 * Validation handler for search table column settings form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_table_column_settings_form_validate($form, &$form_state) {
  // Every column must belong to a group. The JS should prevent this from happening,
  // but check just in case.

  foreach($form_state['values']['columns'] as $idx => $info_arr) {
    if (array_key_exists('column_group', $info_arr)) {
      if ($info_arr['column_group'] == variable_get('dirt_datatable_colgroup_gid_empty', 0))
        form_set_error('columns][' . $idx . '][column_group', t('Column group required.'));
    }
  }

  // Row weights should be unique within a group, or may end up with mismatch
  // between column headers and DataTable data.

  // Organize rows by column group
  $columns_in_group = array();
  $column_group_gids = array_keys($form_state['values']['column_groups']);
  foreach($column_group_gids as $gid) {
    $columns_in_group[$gid] = array();
  }

  foreach($form_state['values']['columns'] as $idx => $col_info) {
    if ($col_info['group_locked'] > 0)
      $columns_in_group[$col_info['group_locked']][$idx] = $col_info;
    else
      $columns_in_group[$col_info['column_group']][$idx] = $col_info;
  }


  foreach($columns_in_group as $gid => $columns) {
    _dirt_datatable_check_unique_row_weights('columns', $columns, $form, $form_state);
  }
}


/**
 * Submit handler for search table column settings form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_admin_search_table_column_settings_form_submit($form, &$form_state) {
  // Need to update the database table from the form values.

  // Not every field was editable, so check for existence of key before
  // attempting to get value.

  $success = TRUE;

  foreach($form_state['values']['columns'] as $idx => $info_arr) {
    // Attempt table update.
    // Updatable columns: label, weight, column_group, visibility, admin_only
    $fields = array();

    if (array_key_exists('label', $info_arr))
      $fields['label'] = $info_arr['label'];
    if (array_key_exists('weight', $info_arr))
      $fields['weight'] = $info_arr['weight'];
    if (array_key_exists('column_group', $info_arr))
      $fields['colgroup'] =  (int)$info_arr['column_group'];

    // Get cid and class(es)
    $cid = $info_arr['cid'];
    $current_classes = explode(' ', $info_arr['class']);

    // Remove "column-hidden" and "admin-only" from class; will be added
    // back in below if applicable.
    $new_classes = array();
    foreach($current_classes as $class) {
      if (!empty($class) && ($class != 'column-hidden') && ($class != 'admin-only'))
        $new_classes[] = $class;
    }

    if (array_key_exists('visibility', $info_arr)) {
      switch($info_arr['visibility']) {
        case 'visible':
          $fields['visible'] = 1;
          $fields['admin_only'] = 0;
          // No class update needed
          break;

        case 'hidden':
          $fields['visible'] = 0;
          $fields['admin_only'] = 0;
          // Update classes: add "column-hidden"
          $new_classes[] = 'column-hidden';
          break;

        case 'admin':
          $fields['visible'] = 1;
          $fields['admin_only'] = 1;
          // Update classes: add "admin-only"
          $new_classes[] = 'admin-only';
          break;
      }
    }

    // Update classes
    $new_class = implode(' ', $new_classes);
    $fields['class'] = $new_class;
    unset($new_classes);
    unset($current_classes);

    // Update DataTable column
    $conditions = array(0 => array(
      'field' => 'cid',
      'value' => $cid,
      'operator' => '=')
    );
    
    $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_COLUMNS, $fields, $conditions);
    if (is_null($updated)) $success = FALSE;
  }

  // Additionally, reset weight of Site ID column to keep it first in table
  $weight_delta = $form_state['values']['weight_delta'];
  $min_weight = -1 - (int)$weight_delta;

  $fields = array('weight' => $min_weight);

  $conditions = array(0 => array(
    'field' => 'column_name',
    'value' => 'site_id',
    'operator' => '=')
  );

  $updated = dirt_update_database_table(DIRT_SEARCH_TABLE_COLUMNS, $fields, $conditions);
  if (is_null($updated)) $success = FALSE;

  if ($success) {
    drupal_set_message(t('Updated Data Search Portal table columns.'));
  }
  else {
    drupal_set_message(t('An error occurred while attempting to update Data Search ' .
      'Portal table columns. See ') . l(t('site log'), 'admin/reports/dblog') .
      t(' for more information.'), 'error');
  }
}


/*----- Force table update -----*/

/**
 * Callback for table force update.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_admin_update_table_form($form, &$form_state) {
  // Force table update
  $form['dirt_datatable_update_table'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Table Update'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 0,
  );

  // Explanation
  $form['dirt_datatable_update_table']['note'] = array(
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#type' => 'markup',
    '#markup' => t('The data search table is updated automatically as surveys are ' .
                   'submitted or deleted. Click the button below to force an ' .
                   'update.<br><br>' .
                   '<em>Normally it is not necessary to force a manual update of ' .
                   'the table, but it may be needed in certain situations, e.g., ' .
                   'if a field is added to or removed from a survey via the ' .
                   'Drupal "Manage fields" interface.</em><br><br>' .
                   '<strong>Note: This may take some time ' .
                   'to complete if there are many surveys in the system.</strong>'),
    '#weight' => 0,
  );

  // Force update button
  $form['dirt_datatable_update_table']['button'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#prefix' => '<div id="populate-table-submit">',
    '#suffix' => '</div>',
    '#value' => t('Update DIRT Data Search Table'),
    '#submit' => array('dirt_datatable_populate_table_form_submit'),
    '#weight' => 1,
  );

  if(isset($form_state['store'])) {
    $summary_markup = '';

    if ($form_state['store']['success']) {
      $summary_markup .= t('Number of rows inserted: ');
      $summary_markup .= $form_state['store']['inserted'] . '<br>';
      $summary_markup .= t('Number of rows updated: ');
      $summary_markup .= $form_state['store']['updated'] . '<br>';
      $summary_markup .= t('Number of rows deleted: ');
      $summary_markup .= $form_state['store']['deleted'] . '<br><br>';
    }
    else {
      $summary_markup .= '<div class="messages error"><em>';
      $summary_markup .= t('One or more errors were detected while attempting ' .
                           'to update the database table. Check the ' .
                           l(t('site log'), 'admin/reports/dblog') .
                           ' for more information');
      $summary_markup .= '</em></div>';
    }

/*
    $form['dirt_datatable_update_table']['status'] = array(
      '#type' => 'markup',
      '#markup' => $summary_markup,
      '#prefix' => '<div id="update-table-status">',
      '#suffix' => '</div>',
      '#weight' => -1,
    );
*/
  }

  return $form;
}


/**
 * Submit handler for table update
 * (currently doesn't do anything except to display a
 * table updated message).
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_populate_table_form_submit(&$form, &$form_state) {
  //$form_state['rebuild'] = TRUE;

  // Begin table update
  dirt_datatable_populate_table_form_update_table($form, $form_state);
}


/**
 * Handler to update table and store summary of updates.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Array Portion of $form containing updated summary table and update summary.
 */
function dirt_datatable_populate_table_form_update_table(&$form, &$form_state) {
  form_load_include($form_state, 'inc', 'dirt_datatable', 'includes/dirt_datatable.search_table_db_ops');

  //$form_state['store'] = array();
  //$form_state['store']['success'] = FALSE;

  // TODO
  dirt_datatable_force_update_data_table();

/*
  if (!empty($status)) {
    $form_state['store']['success'] = TRUE;

    $form_state['store']['inserted'] = $status['inserted'];
    $form_state['store']['updated'] = $status['updated'];
    $form_state['store']['deleted'] = $status['deleted'];

    watchdog('dirt_datatable', 'DIRT Data Search Portal table updated in ' .
      'database: inserted %insert rows, updated %updated rows, deleted ' .
      '%deleted rows.', array(
        '%insert' => $status['inserted'],
        '%updated' => $status['updated'],
        '%deleted' => $status['deleted'],
      ), WATCHDOG_INFO);
  }
  else {
    watchdog('dirt_datatable', 'Error while force updating DIRT Data ' .
      'Search Portal table.', array(), WATCHDOG_ERROR);
  }
*/

}


/**
 * Helper function that adds markup form elements for reviewing data access
 * request fields, confirming action described in header (if present).
 *
 * @param[in] $request Array of data access request fields from 
 *  database table.
 * @param[in,out] $form Form array.
 * @param[in] $header *Optional* Header text (usually to prompt
 *   user to confirm action). Default generic header will be used
 *   if no header passed in as input.
 */
function _dirt_datatable_add_access_account_review_form_fields($request, &$form, $header = '') {
  if (empty($header)) {
    // Generic header text for reviewing fields
    $header = t('Review the data access request fields below.');
  }

  $fields = array(
    t('First name') => $request['first_name'],
    t('Last name') => $request['last_name'],
    t('E-mail') => $request['email'],
    t('Reason for account') => $request['reason'],
  );

  $markup  = '<p>';
  foreach($fields as $label => $field) {
    $markup .= '<strong>' . $label . ': </strong>' . check_plain($field) . '<br>';
  }
  $markup .= '</p>';

  $form['review_fields'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
    '#prefix' => $header,
  );
}


/**
 * Helper function that checks if a field value is in the
 * user database.
 */
function _dirt_datatable_field_already_in_users_database($field_name, $field_value) {
  $retval = FALSE;

  $query = db_select('users', 'u');
  $query->condition('u.' . $field_name, $field_value);
  $query->fields('u', array($field_name));
  $result = $query->execute()->fetchAll();

  if (!empty($result)) {
    watchdog('dirt_datatable', 'Field %field with value %value already exists in users database.',
      array('%field' => $field_name, '%value' => $field_value), WATCHDOG_INFO);
    $retval = TRUE;
  }

  return $retval;
}


/**
 * Helper function to generate a valid username from a first and
 * last name. Defaults to "firstname.lastname" but adds numbers
 * to the end if username already in use.
 */
function _dirt_datatable_create_username_from_name_field($first_name, $last_name) {
  // Remove all non alphanumeric characters from first name and last name
  $first_name = preg_replace("/[^A-Za-z0-9]/", '', $first_name);
  $last_name = preg_replace("/[^A-Za-z0-9]/", '', $last_name);

  // Create username from full name: first set username to be firstname.lastname
  $username = $first_name . '.' . $last_name;

  // Convert to all lowercase for consistency:
  $username = strtolower($username);

  // If name too long, truncate to two characters less than max allowed
  // (to allow for adding numbers at the end)
  if (strlen($username) > USERNAME_MAX_LENGTH) {
    $username = substr($username, 0, USERNAME_MAX_LENGTH - 2);
  } 

  // Check if username exists in system and keep adding numbers behind username
  // until name unique (building in safety of no more than 10 attempts so loop
  // doesn't run forever).
  $attempts = 1;
  $username_orig = $username;
  while (_dirt_datatable_field_already_in_users_database('name', $username) && ($attempts <= 10)) {
    $username = $username_orig . $attempts;
    $attempts++;
  }

  return ($attempts <= 10) ? $username : '';
}


/**
 * Helper function to check if row weights are unique, and if not, set
 * form error.
 *
 * @param[in] $field_name String field name in $form..
 * @param[in] $fields Array with keys corresponding to $form[$field_name] keys
 *  and values arrays each with a 'weight' key to check.
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function _dirt_datatable_check_unique_row_weights($field_name, $fields, &$form, &$form_state) {
  $weights_seen = array();

  // Initialize array of $idx values for form error.
  $form_error_indices = array();

  foreach($fields as $idx => $info_arr) {
    if (array_key_exists('weight', $info_arr)) {
      $weight = $info_arr['weight'];

      $key = array_search($weight, $weights_seen);
      if ($key !== FALSE) {
        // Weight value is not unique
        // Store index as key and value (to force unique array)
        $form_error_indices[$idx] = $idx;
        // Store the weight it's equal to as well for form error
        $form_error_indices[$key] = $key;
      }
      else {
        // Haven't seen this weight yet, so add it to array
        $weights_seen[$idx] = $weight;
      }
    }
  }

  // This is a bit hacky but it's super annoying to have the error message show
  // up 100 times on the page if there are many fields with repeating weights.
  // So, set the error message once (if applicable), then loop through $idx
  // values to highlight the affected weight fields.

  if (!empty($form_error_indices)) {
    $msg = t('Weights must be unique. Click "Show row weights" in the upper ' .
      'right corner of the table if row weight fields are not currently ' .
      'visible.');

    drupal_set_message($msg, 'error');
  }

  foreach($form_error_indices as $idx) {
    form_set_error($field_name . '][' . $idx . '][weight', '');
  }
}

