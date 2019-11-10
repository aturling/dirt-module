<?php

/**
 * @file
 * This file contains functions related to forms.
 */


/**
 * Form alter for all survey forms.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_alter_all_survey_forms(&$form, &$form_state) {
  // Clear breadcrumb
  drupal_set_breadcrumb(array());

  // Attach custom CSS to form
  $form['#attached']['css'][] = drupal_get_path('module', 'dirt') . '/css/dirt_forms.css';

  // Get survey type info from content types info
  $content_types_info = dirt_node_info();
  
  // Get type name
  $type = $form['#node']->type;

  // Set placeholder title
  dirt_set_survey_form_placeholder_title($form);

  // Prepopulate replicate number fields, if present
  dirt_prepopulate_rep_number_fields($form);

  // If in node/add (NOT node/edit), and form is only to be collected once, see
  // if form already filled out by user. If so, clear out form and replace with
  // link to update existing form in system (preventing them from submitting a
  // second form of this type).

  if (!isset($form['#node']->nid) && (dirt_get_collection_frequency_of_survey_type($type) == DIRT_ONE_TIME_COLLECTION)) {
    // See if survey already exists in system for user
    global $user;
    $uid = $user->uid;
    $survey_nid = dirt_get_most_recent_survey_of_type_for_user($uid, $type);
    if ($survey_nid) {
      // Hide field groups
      field_group_hide_field_groups($form, array_keys($form['#groups']));

      // Hide form fields (not all fields are within a field group)
      foreach($form as $key => $value) {
        if (substr($key, 0, 6) === 'field_')
          $form[$key]['#access'] = FALSE;          
      }

      // Disable submit button
      $form['actions']['submit']['#access'] = FALSE;

      // Hide the "Create survey" title
      drupal_set_title('');

      // Get human-readable type name
      $name = dirt_get_name_of_survey_type($type);

      $msg = t('You have already entered a @name survey for this site. Click ' .
               l(t('here'), 'node/' . $survey_nid) .
               ' to update this survey, or use the link below to go back to ' .
               'the data entry portal page.', array('@name' => $name)); 

      // Style this similarly to drupal_set_message() warning:
      $form['dirt_return_to_portal_link'] = array(
        '#type' => 'markup',
        '#markup' => $msg,
        '#prefix' => '<div class="messages warning">',
        '#suffix' => '</div>',
      );

      // Button to return to data portal
      $form['dirt_return_to_data_portal_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Return to the data entry portal'),
        '#attributes' => array(
          'class' => array('form-actions', 'form-wrapper'),
        ),
        '#submit' => array('dirt_return_to_data_portal_submit'),
      );
    }
  }
}


/**
 * Submit handler for node/add survey if one-time survey already exists for
 * site. Returns user to the data entry portal page.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_return_to_data_portal_submit(&$form, &$form_state) {
  drupal_goto(variable_get('dirt_data_entry_portal_path', DIRT_DATA_ENTRY_PATH_DEFAULT));
}


/**
 * Makes data entry fields required on user form.
 * (Assumes that user is data entry account type).
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_require_user_data_entry_fields(&$form, &$form_state) {
  form_load_include($form_state, 'inc', 'dirt', 'includes/dirt.fields');

  // Data entry fields
  $data_entry_fields = dirt_get_data_entry_user_field_names();

  // Special cases: admin notes and collection start are optional
  $optional_fields = array(
    'field_dirt_user_admin_notes',
    'field_dirt_user_collection_start',
  );
  $data_entry_fields = array_diff($data_entry_fields, $optional_fields);

  foreach ($data_entry_fields as $field) {
    if (array_key_exists($field, $form)) {
      $lang = dirt_get_form_field_language($field, $form);
      $form[$field][$lang][0]['value']['#required'] = TRUE;
    }
  }

  // Add markup explaining * fields are required
  $form['required_explanation'] = array(
    '#prefix' => '<div class="form-required" id="required-explanation">',
    '#suffix' => '</div>',
    '#markup' => '* Denotes field is required',
    '#weight' => -99,
  );
}


/**
 * Customize the survey data sharing preferences field.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_customize_sharing_preferences_field(&$form, &$form_state) {
  // Moves the data sharing permissions description between the
  // label and the choices

  $perms_field = 'field_dirt_user_sharing_perms';

  if (array_key_exists($perms_field, $form)) {
    $lang = dirt_get_form_field_language($perms_field, $form);
    $description = $form[$perms_field][$lang]['#description'];

    $form[$perms_field][$lang]['#description'] = '';
    $form[$perms_field][$lang]['#field_prefix'] = '<div class="form-description">' . $description . '</div>';
  }
}


/**
 * Displays data entry fields on form for data entry accounts and hides for
 * all other account types. Also makes data entry fields required if
 * displayed on form.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_toggle_data_entry_fields_form(&$form, &$form_state) {
  form_load_include($form_state, 'inc', 'dirt', 'includes/dirt.fields');

  $data_entry_fields = dirt_get_data_entry_user_field_names();
  $data_entry_user_rid = variable_get('dirt_data_entry_user_role');

  if (user_has_role($data_entry_user_rid, $form['#user'])) {
    // Make data entry fields required
    dirt_require_user_data_entry_fields($form, $form_state);

    // Customize data sharing preferences field
    dirt_customize_sharing_preferences_field($form, $form_state);
  }
  else {
    // Not a data entry user, so hide data entry fields
    foreach ($data_entry_fields as $field) {
      if (array_key_exists($field, $form)) $form[$field]['#access'] = FALSE;
    }
  }  
}


/**
 * Alter group information survey form.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_alter_dirt_group_information_survey_form(&$form, &$form_state) {
  // Customize form header
  $subheader = t('Group Information');
  _dirt_customize_survey_form_header($form, $subheader);
}


/**
 * Alter site description survey form.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_alter_dirt_site_description_survey_form(&$form, &$form_state) {
  // Customize form header
  $subheader = t('One-time data collection');
  _dirt_customize_survey_form_header($form, $subheader);

  // Add save warning
  _dirt_add_save_warning($form);

  // Prepopulate fixed field collection fields other than replicate number
  // which was handled separately.
  // Array pairing field collection field name with replicate field names:
  $field_names = array(
    'field_dirt_site_layer_data' => 'field_dirt_site_layer_number',
  );
  dirt_prepopulate_fixed_fc_fields($field_names, $form);

  unset($field_names);

  // Hide type of crop and cropping system fields unless cropland is selected
  dirt_toggle_site_desc_crop_fields($form);

  // Now check if we are in node/add or node/edit:
  if (isset($form['#node']->nid)) {
    // We're in node/edit

    // Check whether participants are allowed to make changes to the site
    // description survey.
    if (!(variable_get('dirt_config_site_desc_changes_allowed'))) {
      // If not, we need to lock the nonempty fields.
      dirt_lock_nonempty_form_fields('dirt_site_description_survey', $form);
    }
  }
  else {
    // Otherwise, we're in node/add:
    // Hide land management updates field (only makes sense in node/edit,
    // i.e., updating an existing survey)
    $form['field_dirt_site_manage_updates']['#access'] = FALSE;
  }
}


/**
 * Alter monthly survey form.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_alter_dirt_monthly_data_survey_form(&$form, &$form_state) {
  // Customize form header
  $subheader = t('Monthly data collection');
  _dirt_customize_survey_form_header($form, $subheader);

  // Prepopulate color number by hour, which begins counting at zero
  $field_names = array(
    'field_dirt_color_number_by_hour' => 'field_dirt_soil_resp_hour',
  );
  dirt_prepopulate_fixed_fc_fields($field_names, $form, 0);

  unset($field_names);

  // Special case for monthly forms:
  // The last two soil respiration hours should be 10 and 24
  dirt_set_last_soil_respiration_hours($form);

  // Set the form titles for the bulk density calculations section to include the
  // reference letters and equations
  dirt_set_bulk_density_form_column_titles($form);
}


/**
 * Alter twice a year survey form.
 *
 * @param[in,out] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_alter_dirt_twice_year_data_survey_form(&$form, &$form_state) {
  // Customize form header
  $subheader = t('Twice a year data collection');
  _dirt_customize_survey_form_header($form, $subheader);
}


/**
 * Validation function for the group information survey form.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_group_information_survey_node_validate($form, &$form_state) {
  // Validate phone number format: xxx-xxx-xxxx (10 digits)
  $phone_field = 'field_dirt_group_leader_phone';

  if (array_key_exists($phone_field, $form_state['values'])) {
    $lang = dirt_get_form_field_language($phone_field, $form);
    $phone = $form_state['values'][$phone_field][$lang]['0']['value'];

    if ($phone && !preg_match('/^[0-9]{3}-[0-9]{3}-[0-9]{4}$/', $phone)) {
      form_set_error($phone_field, t('Phone number must be in format xxx-xxx-xxxx.'));
    }
  }

  // Validate e-mail format
  $email_field = 'field_dirt_group_leader_email';
  $lang = dirt_get_form_field_language($email_field, $form);
  $email = $form_state['values'][$email_field][$lang]['0']['value'];

  if ($email && !valid_email_address($email))
    form_set_error($email_field, t('Invalid e-mail address format.'));
  
}


/**
 * Validation function for site description survey form.
 *
 * @param[in] $form Form array.
 * @param[in,out] $form_state Form state array.
 */
