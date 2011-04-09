// $Id: hide_this_block.js,v 1.1 2010/04/30 13:40:00 digibike Exp $ 


Drupal.HideThisBlock = function() {
  
}


Drupal.HideThisBlock.autoAttach = function() {
  $('a.hide-this-block-ajax').click(function() {
    var a = this;
    href = $(this).attr('href');
    $.ajax({
      type: 'POST',
      data: { js: 1 },
      url: href,
      global: true,
      success: function (data) {
        $(a).parents('.block').slideUp();
      },
      error: function(data) {}
    });
    return false;
  });
}


if (Drupal.jsEnabled) {
  $(document).ready(Drupal.HideThisBlock.autoAttach);
}

