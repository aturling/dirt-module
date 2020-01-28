<?php

/**
 * @file
 * This file specifies functions for the data entry portal.
 */

/**
 * Callback for data entry portal page.
 *
 * @retval Renderable array.
 */
function dirt_data_entry_portal_page() {
  // Set the page title
  drupal_set_title(t('Data Entry Portal'));

  // Add custom css
  drupal_add_css(drupal_get_path('module', 'dirt') . '/css/dirt_data_entry_portal.css');

  // Override menu breadcrumbs
  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Citizen Science Soil Health Surveys'), $citizen_sci_path);
  $breadcrumb[] = t('DIRT Data Entry Portal');
  drupal_set_breadcrumb($breadcrumb);

  // Get logged-in user ID
  global $user;
  $uid = $user->uid;

  // Return render array
  $build = array();

  // Check if user may access data entry portal, and if so, build page.
  // Otherwise, provide login notice and login box.
  if (user_access('access data entry portal')) {
    // Site's collection status (active vs. closed)
    $collection_status = dirt_get_collection_status($user);

    // Begin building render array:

    $build = array(
      // Welcome username message
      'welcome_message' => array(
        '#type' => 'markup',
        '#markup' => t('Welcome, @name', array('@name' => $user->name)),
        '#prefix' => '<p>',
        '#suffix' => '</p>',
      ),

      /*
      // Site collection status
      'collection_status' => array(
        '#type' => 'markup',
        '#markup' => t('status: @status', array('@status' => $collection_status)),
        '#prefix' => '<p>',
        '#suffix' => '</p>',
      ),
      */
    );

    // If site is still open, add instructions and links to forms. If site is
    // closed, display a message but don't link to any forms (to prevent the
    // incomplete form links from piling up)

    switch($collection_status) {
      case 'active':
        $instructions_markup = t('Enter the data in the links below ' .
          'according to the survey month and year.');

        // Add instructions
        $build['instructions'] = array(
          '#type' => 'markup',
          '#markup' => $instructions_markup,
          '#prefix' => '<p id="dirt-entry-insructions-para">',
          '#suffix' => '</p>',
        );

        // Build $items array according to survey schedule
        $items = dirt_get_survey_schedule($uid);

        // Add survey schedule, if applicable
        $build['survey_schedule'] = array(
          '#theme' => 'item_list',
          '#type' => 'ul',
          '#attributes' => array('class' => 'dirt-survey-schedule-list'),
          '#items' => $items,
        );

        // Add optional links for surveys for other months
        dirt_add_other_month_survey_links($build);

        break;

      case 'closed':
        $instructions_markup = t('This account is closed. All previously ' .
          'submitted surveys can be viewed in the table below.');

        // Add instructions
        $build['instructions'] = array(
          '#type' => 'markup',
          '#markup' => $instructions_markup,
          '#prefix' => '<p id="dirt-entry-insructions-para">',
          '#suffix' => '</p>',
        );

        break;
    }

    // In any case (active or closed), add survey history table
    $section_header = t('History');
    $table_header = array(
      'Date of Survey',
      'Survey Type',
      'View Survey Data',
      'Update Survey Data
    ');
    $table_rows = dirt_get_survey_history_table_rows($uid);

    $build['survey_history_header'] = array(
      '#type' => 'markup',
      '#markup' => $section_header,
      '#prefix' => '<h3>',
      '#suffix' => '</h3>',
    );

    $build['survey_history_table'] = array(
      '#theme' => 'table',
      '#header' => $table_header,
      '#rows' => $table_rows,
      '#empty' => t('No surveys submitted.'),
    );
  }
  elseif (!(user_is_logged_in())) {
    // Display message that user must login to proceed
    $build['login_required_notice'] = array(
      '#type' => 'markup',
      '#markup' => t('Login required to access the Data Entry Portal. If you ' .
        'do not have an account and would like to request one, click') . ' ' .
        l(t('here'), $citizen_sci_path) . ' ' . t('to learn more about the ' .
        'program and how to request a data entry account.'),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );

    // Login block is displayed by default in a fresh Drupal install,
    // but if it is removed, the code below will display it on this
    // page below the message above.
    //$login_block = module_invoke('user', 'block_view', 'login');
    //$build['login_box'] = $login_block['content'];
  }
  else {
    // User is logged-in but doesn't have the appropriate permissions to access
    // the data entry portal.

    // Display message that user must request data entry account to proceed.
    $build['login_required_notice'] = array(
      '#type' => 'markup',
      '#markup' => t('The Data Entry Portal may only be accessed by ' .
        'participants in the Citizen Science program. If you wish to ' .
        'participate, click') . ' ' . l(t('here'), $citizen_sci_path) .
        ' ' . t('to learn more about the program and how to request a data ' .
        'entry account.'),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
  }

  return $build;
}


/**
 * Adds links to add surveys in months other than assigned collection months, if
 * applicable.
 *
 * @param[in,out] $build Renderable array containing data entry portal content.
 */
function dirt_add_other_month_survey_links(&$build) {
  // Go ahead and add the header, but remove it later if there are no links.
  $other_months_header = t('Surveys on other months (optional)');
  $build['other_surveys_header'] = array(
    '#type' => 'markup',
    '#markup' => $other_months_header,
    '#prefix' => '<h3>',
    '#suffix' => '</h3>',
  );

  $links_exist = FALSE;

  // Loop through survey types
  $survey_types = dirt_get_survey_types();

  // Sort by collection frequency in descending order
  usort($survey_types, '_dirt_sort_by_frequency_desc');

  foreach($survey_types as $type) {
    // Get collection months for type
    $collection_months = dirt_get_collection_months_of_survey_type($type);

    // Only relevant for surveys with specified collection months:
    if (!empty($collection_months)) {
      // Get collection frequency for type
      $frequency = dirt_get_collection_frequency_of_survey_type($type);

      if ($frequency == 12) {
        // Handle monthly surveys as special case: single out the months of
        // non-collection, rather than collection which will probably be most
        // if not all months.

        // Get months of non-collection (in reverse chronological order):
        $non_collection_months = array_diff(array_reverse(range(1,12)), $collection_months);

        if (!empty($non_collection_months)) {
          // Get list of months
          $month_list_str = _dirt_get_month_list($non_collection_months);
          $other_monthly_markup = t('If you collect monthly data in') . ' ' .
            $month_list_str . ' ' . t('(or any month not listed above), ' .
            'enter your data') . ' ' . 
            l(t('here'), 'node/add/dirt-monthly-data-survey') . '.';

          $build['other_monthly_surveys'] = array(
            '#type' => 'markup',
            '#markup' => $other_monthly_markup,
            '#prefix' => '<div class="other-survey-months">',
            '#suffix' => '</div>',
          );
        }
      }
      else {
        // Handle rest of survey collection frequencies:
        // Since frequency < 12, there's at least one month of non-collection
        // under the usual schedule.

        // Get collection frequency as string
        $frequency_str = dirt_get_collection_frequency_str_of_survey_type($frequency);      
 
        // Get list of collection months as string
        $month_list_str = _dirt_get_month_list($collection_months); 
        
        $other_markup = t('If you collect') . ' ' . $frequency_str . ' ' .
          t('data in a month other than') . ' ' . $month_list_str . ', ' .
          t('enter your data') . ' ' .
          l(t('here'), 'node/add/XX') . '.';

        $build['other_surveys'] = array(
          '#type' => 'markup',
          '#markup' => $other_markup,
          '#prefix' => '<div class="other-survey-months">',
          '#suffix' => '</div>',
        );
      }
    }
  }

  // Remove header if no links
  if (!$links_exist) unset($build['other_surveys_header']);
}


/**
 * Get collection status of user.
 *
 * @param[in] $user User object.
 *
 * @retval String for collection status (defaults to 'active' if not yet set).
 */
function dirt_get_collection_status($user) {
  // Get field value using Entity API (default active)
  $status = 'active';

  try {
    $user_wrapper = entity_metadata_wrapper('user', $user);
    $collection_status_field = 'field_dirt_user_collection_state';
    $status = $user_wrapper->$collection_status_field->value();
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt', $e);
  }

  return $status;
}


/**
 * Returns a list_item $items array with the survey schedule for the account.
 *
 * The survey schedule includes the name of the survey to be filled out and a
 * link to the node/add form for that survey.
 *
 * @param[in] $uid User ID (uid).
 *
 * @retval Array of rows formatted for form element with 'table' theme.
 */
function dirt_get_survey_schedule($uid) {
  // Initialize $items array
  $items = array();

  // Get array of all scheduled surveys beginning with collection
  // start date to present date.
  $scheduled_surveys = dirt_get_scheduled_surveys($uid);

  // For each scheduled survey, check to see if survey already completed
  // for that date. If not, add to $items to-do list.
  foreach ($scheduled_surveys as $survey) {
    // do database query for surveys from user with type
    // and survey date betweeen date1 and date2
    $type = $survey['type'];
    $date1 = $survey['date1'];
    $date2 = $survey['date2'];

    $survey_nid = dirt_get_survey_between_dates($date1, $date2, $type, $uid);
    if (empty($survey_nid)) {
      // No such survey exists, so add to $items
      $items[] = array(
        'data' => _dirt_get_survey_item_data_text($type, $date1),
        'children' => array(
          _dirt_get_survey_item_submit_link($type),
        ),
      );
    }
  }

  // If all scheduled surveys have been completed, $items is empty, so indicate
  // to user that there's nothing to do at this time.
  if (empty($items)) $items = array(0 => t('All survey submission is up to date.'));

  return $items;
}


/**
 * Return array of associative arrays of every scheduled survey between
 * collection start date and present date.
 *
 * Return array has form:
 * array(
 *   array(
 *     'type' => Survey type machine name string,
 *     'date1' => Oldest possible date of survey,
 *     'date2' => Most recent possible date of survey,
 *   ),
 * )
 * where 'date1' and 'date2' specify the date range for the scheduled
 * survey (e.g., 12 month span for once a year surveys, 1 month span
 * for monthly surveys, etc.)
 *
 * @param[in] $uid User ID (uid).
 *
 * @retval Array of associative arrays of scheduled survey info .
 */
function dirt_get_scheduled_surveys($uid) {
  // Initialize return array
  $scheduled_surveys = array();

  // Get survey type info from content types info
  $survey_types = dirt_get_survey_types();

  // Remove any admin-only types for data entry portal (will be
  // entered by admins separately).
  foreach($survey_types as $idx => $type) {
    if (dirt_get_admin_only_status_of_survey_type($type))
      unset($survey_types[$idx]);
  }

  // To begin, get array of all month+year dates, beginning with the
  // collection start date to present date.
  $dates = dirt_get_all_possible_survey_dates($uid);

  // Next build scheduled surveys array from $dates

  // Initialize month counter
  // This counts which month we're at in the collection cycle,
  // which may not line up with the calendar year
  // E.g., collection begins in May => May is month 1, June is month 2, etc.
  $collection_month = 1;

  // Handle special case of one-time surveys first.:
  foreach ($survey_types as $type) {
    if (dirt_get_collection_frequency_of_survey_type($type) == DIRT_ONE_TIME_COLLECTION) {
      // Add once to scheduled surveys array, set date range undefined
      $scheduled_surveys[] = array(
        'type' => $type,
         'date1' => '',
         'date2' => '',
      );
    }
  }

  // For all other surveys, loop through $dates in chronological order
  // ($dates is created in reverse chronological order)
  foreach (array_reverse($dates) as $date) {
    $calendar_month = date('n', strtotime($date));
    $calendar_year = date('Y', strtotime($date));

    // Loop through survey types and determine whether survey is expected for
    // this date. Do not include one-time surveys here.
    foreach ($survey_types as $type) {
      $frequency = dirt_get_collection_frequency_of_survey_type($type);
      if ($frequency > 0) {
        $month_skip = 12 / $frequency;
        $collection_months = array();
        $is_collection_month = FALSE;

        // Determine collection months
        // First check if survey type specifies them directly
        $collection_months = dirt_get_collection_months_of_survey_type($type);

        if (!empty($collection_months)) {
          // For this check, look at the actual calendar month, not the month
          // counter.
          $is_collection_month = (in_array($calendar_month, $collection_months));
        }
        else {
          // Otherwise, build the list based on frequency
          // Separate out once a year as a special case
          $collection_months = ($frequency == 1) ? array(1) : range(1, 12, $month_skip);
 
          // For this check, look at the collection month, not the calendar month
          $is_collection_month = (in_array($collection_month, $collection_months));
        }

        if ($is_collection_month) {
          // This is a collection month, add to $scheduled_surveys array

          // Set the date range for this survey
          // Start date will be first day of current month+year of $date
          $date1 = date('Y-m', mktime(0, 0, 0, $calendar_month, 1, $calendar_year));

          // End date is until next collection month, if specified, otherwise it
          // depends on survey frequency.
          $date2 = '';

          if (!empty($collection_months)) {
            $idx = array_search($calendar_month, $collection_months);
            $next_idx = $idx + 1;
            $next_year = $calendar_year;
            // Wrap around to next year if necessary
            if ($next_idx == count($collection_months)) {
              $next_idx = 0;
              $next_year++;
            }
            $next_month = $collection_months[$next_idx];

            $date2 = date('Y-m', mktime(0, 0, 0, $next_month, 1, $next_year));
          }
          else {
            // Take start date and add $month_skip (12/frequency) months
            // e.g, 1 month to complete monthly surveys, 1 year to complete annual surveys, etc.
            $date2 = date('Y-m', strtotime($date1 . '+ ' . $month_skip . ' months'));
          }

          // Add to array
          $scheduled_surveys[] = array(
            'type' => $type,
            'date1' => $date1,
            'date2' => $date2,
          );
        }
      }
    }

    // Done looping through types, advance to the next month
    // (loop back around to 1 after 12th month)
    $collection_month = ($collection_month + 1) % 12;
  }

  return $scheduled_surveys;
}


/**
 * Get oldest survey in system from $user and generate array of every date in
 * DIRT short survey date format (month+year) from collection start date to
 * current date.
 *
 * If no start date manually set in user profile, assume date of oldest
 * collection start date.
 *
 * If no surveys in system, return single entry of current month+year.
 *
 * @param[in] $uid User ID (uid).
 *
 * @retval Array of survey dates in DIRT short survey date format.
 */
function dirt_get_all_possible_survey_dates($uid) {
  // Initialize return array
  $dates = array();

  // Get collection start date from user profile, if present
  $start_date = ''; // default empty

  try {
    $user_wrapper = entity_metadata_wrapper('user', $uid);
    $date_field = 'field_dirt_user_collection_start';
    $start_date = $user_wrapper->$date_field->value();
  }
  catch (EntityMetadataWrapperException $e) {
    watchdog_exception('dirt', $e);
  }

  // If no start date set, use oldest survey date as start date
  // (note this could still be empty if no surveys in system)
  if (empty($start_date)) $start_date = strtotime(dirt_get_earliest_survey_date($uid));
  
  // Re-check whether start date is set
  if (!empty($start_date)) {
    // Extract years/months separately
    $oldest_year = date('Y', $start_date);
    $oldest_month = date('n', $start_date);
    $current_year = date('Y'); // current year
    $current_month = date('n'); // current month

    // Generate array of survey dates in DIRT short survey date format,
    // (month+year), beginning with oldest monthly survey up until current
    // date (reverse chronological order).
    
    // Months from current year:
    // Begin counting down from December
    // For same year, stop at oldest month
    // Otherwise count down to January
    $month_stop = ($current_year == $oldest_year) ? $oldest_month : 1;
    for ($month = $current_month; $month >= $month_stop; $month--) {
      $date = $current_year . '-' . $month;
      $dates[] = date(DIRT_SURVEY_DATE_SHORT_FORMAT, strtotime($date));
    }

    // Months from intermediate years (if any):
    // Begin counting from previous year
    // Count down to one year after survey start year
    for ($year = $current_year - 1; $year > $oldest_year; $year--) {
      // Count months from December down to January
      for ($month = 12; $month >= 1; $month--) {
        $date = $year . '-' . $month;
        $dates[] = date(DIRT_SURVEY_DATE_SHORT_FORMAT, strtotime($date));
      }
    }

    // Months from first year of collection
    // (if different from current year)
    // Count down from December, stopping at oldest survey month
    if ($oldest_year < $current_year) {
      for ($month = 12; $month >= $oldest_month; $month--) {
        $date = $oldest_year . '-' . $month;
        $dates[] = date(DIRT_SURVEY_DATE_SHORT_FORMAT, strtotime($date));
      }
    }

    unset($oldest_year);
    unset($oldest_month);
    unset($current_year);
    unset($current_month);
    unset($month_stop);
  }
  else {
    // There are no surveys in the system for this user,
    // so return an array with a single entry of the current
    // month+year
    $dates[] = date(DIRT_SURVEY_DATE_SHORT_FORMAT);
  }

  unset($user_wrapper);
  unset($date_field);
  unset($start_date);

  return $dates;
}


/**
 * Return node ID of survey with survey date in specified range, belonging to
 * user, of a certain survey type.
 *
 * If no such survey found, returns empty string.
 *
 * @param[in] $date1 Beginning date in range in "Y-m" format.
 * @param[in] $date2 End date in range in "Y-m" format.
 * @param[in] $type Machine name string for survey type.
 * @param[in] $uid User ID (uid).
 *
 * @retval Node ID (nid).
 */
function dirt_get_survey_between_dates($date1, $date2, $type, $uid) {
  // Initialize return
  $nid = '';

  $survey_date_field = 'field_dirt_date_of_survey';

  $query = db_select('node', 'n');
  $query->condition('n.uid', $uid);
  $query->condition('n.type', $type);
  $query->fields('n', array('nid', 'type'));
  $query->leftJoin('field_data_' . $survey_date_field, 'd', 'n.nid = d.entity_id');
  // If date range specified, limit to surveys between dates
  if ($date1) {
    $query->condition('d.' . $survey_date_field . '_value', $date1, '>=');
  }
  if ($date2) {
    $query->condition('d.' . $survey_date_field . '_value', $date2, '<');
  }
  $query->orderBy('n.nid', 'DESC');
  $result = $query->execute()->fetchAssoc(); // expect one result
  if($result) {
    $nid = $result['nid'];
  }

  return $nid;
}


/**
 * Returns array of all surveys authored by user.
 *
 * @param[in] $uid User ID (uid).
 *
 * @retval Array of associative arrays, each with keys:
 *   'nid' => node ID (nid),
 *   'type' => survey type,
 *   'date' => date of survey.
 *   If empty, user has not yet submitted any surveys.
 */
function dirt_get_all_surveys_by_user($uid) {
  // Initialize return array
  $surveys = array();

  // Perform database query
  $survey_date_field = 'field_dirt_date_of_survey';
  $survey_types = dirt_get_survey_types();

  $query = db_select('node', 'n');
  $query->condition('n.uid', $uid);
  $query->condition('n.type', $survey_types, 'IN');
  $query->fields('n', array('nid', 'type'));
  $query->leftJoin('field_data_' . $survey_date_field, 'd', 'n.nid = d.entity_id');
  $query->fields('d', array($survey_date_field . '_value'));
  $query->orderBy('d.' . $survey_date_field . '_value', 'DESC');
  $result = $query->execute()->fetchAll();
  foreach ($result as $result_obj) {
    $nid = $result_obj->nid;
    $date_property = $survey_date_field . '_value';
    $surveys[] = array(
      'nid' => $nid,
      'date' => $result_obj->$date_property,
      'type' => $result_obj->type,
    );
  }

  return $surveys;
}


/**
 * Returns survey history table rows for user (as renderable table array).
 *
 * @param[in] $uid User ID (uid).
 *
 * @retval Array of rows formatted for form element with 'table' theme.
 */
function dirt_get_survey_history_table_rows($uid) {
  // Initialize return array
  $rows = array();

  // Get all surveys submitted by user
  $surveys = dirt_get_all_surveys_by_user($uid);

  // Build $rows from $surveys
  foreach($surveys as $survey) {
    $view_url = 'node/' . $survey['nid'];
    $edit_url = $view_url . '/edit';

    // Get survey date
    $survey_date = date(DIRT_SURVEY_DATE_SHORT_FORMAT, strtotime($survey['date']));

    // Get survey (human-readable) name
    $name = dirt_get_name_of_survey_type($survey['type']);

    // Get update link, if applicable. No update link if survey is admin-only.
    $update_link = t('N/A', array(), array('context' => 'Abbreviation for "not applicable"'));
    $admin_only = dirt_get_admin_only_status_of_survey_type($survey['type']);
    if (!$admin_only) $update_link = l(t('Update'), $edit_url);
    
    $rows[] = array(
      'data' => array(
        array(
          'data' => $survey_date,
          'class' => 'survey-history-table-cell',
        ),
        array(
          'data' => $name,
          'class' => 'survey-history-table-cell',
        ),
        array(
          'data' => l(t('View'), $view_url),
          'class' => 'survey-history-table-cell',
        ),
        array(
          'data' => $update_link,
          'class' => 'survey-history-table-cell',
        ),
      ),
      'class' => array('survey-history-table-row'),
    );
  }

  return $rows;
}


/**
 * Helper function to set scheduled survey text when presented to user.
 *
 * @param[in] $type Machine name string for survey content type.
 * @param[in] $date Date of scheduled survey.
 *
 * @retval String of survey item text.
 */
function _dirt_get_survey_item_data_text($type, $date) {
  // Format survey name and date
  $name = dirt_get_name_of_survey_type($type);

  if ($date) {
    $date_str = date('M Y', strtotime($date));

    return t('@name for <span class="scheduled-survey-date">@date</span> not yet submitted.',
      array('@name' => $name, '@date' => $date_str));
  }
  else {
    return t('@name not yet submitted.', array('@name' => $name));
  }
}


/**
 * Helper function to get link to add scheduled survey.
 *
 * @param[in] $type Machine name string for survey content type.
 *
 * @retval String with link markup.
 */
function _dirt_get_survey_item_submit_link($type) {
  // Get node/add url from survey type by replacing underscores with dashes
  $url = 'node/add/' . str_replace('_', '-', $type);

  return l(t('Submit data'), $url);
}


/**
 * Helper function to get list of months as string from array.
 *
 * @param[in] $months Array of months as numbers (e.g., array('1', '2')).
 *
 * @retval String list of the form "X", "X or Y", or "X, Y, ..., or Z", where
 *   X, Y, ..., Z are the months as full names (e.g., "January").
 *   Returns empty string if $months is empty array.
 */
function _dirt_get_month_list($months) {
  $months_str = ''; // Initialize return

  // Count number of months
  $num_months = count($months);

  // Convert month numbers to full names (e.g., "January")
  $months = array_map('_dirt_month_num_to_str', $months);

  // Remove last month temporarily to add at end
  $last_month = array_pop($months);

  // Get comma-separated string of months
  $months_str = implode(', ', $months);

  // If more than two months, add final comma
  if ($num_months > 2) $months_str .= ',';
  // If more than one month, add "or"
  if ($num_months > 1) $months_str .= ' ' . t('or') . ' ';
  // Add final month at end
  $months_str .= $last_month;

  return $months_str;
}


/**
 * Helper function to convert month numbers to strings, used in array_map().
 *
 * @param[in] $month Integer representing month (e.g., 1).
 *
 * @retval String for full name of month (e.g., "January").
 *
 * @sa _dirt_get_month_list().
 */
function _dirt_month_num_to_str($month) {
  return format_date(mktime(0, 0, 0, $month, 1), 'custom', 'F');
}


/**
 * Helper function to sort survey types by their frequencies in order of most
 * frequently to least frequently collected.
 *
 * @param[in] $type1 Survey type machine name string.
 * @param[in] $type2 Survey type machine name string.
 *
 * @retval Integer for usort() function: 0 if frequencies are equal, -1 if 
 *   $type1 is more frequent, and 1 if $type2 is more frequent.
 */
function _dirt_sort_by_frequency_desc($type1, $type2) {
  $freq1 = dirt_get_collection_frequency_of_survey_type($type1);
  $freq2 = dirt_get_collection_frequency_of_survey_type($type2);

  if ($freq1 == $freq2) return 0;
  return ($freq1 > $freq2) ? -1 : 1;
}

