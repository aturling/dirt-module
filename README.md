# DIRT Citizen Science Module

## Introduction

The DIRT Citizen Science Core module provides a web-based data portal for 
collecting data from citizen scientists.

This module was created manage data collection for the 
[MO DIRT](https://modirt.missouriepscor.org/) (Missourians Doing Impact 
Research Together) project, and thus is primarily set up to collect soil health 
data such as soil respiration and chemical soil properties, as well as basic 
information about the collection sites themselves (location, habitat type, land 
management practices, etc.)


## Overview

After installing the module, the web site is initially populated with suggested 
data entry portal pages and data collection surveys. The four survey types are:

* *Group Information Survey*: Information about the Citizen Scientist 
  participants (e.g., group name, group members).
* *Site Description Survey*: Information about the collection site (e.g., site 
  address, GSP coordinates, habitat, land management practices).
* *Monthly Data Survey*: Site variables collected monthly, such as soil 
  respiration and soil water content. Some fields are calculated automatically 
  for the participants on this survey.
* *Twice a Year Survey*: Site variables collected twice a year (defaults to May 
and August of the calendar year), such as soil pH and active carbon.

The DIRT DataTable module extends the DIRT Citizen Science Core module to 
provide a Data Search Portal, a searchable interface for sharing collected soil 
data with the general public. The Data Search Portal consists of a table of all 
collected soil data, searchable by Site ID, habitat type, variable category, 
county, and survey date range, and a map of collection sites (if enabled).


## Uses

* Collecting Citizen Science survey data
Citizen Scientists request data entry accounts via the web site. After setting 
up an account and logging in, they have access to all of the surveys via the 
Data Entry Portal.

* Sharing Citizen Science data with the public
If the DIRT DataTable module is enabled, a Data Search Portal is created that
displays all collected Citizen Science survey data in a searchable table and 
optionally includes a map of all collection sites.

* Exporting Citizen Science data
If the DIRT DataTable module is enabled, the Data Search table includes 
options to export all survey data as a spreadsheet in XLS or CSV format.

* Managing data access
The DIRT Core module provides a user profile field that allows participants to 
control access to their data ("public", "semi-public", i.e., accessible to 
other participants only, or "semi-private"). The most restrictive option is 
called "semi-private" because site administrators have access to all data 
submitted. The "Data Entry Account" role is created as part of this module to 
assign to Citizen Scientist participants, allowing them to view all other 
"semi-public" surveys.
The DIRT DataTable module extends this core functionality to allow members of 
the general public (in particular, soil scientists) to request access to all of 
the data, including the "semi-private" data. The requests are stored in the 
database, pending admin approval. Approved accounts are given the "Data Viewing 
Account" role, also created as part of the module.

* Managing collection schedules
The Data Entry Portal keeps track of the collection start date for each site 
and prompts participants to enter surveys as scheduled (e.g., every month for 
monthly surveys).

* Displaying survey history
The Data Entry Portal keeps track of all surveys submitted for each site. 
Participants may review all of their previously submitted surveys via the 
portal and make changes to survey fields if necessary.

* Educating the public about the project
The module provides a basic site layout with suggested content pages and menus 
to describe the project to the public. Suggested pages include a page to upload 
resources (such as a data entry manual), a page to keep track of upcoming or 
past training sessions (if applicable), and a page to provide basic statistics 
on the project demographics, such as number of active sites, number of 
participating institutions/organizations, etc.


## Requirements

The DIRT module requires the following contributed modules:

