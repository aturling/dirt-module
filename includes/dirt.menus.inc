<?php

/**
 * @file
 * This file specifies the definitions for custom menus created by this module.
 */

/**
 * Returns info on menus created by this module.
 *
 * @retval Array of menu information.
 */
function dirt_get_installed_menus() {
  return array(
    'dirt-menu-soil-health-surveys' => array(
      'menu_name' => 'dirt-menu-soil-health-surveys',
      'title' => t('Soil Health Surveys Menu'),
      'description' => t('Submenu for the soil health surveys portal.'),
    ),
  );
}


/**
 * Returns info on menu links created by this module.
 *
 * @retval Array of menu links.
 */
function dirt_get_installed_menu_links() {
  // The first few links are part of the custom soil health surveys
  // submenu. The last is a link to the soil health surveys overview
  // page from the site main menu.

  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);

  return array(
    0 => array(
      'link_path' => drupal_get_normal_path($citizen_sci_path),
      'link_title' => t('Overview'),
      'menu_name' => 'dirt-menu-soil-health-surveys',
      'weight' => 0,
      'options' => array(
        'attributes' => array(
          'class' => array('dirt-menu-link'),
        ),
      ),
    ),
    1 => array(
      'link_path' => drupal_get_normal_path($citizen_sci_path . '/resources'),
      'link_title' => t('Resources'),
      'menu_name' => 'dirt-menu-soil-health-surveys',
      'weight' => 1,
      'options' => array(
        'attributes' => array(
          'class' => array('dirt-menu-link'),
        ),
      ),
    ),
    2 => array(
      'link_path' => drupal_get_normal_path($citizen_sci_path . '/trainingsessions'),
      'link_title' => t('Training Sessions'),
      'menu_name' => 'dirt-menu-soil-health-surveys',
      'weight' => 2,
      'options' => array(
        'attributes' => array(
          'class' => array('dirt-menu-link'),
        ),
      ),
    ),
    3 => array(
      'link_path' => drupal_get_normal_path($citizen_sci_path . '/demographics'),
      'link_title' => t('Demographics'),
      'menu_name' => 'dirt-menu-soil-health-surveys',
      'weight' => 3,
      'options' => array(
        'attributes' => array(
          'class' => array('dirt-menu-link'),
        ),
      ),
    ),
    4 => array(
      'link_path' => drupal_get_normal_path($citizen_sci_path),
      'link_title' => t('Citizen Science'),
      'menu_name' => 'main-menu',
      'weight' => 10,
    ),
  );
}

