<?php

/**
 * @file
 * Default theme implementation to configure DataTable columns.
 *
 * Modeled after block-admin-display-form.tpl.php from Block module.
 *
 * Available variables:
 * - $column_groups: An array of column groups. Keyed by column group ID (gid) with the label as value.
 * - $column_list: An array of columns keyed by column group ID (gid) and then column ID (cid).
 * - $form_submit: Form submit button.
 *
 * Each $column_list[$column_group] contains an array of columns for that column group.
 *
 * Each $data in $column_list[$column_group] contains:
 * - $data->label: Column label.
 * - $data->column_group_select: Drop-down menu for assigning a column group.
 * - $data->weight_select: Drop-down menu for setting weights.
 */
?>
<?php
  // Add table javascript.
  drupal_add_js('misc/tableheader.js');
  drupal_add_js(drupal_get_path('module', 'dirt_datatable') . '/js/dirt_datatable_admin.js');
  foreach ($column_groups as $gid => $label) {
    drupal_add_tabledrag('columns', 'match', 'sibling', 'column-group-select', 'column-group-' . $gid, NULL, FALSE);
    drupal_add_tabledrag('columns', 'order', 'sibling', 'column-weight', 'column-weight-' . $gid);
  }
?>
<table id="columns" class="sticky-enabled">
  <thead>
    <tr>
      <th><?php print t('Column Header Label'); ?></th>
      <th><?php print t('Column Group'); ?></th>
      <th><?php print t('Weight'); ?></th>
      <th><?php print t('Visibility'); ?></th>
    </tr>
  </thead>
  <tbody>
    <?php $row = 0; ?>
    <?php foreach ($column_groups as $gid => $label): ?>
      <tr class="column-group-title column-group-title-<?php print $gid?>">
        <td colspan="4"><?php print $label; ?></td>
      </tr>
      <tr class="column-group-message column-group-<?php print $gid?>-message <?php print empty($column_list[$gid]) ? 'dirt-datatable-column-group-empty' : 'dirt-datatable-column-group-populated'; ?>">
        <td colspan="4"><em><?php print t('(Empty)'); ?></em></td>
      </tr>
      <?php foreach ($column_list[$gid] as $cid => $data): ?>
      <tr class="draggable <?php print $row % 2 == 0 ? 'odd' : 'even'; ?><?php print $data->row_class ? ' ' . $data->row_class : ''; ?>">
        <td class="column"><?php print $data->label; ?></td>
        <td><?php print $data->column_group_select; ?></td>
        <td><?php print $data->weight_select; ?></td>
        <td><?php print $data->visibility_select; ?></td>
      </tr>
      <?php $row++; ?>
      <?php endforeach; ?>
    <?php endforeach; ?>
  </tbody>
</table>

<?php print $form_submit; ?>
