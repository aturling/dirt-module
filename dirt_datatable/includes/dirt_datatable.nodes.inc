<?php

/**
 * @file
 * This file specifies the definitions for custom nodes created by this module.
 */

/**
 * Returns info on nodes created by this module
 */
function dirt_datatable_get_installed_nodes() {
  // Create data search portal nodes
  // Suggested text is provided but may be easily edited by site admins
  $nodes = array();

  $type = 'dirt_datatable_data_search_page';

  $search_data_path = variable_get('dirt_data_search_portal_path', DIRT_DATA_SEARCH_PATH_DEFAULT);
  $instructions_path = $search_data_path . '/instructions';

  $text  = t('<h3>Search and Display Options</h3>');
  $text .= '<p><ol>';
  $text .= '<li>';
  $text .= t('Use Search Type: On the top left column select how you will do ' .
             'your search based on site location, habitat, or including all ' .
             'sites.');
  $text .= '</li><li>';
  $text .= t('To search by site location: Use the <strong>Map Site Search' .
             '</strong> to view the map of collection sites and search for a ' .
             'specific site based on its location. Click on a site (red ' .
             'icon) to obtain the ID number, habitat type, and county and ' .
             'zip code of the site. Use the <strong>Table Filters</strong> ' .
             '(left column) to search for one or multiple sites based on ' .
             'their IDs. You can also use these filters to narrow your ' .
             'search based on site description parameters, soil health ' .
             'variables, and/or date range, and toggle the visibility of the ' .
             'columns in the table. The table filters will allow you to ' .
             'search by groups of variables related to the different soil ' .
             'health indicators rather than by individual variables.');
  $text .= '</li><li>';
  $text .= t('To search sites by habitat type: Use the <strong>Table ' .
             'Filters</strong> (left column) to select the habitat type(s),' .
             'variables, and date range of interest.');
  $text .= '</li></ol></p><p>';
  $text .= t('Note that the table can be very large. Use the horizontal ' .
             'scrollbar at the bottom of the table to view all of the ' .
             'columns and the vertical scrollbar (if present) to view all of ' .
             'the rows on the current page. The number of rows per page may ' .
             'be changed using the "Show [number] entries" drop-down list ' .
             'below the table. Click on the page numbers or the Previous/' .
             'Next buttons below the table to scroll through the pages of ' .
             'table rows. You may also hide the filter menu to allow the ' .
             'table to expand to the full width of the page by clicking ' .
             '<strong>Hide filter menu</strong> directly above it. (Once ' .
             'hidden, click <strong>Show filter menu</strong> to make it ' .
             'visible again.)');
  $text .= '</p>';
  $text .= t('<h3>Export Data</h3>');
  $text .= '<p>';
  $text .= t('To export the table as a spreadsheet, click on either the ' .
             '<strong>CSV</strong> or <strong>Excel</strong> button above ' .
             'the table to export the table as a CSV (comma-separated ' .
             'values) or Excel file, respectively.<br><em>Note: if you do ' .
             'not have Excel, use the CSV option as that is compatible with ' .
             'all spreadsheet programs.');
  $text .= '</p>';

  $nodes[] = array(
    'type' => $type,
    'title' => t('How to Use the Data Search Portal'),
    'alias' => $instructions_path,
    'body' => $text,
  );

  $text  = t('<h3>Overview</h3>');
  $text .= '<p>';
  $text .= t('Welcome to the @name Data Search Portal. Here you have access ' .
             'to the soil health survey data collected by citizen ' .
             'scientists. This data includes measurements of physical, ' .
             'chemical, and biological soil health indicators from different ' .
             'collection sites. Depending on the soil health indicator, ' .
             'these variables have been measured once a year, twice a year, ' .
             'or monthly between the months of February and November.',
    array('@name' => variable_get('site_name')));
  $text .= '</p>';
  $text .= '<p>';
  $text .= t('You can search and download the data based on site location or ' .
             'habitat type.') . ' ' .
             l(t('See instructions here.'), $instructions_path) .
             ' ' . t('Searched data is presented in a table where each row ' .
             'represents a soil survey organized by collection site (Site ' .
             'ID). The columns in the table represent site description ' .
             'parameters or soil health indicators (variables).');
  $text .= '</p>';
  $text .= t('<h3>Data Access</h3>');
  $text .= '<p>';
  $text .= t('Access to the soil health data is limited based on the data ' .
             'sharing level chosen by the citizen scientists. Some data is ' .
             'available to the general public, while other is only available ' .
             'among participants.');
  $text .= '</p>';

  $nodes[] = array(
    'type' => $type,
    'title' => t('Data Search Portal Overview'),
    'alias' => $search_data_path . '/overview',
    'body' => $text,
  );

  $text  = '<p>';
  $text .= t('Use the table filters below to select the habitat type(s), ' .
             'variables, and date range of your interest. The searched data ' .
             'is presented in a table where each row represents a soil ' .
             'survey organized by collection site (Site ID). The columns in ' . 
             'the table represent site description parameters and soil ' .
             'health indicators (variables).');
  $text .= '</p>';

  $nodes[] = array(
    'type' => $type,
    'title' => t('Search Instructions for Tablet Users'),
    'alias' => $search_data_path . '/instructions-tablet',
    'body' => $text,
  );

  unset($text);

  return $nodes;
}


/**
 * Sets breadcrumbs for data search portal page content types.
 *
 * @param[in,out] $node Node object.
 */
function dirt_datatable_set_data_search_portal_page_breadcrumbs(&$node) {
  $citizen_sci_path = variable_get('dirt_citizen_science_portal_path', DIRT_CITIZEN_SCI_PATH_DEFAULT);
  $search_data_path = variable_get('dirt_data_search_portal_path', DIRT_DATA_SEARCH_PATH_DEFAULT);

  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Citizen Science Soil Health Surveys'), $citizen_sci_path);
  $breadcrumb[] = l(t('Data Search Portal'), $search_data_path);
  $breadcrumb[] = $node->title;

  drupal_set_breadcrumb($breadcrumb);
}