function dirt_site_description_survey_node_validate($form, &$form_state) {
  // Validate zip code format: xxxxx (5 digits)

  $zip_field = 'field_dirt_site_zip_code';
  if ( array_key_exists($zip_field, $form_state['values']) ) {
    $lang = dirt_get_form_field_language($zip_field, $form);
    $zipcode = $form_state['values'][$zip_field][$lang]['0']['value'];

    if (strlen($zipcode) > 0 && !preg_match('/^[0-9]{5}$/', $zipcode)) {
        form_set_error($zip_field, t('Zip code must be 5 digits long.'));
    }
  }

  // If land management update notes entered, require an update date
  $manage_field = 'field_dirt_site_manage_updates';
  $date_field = 'field_dirt_site_update_date';
  $notes_field = 'field_dirt_site_update_notes';

  if ( array_key_exists($manage_field, $form_state['values']) ) {
    $manage_lang = dirt_get_form_field_language($manage_field, $form);
    $date_lang = dirt_get_form_field_collection_field_language($date_field, $manage_field, $form);
    $notes_lang = dirt_get_form_field_collection_field_language($notes_field, $manage_field, $form);

    foreach ($form_state['values'][$manage_field][$manage_lang] as $delta => $val_array) {
      if (is_array($val_array)) {
        $note_entered = ($val_array[$notes_field][$notes_lang][0]['value'] != '');
        $date_not_entered = !(isset($val_array[$date_field][$date_lang][0]['value']));

        if ($note_entered && $date_not_entered) {
          form_set_error($manage_field . '][' . $manage_lang . '][' . $delta . '][' . $date_field,
            'Date of land management update is required.');
        }
      }
    }

    unset($manage_lang);
    unset($date_lang);
    unset($notes_lang);
  }
}


