<?php

/**
 * @file
 * Search Autocomplete
 * Create a new suggestion to Search Autocomplete suggestion list.
 *
 * @authors
 * Miroslav Talenberg (Dominique CLAUSE) <http://www.axiomcafe.fr/contact>
 *
 * Sponsored by:
 * www.axiomcafe.fr
 */

/**
 * MENU CALLBACK:
 * Define the page to create a new suggestion.
 * @return  A rendered form
 */
function search_autocomplete_suggestion_new() {

  $form = search_autocomplete_suggestion_configure(0);
  return $form;
}

/**
 * MENU CALLBACK:
 * Define the page to configure a suggestion.
 * @return  A rendered form
 */
function search_autocomplete_suggestion_configure($form, $sid = -1) {
  // if no sid provided: get it
  if ($sid == -1)
    $sid = arg(4);

  // default values:
  $title = '';
  $dependancies = '';
  $prefix = '';
  $query = '';

  // if we got a sid: retrieve it from database for edition
  if ($sid > 0) {
    $matches = db_query('SELECT * FROM {search_autocomplete_suggestions} WHERE sid = %d AND sug_fid=0', $sid);
    $match = db_fetch_array($matches);
    $title = $match['sug_title'];
    $dependancies = $match['sug_dependencies'];
    $prefix = $match['sug_prefix'];
    $query = $match['sug_query'];
  }

  // define the form:
  $form = array();
  /* ------------------------------------------------------------------ */
  $from['sid'] = array(
    '#type' => 'hidden',
    '#value' => $sid,
  );
  $form['title'] = array(
    '#title' => t('Title'),
    '#description' => 'Please enter a title for this suggestion',
    '#type' => 'textfield',
    '#default_value' => $title,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $descr =  t('Enter the list of machine name modules which your suggestion depend on (items should be separated with comma ",").') . '<br/>' .
            t('For example, if your suggestion adds taxonomy names, it depends on taxonomy module. Therefore, you should enter: <i>taxonomy</i>.');
  $form['module'] = array(
    '#title' => t('Dependency module'),
    '#description' => $descr,
    '#type' => 'textfield',
    '#default_value' => $dependancies,
    '#maxlength' => 255,
    '#required' => FALSE,
  );
  $descr =  t('This prefix will be added to any suggestion of that type during autocompletion.');
  $form['prefix'] = array(
    '#title' => t('Suggestion prefix'),
    '#description' => $descr,
    '#type' => 'textfield',
    '#default_value' => $prefix,
    '#maxlength' => 255,
    '#required' => FALSE,
  );
  $descr =  t('The query to perform to retrieve suggestions. If you are not sure what to do, please look at examples in <a href="http://projects.axiomcafe.fr/search-autocomplete">the documentation</a> and/or ask for help');
  $form['query'] = array(
    '#title' => t('Query performed to get suggestion'),
    '#description' => $descr,
    '#type' => 'textfield',
    '#default_value' => $query,
    '#size' => 105,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  // submit buton
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#name' => 'submit',
  );
  if ($sid > 0) {     // no need to delete a form that has not been created
    // delete buton
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#name' => 'delete',
    );
  }
  // cancel buton
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#name' => 'cancel',
  );
  return $form;
}

//-------------------------------------------------------------------------------------

function search_autocomplete_suggestion_configure_submit($form, &$form_state) {
  $values = $form_state['values'];
  $sid = arg(4);
  $ok_query = TRUE;

  // if cancel button has send the form: cancel any configuration
  if ($form_state['clicked_button']['#name'] == 'cancel') {
    drupal_set_message(t('No suggestion has been created'), 'info');
    $form_state['redirect'] = 'admin/settings/search_autocomplete';
    return;
  }
  // if delete button has send the form: delete the form
  if ($form_state['clicked_button']['#name'] == 'delete') {
    $form_state['redirect'] = 'admin/settings/search_autocomplete/suggestion/' . $sid . '/delete';
    return;
  }

  // change common values (title, dependencies,...)
  $ok_query &= db_query('UPDATE {search_autocomplete_suggestions} SET sug_title = "%s", sug_dependencies = "%s", sug_query = "%s" WHERE sid=%d',
    $values['title'], $values['module'], $values['query'], $sid);
  $ok_query &= db_query('UPDATE {search_autocomplete_suggestions} SET sug_prefix = "%s" WHERE sid=%d AND sug_fid=0',
    $values['prefix'], $sid);

  // redirect to configuration page
  $form_state['redirect'] = 'admin/settings/search_autocomplete';

  // Give a return to the user
  $ok_query ? drupal_set_message(t('The suggestion has been updated successfully !')) : drupal_set_message(t("An error has occured while updating the suggestion. Please, double check your settings!"), 'error');

}
/**
 * Implementation of hook_submit().
 * Save the new form in database
 */
function search_autocomplete_suggestion_new_submit($form, &$form_state) {
  $ok_query = TRUE; // so far so good
  // if cancel the creation:
  if ($form_state['clicked_button']['#name'] == 'cancel') {
    drupal_set_message(t('No suggestion has been created'), 'info');
    $form_state['redirect'] = 'admin/settings/search_autocomplete';
    return;
  }

  $values = $form_state['values'];

  // get a new possible sid
  $matches = db_query('SELECT sid FROM {search_autocomplete_suggestions} ORDER BY sid DESC');
  $match = db_fetch_array($matches);
  $newsid = $match['sid'] +1;

  // insert the default entry:
  $ok_query &= db_query('INSERT {search_autocomplete_suggestions} SET sid = %d, sug_fid = 0, sug_title = "%s", sug_dependencies = "%s", sug_prefix = "%s", sug_query = "%s"',
    $newsid, $values['title'], $values['module'], $values['prefix'], $values['query']);
  // duplicate the suggestion for as many form as needed:
  $ret= db_query('SELECT fid FROM {search_autocomplete_forms}');
  while ($match = db_fetch_array($ret)) {
    $ok_query &= db_query('INSERT {search_autocomplete_suggestions} SET sid = %d, sug_fid = %d, sug_title = "%s", sug_dependencies = "%s", sug_prefix = "%s", sug_query = "%s"',
      $newsid, $match['fid'], $values['title'], $values['module'], $values['prefix'], $values['query']);
  }

  // redirect to configuration page
  $form_state['redirect'] = 'admin/settings/search_autocomplete';

  // Give a return to the user
  $ok_query ? drupal_set_message(t('The suggestion has been created successfully !')) : drupal_set_message(t("An error has occured while creating the suggestion. Please, double check your settings!"), 'error');
}