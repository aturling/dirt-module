<?php

/**
 * @file
 * This file contains functions related to forms.
 */


/**
 * Callback for data access request form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 *
 * @retval Object Form array.
 */
function dirt_datatable_account_request_form($form, &$form_state) {
  // Add instructions
  $form['description'] = array(
    '#markup' => t('Submit this form to request a data access account.'),
    '#weight' => -100,
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  // Add markup explaining * fields are required
  $form['required_explanation'] = array(
    '#prefix' => '<div class="form-required" id="required-explanation">',
    '#suffix' => '</div>',
    '#markup' => '* Denotes field is required',
    '#weight' => -99,
  );

  // Add form fields

  $form['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
    '#required' => TRUE,
    '#weight' => 1,
  );

  $form['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#required' => TRUE,
    '#weight' => 2,
  );

  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#required' => TRUE,
    '#weight' => 3,
  );

  $form['reason'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason for Account'),
    '#required' => TRUE,
    '#description' => t('Briefly explain why you are requesting a data access account.'),
    '#weight' => 4,
  );

  // Add submit button

  $form['submit'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-actions', 'form-wrapper'),
    ),
    '#value' => t('Submit'),
    '#submit' => array('dirt_datatable_data_access_form_submit'),
    '#weight' => 5,
  );

  // Add validation
  $form['#validate'][] = 'dirt_datatable_data_access_form_validate';

  return $form;
}


/**
 * Validation handler for data access request form.
 */
function dirt_datatable_data_access_form_validate(&$form, &$form_state) {
  $email = trim($form_state['values']['email']); // trim whitespace if present
  $reason = $form_state['values']['reason'];

  // Check valid email address format
  if (!valid_email_address($email))
    form_set_error('email', t('Invalid e-mail address format.'));

  // Check that reason isn't just a number (common for spam submissions)
  if (is_numeric($reason))
    form_set_error('reason', t('Invalid format.'));
}



/**
 * Submit handler for data access request form.
 *
 * @param[in]     $form       Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_datatable_data_access_form_submit(&$form, &$form_state) {
  // Add to database for admin review later
  try {
    // No fields should be empty - all required:
    $fields = array(
      'first_name' => $form_state['values']['first_name'],
      'last_name' => $form_state['values']['last_name'],
      'email' => $form_state['values']['email'],
      'reason' => $form_state['values']['reason'],
    );
    dirt_insert_into_database_table(DIRT_DATA_ACCESS_REQUESTS_TABLE, $fields); 
 
    // Redirect upon successful submission
    $form_state['redirect'] = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);

    drupal_get_messages('status');
    drupal_set_message(t('Form successfully submitted. You will be contacted shortly.'));
  }
  catch (Exception $e) {
    // Something went wrong, user needs to contact a site admin to review this
    drupal_set_message(t('Please contact a site administrator.'), 'error');
  }

  // Send e-mail notification, if enabled.
  if (variable_get('dirt_datatable_config_data_access_requests_email')) {
    $to = variable_get('dirt_datatable_config_notify_access_request_emails');

    $email_subject = t('@site New Data Access Account Request',
      array('@site' => variable_get('site_name')));

    $email_body = array();
    $email_body[] = t('A new data access account request has been submitted.');
    $email_body[] = l(t('View the account request.'), 'admin/config/dirt/data-search/data-access');
    $email_body[] = t('Use the above link to approve or deny the request.');

    dirt_datatable_send_email_notification($to, $email_subject, $email_body, 'data_access_request');
  }
}