/**
 * Sets a placeholder title for survey form.
 *
 * @param[in,out] $form Form array.
 */
function dirt_set_survey_form_placeholder_title(&$form) {
  // Make title field not required and hide it on form
  $form['title']['#required'] = FALSE;
  $form['title']['#access'] = FALSE;

  // Set placeholder title:
  $form['title']['#value'] = t('Survey Collected');
}


/**
 * Prepopulates replicate numbers in field collection, if present on form.
 *
 * @param[in,out] $form Form array.
 */
function dirt_prepopulate_rep_number_fields(&$form) {
  // Get all instances of replicate number field and bundles attached to
  $instances = field_info_field_map();
  if (array_key_exists('field_dirt_replicate_number', $instances)) {
    $bundles = $instances['field_dirt_replicate_number']['bundles']['field_collection_item'];
    $field_names = array();

    foreach($bundles as $bundle) {
      if (array_key_exists($bundle, $form))
        $field_names[$bundle] = 'field_dirt_replicate_number';
    }

    dirt_prepopulate_fixed_fc_fields($field_names, $form);
  }
}


/**
 * Prepopulates layer numbers, replicate numbers, etc. for site description
 * form.
 *
 * @param[in] $field_names Array of field collection field (machine) names.
 * @param[in,out] $form Form array.
 * @param[in] $start *Optional* Integer to begin counting, defaults to 1.
 */
