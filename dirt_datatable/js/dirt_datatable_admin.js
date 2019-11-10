(function ($) {

  var currentGroup = '';

  /**
   * DataTable column drag and drop interface. Modeled after block.js code.
   *
   * Move a column in the config table from one column group to another via select list.
   *
   * This behavior is dependent on the tableDrag behavior, since it uses the
   * objects initialized in that behavior to update the row.
   */
  Drupal.behaviors.columnDrag = {
    attach: function (context, settings) {
      // tableDrag is required and we should be on the DataTable columns admin page.
      if (typeof Drupal.tableDrag == 'undefined' || typeof Drupal.tableDrag.columns == 'undefined') {
        return;
      }

      var table = $('table#columns');
      var tableDrag = Drupal.tableDrag.columns;

      // Add a handler for when a row is swapped, update empty column group.
      tableDrag.row.prototype.onSwap = function (swappedRow) {
        checkEmptyColumnGroups(table, this);
      };

      tableDrag.onDrag = function () {
        dragObject = this;

        var columnGroupRow = $(dragObject.rowObject.element).prevAll('tr.column-group-message').get(0);
        var columnGroupName = columnGroupRow.className.replace(/([^ ]+[ ]+)*column-group-([^ ]+)-message([ ]+[^ ]+)*/, '$2');
        var columnGroupField = $('select.column-group-select', dragObject.rowObject.element);

        currentGroup = columnGroupField[0].value;
      }

      // Add a handler so when a row is dropped, update fields dropped into new column groups.
      tableDrag.onDrop = function () {
        dragObject = this;

        var columnGroupRow = $(dragObject.rowObject.element).prevAll('tr.column-group-message').get(0);
        var columnGroupName = columnGroupRow.className.replace(/([^ ]+[ ]+)*column-group-([^ ]+)-message([ ]+[^ ]+)*/, '$2');
        var columnGroupField = $('select.column-group-select', dragObject.rowObject.element);

        // Check whether the newly picked column group is available for this column.
        if ($('option[value=' + columnGroupName + ']', columnGroupField).length == 0) {
          // If not, alert the user and keep the column in its old column group setting.
          alert(Drupal.t('The column cannot be placed in this column group.'));

          // Simulate that there was a selected element change, so the row is put
          // back to from where the user tried to drag it.
          columnGroupField.change();
        }
        else if ($(dragObject.rowObject.element).prev('tr').is('.column-group-message')) {
          var weightField = $('.column-weight', dragObject.rowObject.element);
          var oldColumnGroupName = weightField[0].className.replace(/([^ ]+[ ]+)*column-weight-([^ ]+)([ ]+[^ ]+)*/, '$2');

          if (!columnGroupField.is('.column-group-' + columnGroupName)) {
            columnGroupField.removeClass('column-group-' + oldColumnGroupName).addClass('column-group-' + columnGroupName);
            weightField.removeClass('column-weight-' + oldColumnGroupName).addClass('column-weight-' + columnGroupName);
            columnGroupField.val(columnGroupName);
          }
        }
      };

      // Add the behavior to each column group select list.
      $('select.column-group-select', context).once('column-group-select', function () {
        $(this).change(function (event) {
          // Make our new row and select field.
          var row = $(this).closest('tr');
          var select = $(this);
          tableDrag.rowObject = new tableDrag.row(row);

          var selectVal = select[0].value;

          // This part is different from the block module:
          // If selectVal isn't set, change it to last
          // known column group, force the select to take on
          // that value.
          if (!selectVal) {
            selectVal = currentGroup;
 
            var selectId = select[0].id;
            $('#' + selectId).val(selectVal);

            // Also update weight - it will be set to max to new
            // (invalid) group, set it to max of last known group.
            var cid = selectId.match(/^edit-columns-([0-9]+)-column-group$/);
            if (cid) {
              cidVal = cid[1];

              var maxWeight = 0;
              $('input.column-weight-' + selectVal).each(function() {
                thisWeight = parseInt($(this).val());
                if (thisWeight > maxWeight) {
                  maxWeight = thisWeight;
                }
              });

              $('input#edit-columns-' + cidVal + '-weight').val(maxWeight + 1);
            }
          }

          // Find the correct column group and insert the row as the last in the column group.
          table.find('.column-group-' + selectVal + '-message').nextUntil('.column-group-message').last().before(row);

          // Modify empty column groups with added or removed fields.
          checkEmptyColumnGroups(table, row);
          // Remove focus from selectbox.
          select.get(0).blur();
        });
      });

      var checkEmptyColumnGroups = function (table, rowObject) {
        $('tr.column-group-message', table).each(function () {
          // If the dragged row is in this column group, but above the message row, swap it down one space.
          if ($(this).prev('tr').get(0) == rowObject.element) {
            // Prevent a recursion problem when using the keyboard to move rows up.
            if ((rowObject.method != 'keyboard' || rowObject.direction == 'down')) {
              rowObject.swap('after', this);
            }
          }
          // This column group has become empty.
          if ($(this).next('tr').is(':not(.draggable)') || $(this).next('tr').length == 0) {
            $(this).removeClass('dirt-datatable-column-group-populated').addClass('dirt-datatable-column-group-empty');
          }
          // This column group has become populated.
          else if ($(this).is('.dirt-datatable-column-group-empty')) {
            $(this).removeClass('dirt-datatable-column-group-empty').addClass('dirt-datatable-column-group-populated');
          }
        });
      };
    }
  };

  /**
   * Adds required markup to some fields on column groups admin config page.
   */
  Drupal.behaviors.dirtDataTableAdminColumnGroups = {
    attach: function (context, settings) {
      var fieldClasses = ['form-item-add-column-group-column-group-label',
        'form-item-add-column-group-survey-type',
        'form-item-delete-column-group-group-to-delete',
        'form-item-add-filter-group-filter-group-label',
        'form-item-add-filter-group-survey-type',
        'form-item-delete-filter-group-filter-group-to-delete'];

      var i;
      for(i=0; i<fieldClasses.length; i++) {
        $('.' + fieldClasses[i] + ' label').append('<span class="form-required">*</span>');
      }
    }
  };

})(jQuery);
