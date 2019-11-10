(function ($) {

  Drupal.behaviors.dirtAdminSurveyTypes = {
    attach: function (context, settings) {
      // Add required markup to some fields
      var fieldClasses = ['form-item-dirt-add-survey-type-type-frequency',
        'form-item-dirt-add-survey-type-type-select',
        'form-item-dirt-delete-survey-type-type-select'];

      var i;
      for(i=0; i<fieldClasses.length; i++) {
        $('.' + fieldClasses[i] + ' label').append('<span class="form-required">*</span>');
      }

      // Expand fieldsets upon form_set_error if collapsed
      if ($('.messages.error').length) {
        var op = Drupal.settings.dirt.op;

        if (($op === 'add') && $('#edit-dirt-add-survey-type').is('.collapsed')) {
          Drupal.toggleFieldset('#edit-dirt-add-survey-type');
        }
        if (($op === 'delete') && $('#edit-dirt-delete-survey-type').is('.collapsed')) {
          Drupal.toggleFieldset('#edit-dirt-delete-survey-type');
        }
      }
    }
  };

})(jQuery);