function dirt_prepopulate_fixed_fc_fields($field_names, &$form, $start = 1) {
  // For the following field collection fields, prepopulate their replicate
  // number fields with 1, 2, 3, ..., N where N=cardinality of fc field.

  // Note: this will NOT work for field collections where cardinality is
  // unlimited, but it is assumed that $field_names only contains
  // field collections with finite cardinality, as is true of the fields that
  // are installed with this module.

  // Additionally, make the replicate numbers read-only so that they cannot be
  // changed on the form, and attach custom css class that can be used to
  // style the fields to indicate to the user that they can't be changed.

  foreach ($field_names as $fc_name => $rep_field) {
    // First check that fields exist in case forms were altered from the UI
    // or another module
    $fc_field_exists = array_key_exists($fc_name, $form);
    if ($fc_field_exists) {
      // Get the field collection field language
      $fc_lang = dirt_get_form_field_language($fc_name, $form);

      $rep_field_exists = array_key_exists($rep_field, $form[$fc_name][$fc_lang][0]);

      if ( $rep_field_exists ) {
        // Get cardinality of field collection field
        $card = $form[$fc_name][$fc_lang]['#cardinality'];

        // Get the replicate field language
        $rep_lang = dirt_get_form_field_collection_field_language($rep_field, $fc_name, $form);

        for ($delta = 0; $delta < $card; $delta++) {
          // Set the default replicate values to 1, 2, 3, ...
          $form[$fc_name][$fc_lang][$delta][$rep_field][$rep_lang][0]['value']['#default_value'] = $delta + $start;

          // Lock the field so that the replicate values cannot be changed on the form
          // Lock these for admins too
          dirt_lock_field_collection_form_field($rep_field, $delta, $fc_name, $form, TRUE);
        }

        unset($card);
        unset($rep_lang);
      }

      unset($rep_field_exists);
      unset($fc_lang);
    }

    unset($fc_field_exists);
  }

  unset($field_names);
}


/**
 * Hide type of crop fields unless cropland (tillage/no-tillage) is selected as
 * habitat type.
 *
 * @param[in,out] $form Form array.
 */
function dirt_toggle_site_desc_crop_fields(&$form) {
  // First check that fields exist in case forms were altered from the GUI
  // or another module
  $habitat_field = 'field_dirt_site_habitat_type';
  if (array_key_exists($habitat_field, $form)) {
    $hab_field_lang = dirt_get_form_field_language($habitat_field, $form);

    $crop_fields = array(
      'field_dirt_site_type_of_crop',
      'field_dirt_site_cropping_system',
    );

    // Check if cropland tillage is selected
    $cropland_tillage = array(
      ':input[name="' . $habitat_field . '[' . $hab_field_lang . '][select]"]' => array(
        'value' => t('Cropland tillage')
      ),
    );

    // Check if cropland no-tillage is selected
    $cropland_no_tillage = array(
      ':input[name="' . $habitat_field . '[' . $hab_field_lang . '][select]"]' => array(
        'value' => t('Cropland no-tillage')
      ),
    );

    // Hide crop fields unless cropland is selected (tillage or no-tillage)
    foreach ($crop_fields as $crop_field) {
      if ( array_key_exists($crop_field, $form) ) {
        $form[$crop_field]['#states'] = array(
          'visible' => array($cropland_tillage, $cropland_no_tillage),
        );
      }
    }
  }

  unset($habitat_field);
  unset($hab_field_lang);
  unset($crop_fields);
}


