if(Drupal.jsEnabled) {
  $(document).ready(function(){
    Drupal.CTools.AJAX.commands.modal_dismiss = null;
    Drupal.CTools.AJAX.commands.modal_dismiss = function(){
      Drupal.CTools.Modal.dismiss();
      window.location.reload(true);
    }
  });
}
