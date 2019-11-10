<?php

/**
 * @file
 * Default theme implementation to configure DataTable column groups.
 *
 * Modeled after block-admin-display-form.tpl.php from Block module.
 *
 * Available variables:
 * - $group_list: An array of column groups.
 * - $filter_group_list: An array of variable filter groups.
 * - $edit_column_groups_header: Header for edit column groups table
 * - $edit_filter_groups_header: Header for edit filter groups table
 * - $group_form_submit: Form submit button for column groups.
 * - $filter_group_form_submit: Form submit button for filter groups.
 *
 * Each $data in $group_list contains:
 * - $data->row_class: Row CSS class.
 * - $data->label: Column label.
 * - $data->filter_group_select: Drop-down menu for assigning a filter group (optional).
 * - $data->weight_select: Drop-down menu for setting weights.
 *
 * Each $data in $filter_list contains:
 * - $data->row_class: Row CSS class.
 * - $data->label: Filter group label.
 * - $data->weight_select: Drop-down menu for setting weights.
 */
?>
<?php
  // Add table javascript.
  drupal_add_js('misc/tableheader.js');
  drupal_add_js(drupal_get_path('module', 'dirt_datatable') . '/js/dirt_datatable_admin.js');
  drupal_add_tabledrag('groups', 'order', 'sibling', 'column-group-weight');
  drupal_add_tabledrag('filters', 'order', 'sibling', 'filter-group-weight');
?>

<?php print $edit_column_groups_header; ?>
<table id="groups" class="sticky-enabled">
  <thead>
    <tr>
      <th><?php print t('Column Group Header Label'); ?></th>
      <th><?php print t('Weight'); ?></th>
      <th><?php print t('Filter Group'); ?></th>
    </tr>
  </thead>
  <tbody>
    <?php $row = 0; ?>
    <?php foreach ($group_list as $idx => $data): ?>
    <tr class="draggable <?php print $row % 2 == 0 ? 'odd' : 'even'; ?><?php print $data->row_class ? ' ' . $data->row_class : ''; ?>">
      <td class="column-group"><?php print $data->label; ?></td>
      <td><?php print $data->weight_select; ?></td>
      <td><?php print $data->filter_group_select; ?></td>
    </tr>
    <?php $row++; ?>
    <?php endforeach; ?>
  </tbody>
</table>

<?php print $group_form_submit; ?>

<?php print $edit_filter_groups_header; ?>
<table id="filters" class="sticky-enabled">
  <thead>
    <tr>
      <th><?php print t('Filter Group Label'); ?></th>
      <th><?php print t('Weight'); ?></th>
    </tr>
  </thead>
  <tbody>
    <?php $row = 0; ?>
    <?php foreach ($filter_group_list as $idx => $data): ?>
    <tr class="draggable <?php print $row % 2 == 0 ? 'odd' : 'even'; ?><?php print $data->row_class ? ' ' . $data->row_class : ''; ?>">
      <td class="filter-group"><?php print $data->label; ?></td>
      <td><?php print $data->weight_select; ?></td>
    </tr>
    <?php $row++; ?>
    <?php endforeach; ?>
  </tbody>
</table>

<?php print $filter_group_form_submit; ?>

<?php print $form_submit; ?>