/**
 * Lock all nonempty form fields of node type.
 * Assumes that we're in the node/edit form.
 *
 * @param[in] $type Node type string.
 * @param[in,out] $form Form array.
 */
function dirt_lock_nonempty_form_fields($type, &$form) {
  // Get all fields belonging to the survey:
  $survey_fields = array_keys(field_info_instances('node', $type));

  // Since we're in node/edit, we'll use the node object itself to
  // determine which fields have values already and which are empty.
  $node = $form['#node'];

  foreach ($survey_fields as $field) {
    // Get field info, primarily to determine field type
    $field_info = field_info_field($field);

    // Get field value, if it exists
    $field_val = field_get_items('node', $node, $field);

    // Handle field collections separately:
    if ($field_info['type'] == 'field_collection') {
      $fc_ids = array();

      $fc_fields = field_info_instances('field_collection_item', $field);

      foreach ($fc_fields as $fc_field) {
        foreach ($field_val as $delta => $val_array) {
          $fc_item = field_collection_item_load($val_array['value']);

          $fc_field_val = field_get_items('field_collection_item', $fc_item, $fc_field['field_name']);

          if ($fc_field_val) {
            dirt_lock_field_collection_form_field($fc_field['field_name'], $delta, $field, $form);
          }

          unset($fc_item);
          unset($fc_field_val);
        }
      }

      unset($fc_ids);
      unset($fc_fields);
    }
    elseif ($field_val) {
      // Handle all other field types:
      dirt_lock_form_field($field, $form);
    }

    unset($field_info);
    unset($field_val);
  }

  unset($node);
}


/**
 * Sets the 10 and 24 hour special cases for soil respiration input.
 *
 * @param[in,out] $form Form array.
 */
function dirt_set_last_soil_respiration_hours(&$form) {
  $bundle = 'field_dirt_color_number_by_hour';
  $hour_field = 'field_dirt_soil_resp_hour';
  $bundle_exists = array_key_exists($bundle, $form);

  if ($bundle_exists) {
    $bundle_lang = dirt_get_form_field_language($bundle, $form);
    $hour_lang = dirt_get_form_field_collection_field_language($hour_field, $bundle, $form);
    $form[$bundle][$bundle_lang][6][$hour_field][$hour_lang][0]['value']['#default_value'] = 10;
    $form[$bundle][$bundle_lang][7][$hour_field][$hour_lang][0]['value']['#default_value'] = 24;
  }

  unset($bundle);
  unset($hour_field);
  unset($bundle_exists);
}


/**
 * Sets the bulk density column titles in the monthly form.
 *
 * @param[in,out] $form Form array.
 */
function dirt_set_bulk_density_form_column_titles(&$form) {
  module_load_include('inc', 'dirt', 'includes/dirt.fields');

  // Get array of bundle name, field (column) name, and titles to set:
  $column_info = dirt_get_bulk_density_column_title_info();

  foreach($column_info as $field_name => $column) {
    // Get column and field info
    $bundle = $column['bundle'];
    $bundle_exists = array_key_exists($bundle, $form);

    if ($bundle_exists) {
      $bundle_lang = dirt_get_form_field_language($bundle, $form);
      $field_lang = dirt_get_form_field_collection_field_language($field_name, $bundle, $form);
      $title = $form[$bundle][$bundle_lang][0][$field_name][$field_lang]['#title'];

      if (array_key_exists('title_prefix', $column)) {
        // Set title prefix
        $form[$bundle][$bundle_lang][0][$field_name][$field_lang]['#title'] = $column['title_prefix'] . $title;
      }
      if (array_key_exists('title_suffix', $column)) {
        // Set title suffix
        $form[$bundle][$bundle_lang][0][$field_name][$field_lang]['#title'] .= $column['title_suffix'];
      }

      unset($bundle_lang);
      unset($field_lang);
      unset($title);
    }

    unset($bundle);
    unset($bundle_exists);
  }

  unset($column_info);
}


