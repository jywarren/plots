/* $Id: datepicker.js,v 1.1 2009/07/03 22:00:29 killes Exp $ */
Drupal.behaviors.datepicker = function(context) {
  for(field in Drupal.settings.datepicker.fields) {
    var current_field = Drupal.settings.datepicker.fields[field];
    $('#edit-event-' + current_field + '-exploded-wrapper>div.container-inline').append($('<a></a>').attr({id : 'event-datepicker-' + current_field, href : '#'}).text(Drupal.t('Select date')));
    $('#event-datepicker-' + current_field)
      .datePicker(
        {
          createButton:false,
          startDate:'01/01/2007',
          endDate:'31/12/2012'
        }
      ).bind(
        'click',
        function() {
          updateSelects(this,$(this).dpGetSelected()[0]);
          $(this).dpDisplay();
          return false;
        }
      ).bind(
        'dateSelected',
        function(e, selectedDate, $td, status) {
          updateSelects(this,selectedDate);
        }
      ).bind(
        'dpClosed',
        function(e, selected) {
          updateSelects(this,selected[0]);
        }
      );

    var updateSelects = function (trigger, date) {
      var selectedDate = new Date(date);
      var fieldname = trigger.id.replace('event-datepicker-','');
      $('#edit-event-' + fieldname + '-exploded-day').val(selectedDate.getDate());
      $('#edit-event-' + fieldname + '-exploded-month option[value=' + selectedDate.asString('mm') + ']').attr('selected', 'selected');
      $('#edit-event-' + fieldname + '-exploded-year').val(selectedDate.getFullYear());
    }

    // listen for when the selects are changed and update the picker
    $('#edit-event-' + current_field + '-exploded-day, #edit-event-' + current_field + '-exploded-month, #edit-event-' + current_field + '-exploded-year')
      .bind(
        'change',
        function() {
          var reg_field = new RegExp("^edit-event-([^-]+)-.+",'i');
          var fieldname = this.id.replace(reg_field,'$1');
          var d = new Date(
            $('#edit-event-' + fieldname + '-exploded-year').val(),
            $('#edit-event-' + fieldname + '-exploded-month').val()-1,
            $('#edit-event-' + fieldname + '-exploded-day').val()
          );
          $('#event-datepicker-' + fieldname).dpSetSelected(d.asString());
        }
      );

    // and update the datePicker to reflect it...
    $('#edit-event-' + current_field + '-exploded-day').trigger('change');
  }
};
