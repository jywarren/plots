<?php

/**
 * @file
 * Search Autocomplete
 * Delete a form from Search Autocomplete form list.
 *
 * @authors
 * Miroslav Talenberg (Dominique CLAUSE) <http://www.axiomcafe.fr/contact>
 *
 * Sponsored by:
 * www.axiomcafe.fr
 */

/**
 * Return the filter delete form.
 */
function search_autocomplete_form_delete($form_state) {
  $form = array();

  // get data from database
  $fid = arg(3);

  if (!$fid) {
    drupal_set_message(
      t('The form has not been found, or the menu callback received a wrong parameter.'),
      'error'
    );
    watchdog(
      'search_autocomplete',
      'The form has not been found, or the menu callback received a wrong parameter.',
      NULL,
      WATCHDOG_ERROR
    );

    return $form;
  }

  $form['fid'] = array(
    '#type' => 'hidden',
    '#value' => $fid,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete this form?'),
    'admin/settings/search_autocomplete',
    NULL,
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submission callback for the filter delete form.
 */
function search_autocomplete_form_delete_submit($form, &$form_state) {
  $ok = TRUE;
  $values = $form_state['values'];
  $fid = $values['fid'];
  $parent_fid = $values['parent_fid'];

  $ok .= db_query('UPDATE {search_autocomplete_forms} SET parent_fid = %d WHERE parent_fid = %d', $parent_fid, $fid);
  $ok .= db_query('DELETE FROM {search_autocomplete_forms} WHERE fid = %d', $fid);
  $ok .= db_query('DELETE FROM {search_autocomplete_suggestions} WHERE sug_fid = %d', $fid);

  // Give a return to the user
  $ok ? drupal_set_message(t("The form has been successfully deleted !")) : drupal_set_message(t("An error has occured while deleting the form!"), 'error');

  $form_state['redirect'] = 'admin/settings/search_autocomplete';
}