/**
 * Lock the auto-calculated bulk density form fields.
 *
 * @param[in,out] $form Form array.
 */
function dirt_lock_auto_calculated_bulk_density_fields(&$form) {
  // array pairing field name to lock with its bundle (field collection name)
  $field_names = array(
    'field_dirt_weight_wet_soil' => 'field_dirt_sample_before_drying',
    'field_dirt_weight_dry_soil' => 'field_dirt_sample_after_drying',
    'field_dirt_soil_bulk_density' => 'field_dirt_bulk_density_calcs',
    'field_dirt_water_gravimetric' => 'field_dirt_bulk_density_calcs',
    'field_dirt_water_volumetric' => 'field_dirt_bulk_density_calcs',
    'field_dirt_total_soil_porosity' => 'field_dirt_bulk_density_calcs',
    'field_dirt_water_filled_pores' => 'field_dirt_bulk_density_calcs',
  );

  foreach ($field_names as $field => $bundle) {
    dirt_lock_field_collection_form_field($field, 0, $bundle, $form);
  }
}


/**
 * Makes a survey form field read-only ("locking" it).
 *
 * @param[in] $field Field name string to lock.
 * @param[in,out] $form Form array.
 * @param[in] $lock_for_admins *Optional* Optional parameter to specify whether
 *   to lock the fields for admins as well, defaults to FALSE.
 */
function dirt_lock_form_field($field, &$form, $lock_for_admins = FALSE) {
  // Verify that field exists before attempting to lock:
  if ( array_key_exists($field, $form) ) {
    // If user is not admin or $lock_for_admins is TRUE, lock fields
    $user_is_not_admin = !(user_access('administer site configuration'));

    if ( $user_is_not_admin || $lock_for_admins) {
      $lang = dirt_get_form_field_language($field, $form);

      // Slightly different for different fields (text, number, date, etc.)
      if (array_key_exists('0', $form[$field][$lang]) && array_key_exists('value', $form[$field][$lang][0])) {
        $form[$field][$lang][0]['value']['#attributes']['readonly'] = 'readonly';
        $form[$field][$lang][0]['value']['#attributes']['#disabled'] = TRUE;
        $form[$field][$lang][0]['value']['#attributes']['class'][] = 'lock-field';
      }
      elseif (array_key_exists('#attributes', $form[$field][$lang])) {
        // select fields / select or other fields
        $form[$field][$lang]['#attributes']['readonly'] = 'readonly';
        $form[$field][$lang]['#attributes']['#disabled'] = TRUE;
        $form[$field][$lang]['#attributes']['class'][] ='lock-field';
      }
      else {
        $form[$field][$lang]['#disabled'] = TRUE;
      }

      unset($lang);
    }

    unset($user_is_not_admin);
  }
}


/**
 * Makes a field collection form field read-only ("locking" it).
 *
 * @param[in] $field Field name string of the field belonging to the field collection.
 * @param[in] $delta Field collection delta integer.
 *   For field collections with cardinality 1, $delta will be 0.
 *   For field collections with cardinality N > 1 or unlimited, $delta will be
 *   an integer >= 0.
 * @param[in] $bundle Bundle (node, user) field attached to.
 * @param[in,out] $form Form array.
 * @param[in] $lock_for_admins *Optional* Optional parameter to specify whether to lock
 *   the fields for admins as well, defaults to FALSE.
 */