* Chaos Tool Suite (https://www.drupal.org/project/ctools)
* Date (https://www.drupal.org/project/date)
* Entity API (https://www.drupal.org/project/entity)
* Field Collection (https://www.drupal.org/project/field_collection)
* Field Collection Table (https://www.drupal.org/project/field_collection_table)
* Field Group (https://www.drupal.org/project/field_group)
* Markup (https://www.drupal.org/project/markup)
* Select (or other) (https://www.drupal.org/project/select_or_other)

The DIRT DataTable module additionally requires these modules:

* jQuery Update (https://www.drupal.org/project/jquery_update)
* Libraries API (https://www.drupal.org/project/libraries)

DIRT DataTable also requires the following jQuery plug-in:

* Chosen (https://harvesthq.github.io/chosen)


## Configuration

After installation, navigate to admin/config/dirt to access the configuration 
pages. The first option, DIRT Survey Configuration, contains configuration 
settings related to the surveys:

* *Survey types*: Displays all current survey types in the system and allows 
  for adding or deleting survey types.

* *Site description surveys*: Choose whether to allow participants to edit 
  their site description survey entries.

* *Duplicate surveys*: Checks for potential duplicate surveys in the system 
  (i.e., two or more surveys of the same type submitted from the same account 
  in the same month and year). Also provides the option of sending e-mail 
  notifications whenever a potential duplicate survey is submitted.

The second option, DIRT Data Entry Portal Configuration, contains configuration 
settings related to the Data Entry Portal:

* *Data Entry Portal settings*: Displays a list of pre-installed data entry 
  portal pages. Also sets the project start year (defaults to year that module 
  was installed).

* *Site ID lookup* Provides a form to search for Site IDs matching a given 
  habitat type and participant full name.

If the DIRT DataTable module is installed, a third option will appear 
containing configuration settings for the Data Search Portal:

* *Summary*: Displays a summary of all collection sites and pre-installed data 
  search portal pages in the system.

* *Map*: Choose whether to display a map of collection sites on the data search 
  portal page and related configuration settings (map center, initial zoom 
  level, etc.).

* *Data Access* Choose whether to allow users to fill out a form to request 
  access to all data (including semi-private data) in the system. Accounts will 
  need to be approved manually by site admins before they are allowed to log in 
  and view data.

* *Data table groups*: Configure the data search table column and filter groups.

* *Data table columns*: Configure the data search table columns.

* *Update data table*: Force a manual update of the data search table database.


## Data Collection

### Data Collection Cycle

The collection cycle goes by calendar year (January to December). Participants 
may join the project at any time and begin collecting at any month. The 
one-time surveys should be filled out as soon as the data collection begins. 
Monthly surveys should be filled out once a month, taking care to submit only 
one survey per calendar month. Twice a year surveys are collected in May and 
August of each calendar year.

While the system has some functionality programmed in to validate the surveys 
and keep track of the survey collection schedule, it is ultimately the 
responsibility of the participants and/or the site administrators to ensure 
that the surveys are being submitted as expected (e.g., at the correct 
frequency, no more than one monthly survey per calendar month, etc.).

Collection frequency is specified by months (e.g., monthly, every 3 months, 
every 6 months, etc.) There is currently no support for collection frequencies 
shorter than 1 month intervals.
Additionally, the DIRT DataTable requires that, for each survey type, no more 
than one survey per account be submitted per month. The Data Search table 
groups together surveys to display all variables collected at a site in a 
single month in the same table row. "Duplicate" surveys (two or more surveys 
of the same type, submitted in the same survey month and year, for the same 
collection site) cannot be grouped together in such a way and will yield 
incorrect results in the search table.
Currently, the DIRT DataTable module does not enforce the one-survey-per-type-
per-month rule, but it does track the duplicate surveys to be reviewed by 
the site admins. There is also an option to automatically send e-mail 
notifications to site admins whenever a duplicate survey is submitted. More 
information on this can be found on the duplicate survey configuration page at 
admin/config/dirt/surveys/duplicates.


### Survey Fields

Each survey consists of two categories of fields. Most of the fields are 
*variables*, i.e., fields containing the collected soil health data (e.g., 
habitat type, soil temperature, soil pH, etc.) The other fields store data 
collection *metadata* (e.g., date of survey). Some of these fields in the 
second category are simply used to format the surveys (e.g., the header markup 
field).

*IMPORTANT:* Each variable should appear in no more than one survey. The 
assumption is that participants will collect variables at various frequencies 
(monthly, twice a year, etc.), so it does not make sense for a particular 
variable to appear more than once. Otherwise, there would be months where 
participants would enter the variable twice in both surveys for the same survey 
date, which could introduce inconsistencies in the collection. The DIRT Core 
fields satisfy this requirement. If any new variable fields are added and/or 
new survey types are created, they must be entirely new fields. (In Drupal 
terminology, the requirement is that each variable field must only have one 
field instance.)

The fields in the second category are those which every survey has in common 
(and thus the rule above does not apply to these fields). For example, every 
survey must have a collection date. The fields assigned to every survey type 
are:

* Date of survey
* Form header
* Form submission instructions

If DIRT DataTable is installed, there are two additional fields attached to 
each survey, visible only to site administrators:

* Include survey in data portal
* Data curation notes

These fields are automatically added to content types which are designated as  
survey types (see next section for more details) and removed when a content 
type is no longer assigned as a survey type.


### Survey types

Content types may be designated as survey types, meaning that they contain data 
collection fields and appear in the Data Entry Portal (and Data Search Portal, 
if DIRT DataTable is installed). Refer to the Overview(#overview) section for a 
list of the core content types with their descriptions.

Survey types may be added or removed via the admin config page at 
admin/config/dirt/surveys. *Note:* Built-in content types may not be deleted 
from the system, but they may be removed as survey types via this form.


## Customizing Content

## Customizing via Web UI

General procedure for adding/removing fields and/or survey content types:

1. Install the DIRT Core module, but do NOT install the DIRT DataTable module 
at this time.

2. Add content types and fields via the Drupal UI as usual. Delete any of the 
non-locked fields from the forms, if desired.

3. Add these new content types as survey types via the DIRT Survey 
Configuration admin form at admin/config/dirt/surveys. This form may also be 
used to remove any of the core survey types.

4. After surveys and fields are set up, the Data Entry Portal will be ready. If 
also using the DIRT DataTable module, install it at this time. All of the 
existing survey fields in the system will automatically be added to the Data
Search Portal table.

5. Core fields will automatically be added to column groups and filter groups, 
but fields created via the UI will need to be added to column groups (and, 
optionally, field groups). Begin with the table column groups configuration at 
admin/config/dirt/data-search/table-groups to add, edit, or remove column 
groups. The same form may be used to update variable filter groups, if desired. 
Then, click on the "Data table columns" tab at 
admin/config/dirt/data-search/table-columns to add columns to column groups. 
This form may also be used to edit column labels and column visibility.

Once the module is installed, no further changes may be made to the Data Search 
Portal columns beyond those allowed on the admin config forms. To add a new 
survey type or field to the table, uninstall the DIRT DataTable module, make 
the changes via the Drupal UI, and reinstall the module.

Uninstalling the DIRT DataTable module will NOT delete any existing surveys in 
the system; it only removes the Data Search Portal page and deletes the 
underlying database tables used in implementing the search portal 
functionality. Likewise, the module must be uninstalled and reinstalled to 
delete a survey type or field.

Refer to the Supported Field Types(#supported-field-types) section for a list 
of field types that may be added to the Data Search Portal table.

*WARNING:* Some DIRT Core fields are required for the site to function 
propertly and therefore must not be deleted. In particular, the following 
fields should not be removed:

User profile fields:
* Data sharing permissions
* Collection status
* Collection start

Survey fields:
* Date of survey
* Form header
* Form submission instructions
* Habitat type
* Geographical Coordinates in Decimal Degrees
* Include survey in data portal
* Data curation notes

Note that markup fields (form header, form submission instructions) can be 
editied via the web UI to customize the markup. Also, any unused fields may 
be hidden in both the form and display on the content type config page, and 
set to hidden in the data search table via the admin config form at
admin/config/dirt/data-search/table-columns.


## Customizing Programmatically

For more advanced Drupal users, an alternative to the above procedure is to 
write a custom module that uses hook_schema_alter() upon installation to update 
the appropriate tables to include new survey types and/or survey fields (either 
added via UI or in the module code). The MO DIRT module uses this method to add 
a new survey type and make customizations specific to the MO DIRT project.

If using the DIRT DataTable module, install it first and define it as a 
dependency in the new add-on module. Add fields either via the Drupal UR or
programmatically in the module. Then, there are three steps to add the new 
field to the data search table:

1. Update the data table schema to add a column for this field in the schema. 
Use the function dirt_datatable_add_column_to_data_table_schema() within 
hook_schema_alter().

Example usage:

```php
$field_name = 'field_mymodule_example';
// Call with last parameter set to TRUE to get schema fields only
$columns = dirt_datatable_get_column_metadata_from_active_node_field($field_name, TRUE);
foreach($columns as $column_name => $column_info) {
  dirt_datatable_add_column_to_data_table_schema($column_info, $schema);
}
```

2. Update the data table in the database to add the column. Use db_add_field().

Example usage:

```php
$new_schema = array();
hook_schema_alter($new_schema);
if (isset($new_schema[DIRT_SEARCH_TABLE_DATA]['fields'])) {
  foreach($new_schema[DIRT_SEARCH_TABLE_DATA]['fields'] as $field_name => $field_spec) {
    db_add_field(DIRT_SEARCH_TABLE_DATA, $field_name, $field_spec);
  }
}
```

3. Add a row to the column metadata table with the info for this field. Use the 
function dirt_datatable_add_column_metadata_for_field().

Example usage:

```php
$field_name = 'field_example';
$columns = dirt_datatable_get_column_metadata_from_active_node_field($field_name);
foreach($columns as $column_name => $column_info) {
  // Optionally specify additional fields in $column_info:
  // $column_info['colgroup'] = $column_group_id;
  // $column_info['weight'] = $weight;
  dirt_datatable_add_column_metadata_for_field($column_info);
}
```


## Supported Field Types

Currently, these are the field types supported by the DIRT DataTable module:

* Boolean
* Date (date module)
* Decimal
* Field Collection (field collection module)
* Float
* Integer
* List (text)
* List (integer)
* List (float)
* Text

*Important:* For field collections, the member fields must be supported types, 
and the cardinality of each member field must be limited to one. The overall 
field collection may have any *finite* cardinality (cannot be  unlimited, as 
the number of database columns must be known in advance). Additionally, it is 
assumed that the first field in the collection defines the field order (e.g., 
replicate number, sample number, etc.) This field is only used to define the 
column order in the database table and is otherwise ignored. The core field 
"Replicate Number" (field_dirt_replicate_number) may be used as the first field.

