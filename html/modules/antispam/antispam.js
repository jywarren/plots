// $Id: antispam.js,v 1.2 2010/03/28 17:26:33 pixture Exp $

/**
 * @file
 * Provides the administration JavaScript for the AntiSpam module.
 */

/**
 * The AntiSpam administration JavaScript behavior.
 */
Drupal.behaviors.antispam = function(context) {
  // Hide everything but the active provider.
  var active = $('input[name="antispam_service_provider"]:checked').attr('value');
  $('.antispam-wrapper').hide();
  $('#antispam-wrapper-' + active).show();

  // Switch the active provider with user input.
  $('input[name="antispam_service_provider"]').click(function() {
    var active = $(this).attr('value');
    $('.antispam-wrapper').hide();
    $('#antispam-wrapper-' + active).show();
  });
};
