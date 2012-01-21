/* $Id: suggestedterms.js,v 1.2.2.2 2009/05/21 18:46:12 crell Exp $ */
if (Drupal.jsEnabled) {
  $(document).ready(suggestedterms_build_links);
}

function suggestedterms_build_links() {
  // get all attributes of span tag
  $('span.suggestedterm').each ( function() {
    var a = $('<a>' + this.innerHTML + '</a>')
    .attr('href', '#')
    .addClass($(this).attr('class'))
    .bind('click', function(event) {
      event.preventDefault();
      var input = $(this).parents(".form-item").children('input');
      var text = $(this).text();
      if (((', ' + input.val() + ',').indexOf(', ' + text + ',') < 0) && ((', ' + input.val() + ',').indexOf(', "' + text + '",') < 0)) {
        if ((input.val()).length > 0) {
          input.val(input.val() + ', ');
        }
        input.val(input.val() + text);
      }
    }); // end bind
    $(this).before(a).remove();
  }); // end span.suggestedterm
} // end build_links