function dirt_lock_field_collection_form_field($field, $delta, $bundle, &$form, $lock_for_admins = FALSE) {
  // Verify that bundle exists before attempting to lock field
  if ( array_key_exists($bundle, $form) ) {
    // If user is not admin or $lock_for_admins is TRUE, lock fields
    $user_is_not_admin = !(user_access('administer site configuration'));

    if ( $user_is_not_admin || $lock_for_admins) {
      // Get field languages
      $bundle_lang = dirt_get_form_field_language($bundle, $form);

      if ( array_key_exists($field, $form[$bundle][$bundle_lang][$delta]) ) {
        $field_lang = dirt_get_form_field_collection_field_language($field, $bundle, $form);

        // Set the field to be read-only
        // Slightly different for different fields (text, number, date, etc.)
        if ( array_key_exists('0', $form[$bundle][$bundle_lang][$delta][$field][$field_lang])
             && array_key_exists('value', $form[$bundle][$bundle_lang][$delta][$field][$field_lang][0]) ) {
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang][0]['value']['#attributes']['readonly'] = 'readonly';
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang][0]['#disabled'] = TRUE;
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang][0]['value']['#attributes']['class'][] = 'lock-field';
        }
        elseif (array_key_exists('#attributes', $form[$bundle][$bundle_lang][$delta][$field][$field_lang])) {
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang]['#attributes']['readonly'] = 'readonly';
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang]['#attributes']['#disabled'] = TRUE;
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang]['#attributes']['class'][] = 'lock-field';
        }
        else {
          // This should handle just about every other type of field if the above methods don't work:
          $form[$bundle][$bundle_lang][$delta][$field][$field_lang]['#disabled'] = TRUE;
        }

        unset($field_lang);
      }

      unset($bundle_lang);
    }
  }

  unset($user_is_not_admin);
}


/**
 * Returns field language from form object.
 *
 * See: https://drupal.stackexchange.com/questions/25839/best-practice-for-language-key-for-und-in-hook-form-alter/31639#31639
 *
 * @param[in] $field Field name string.
 * @param[in,out] $form Form array.
 */
function dirt_get_form_field_language($field, &$form) {
  return $form[$field]['#language'];
}


/**
 * Returns field collection field language from from object.
 *
 * @param[in] $field Field name string.
 * @param[in] $bundle Bundle name string.
 * @param[in,out] $form Form array.
 */
function dirt_get_form_field_collection_field_language($field, $bundle, &$form) {
  // Need to get the bundle (field collection) language first
  $lang = dirt_get_form_field_language($bundle, $form);

  return $form[$bundle][$lang][0][$field]['#language'];
}


/**
 * Helper function to customize form header to survey type.
 *
 * @param[in,out] $form Form array.
 * @param[in] $subheader Subheader string.
 */
function _dirt_customize_survey_form_header(&$form, $subheader) {
  // Add subheader markup
  $markup = '<h3>' . $subheader . '</h3>';
  $field = 'field_dirt_form_header';
  $lang = dirt_get_form_field_language($field, $form);
  $form[$field][$lang][0]['markup']['#markup'] .= $markup;
}


/**
 * Helper function to add save warning to submission note on
 * site description surveys, if no changes allowed.
 */
function _dirt_add_save_warning(&$form) {
  if (!(variable_get('dirt_config_site_desc_changes_allowed'))) {
    $markup = _dirt_get_save_warning_message_markup();
    $field = 'field_dirt_form_submission_note';
    $lang = dirt_get_form_field_language($field, $form);
    $msg = $form[$field][$lang][0]['markup']['#markup'];
    $form[$field][$lang][0]['markup']['#markup'] = $markup . $msg;
  }
}


/**
 * Helper function to retrieve save warning markup common to all surveys
 * displaying the warning message.
 *
 * @retval String warning message HTML markup.
 */
function _dirt_get_save_warning_message_markup() {
  return '<div><span class="dirt-save-warning-message">' . t('Review all ' .
    'fields very carefully before you Save; you will not be able to change ' .
    'any nonempty fields once this form is submitted without contacting ' .
    'the webmaster.') . '</span></div>';
}
