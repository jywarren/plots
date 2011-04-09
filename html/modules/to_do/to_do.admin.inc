<?php
// $Id: to_do.admin.inc,v 1.1.2.8 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * Setup menu and forms for the to_do module.
 */

/**
 * Actual implementation of hook_menu().
 *
 * This function generates the array of menu items.
 */
function to_do_menu_array() {
  $menu = array();

  // parent settings
  $menu['admin/settings/to_do'] = array(
    'title' => 'To do list',
    'description' => 'Settings for the to do module',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('to_do_settings_form'),
    'access arguments' => array('administer to do list'),
    'file' => 'to_do.admin.inc',
  );

  // global settings
  $menu['admin/settings/to_do/settings'] = array(
    'title' => 'Settings',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -100,
  );

  // default new to do settings
  $menu['admin/settings/to_do/defaults'] = array(
    'title' => 'Defaults',
    'description' => 'Defaults used when creating new To do tasks',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('to_do_defaults_form'),
    'access arguments' => array('administer to do list'),
    'file' => 'to_do.admin.inc',
    'type' => MENU_LOCAL_TASK
  );

  // main list (like a user call, this is a redirect)
  $menu['to_do'] = array(
    'title' => 'To do list',
    'description' => 'A listing of all To Do listings on the site',
    'page callback' => 'to_do_list_redirect',
    'access callback' => '_to_do_redirect_access_callback',
    'file' => 'to_do.users.inc',
  );

  // user lists
  $menu['user/%/to_do'] = array(
    'title' => 'To do list',
    'description' => 'A list of things that need to be done',
    'page callback' => 'to_do_list',
    'page arguments' => array(1),
    'access callback' => '_to_do_access_callback',
    'access arguments' => array(1),
    'file' => 'to_do.users.inc',
    'type' => MENU_LOCAL_TASK
  );
  $menu['user/%/to_do/list'] = array(
    'title' => 'To do list',
    'description' => 'A list of things that need to be done',
    'weight' => -1,
    'file' => 'to_do.users.inc',
    'type' => MENU_DEFAULT_LOCAL_TASK
  );
  $menu['user/%/to_do/created'] = array(
    'title' => 'Created to do list items',
    'description' => 'A list of to do items created by the user',
    'page callback' => 'to_do_list_created',
    'page arguments' => array(1),
    'access arguments' => array('create to do content'),
    'file' => 'to_do.users.inc',
    'type' => MENU_LOCAL_TASK,
  );
  $menu['user/%/to_do/all'] = array(
    'title' => 'All to do listings',
    'description' => 'A list of all to do items',
    'page callback' => 'to_do_list_all',
    'page arguments' => array(1),
    'access arguments' => array('access all to do content'),
    'file' => 'to_do.users.inc',
    'type' => MENU_LOCAL_TASK,
  );

  // auto complete user names
  $menu['to_do/autocomplete/users'] = array(
    'title' => 'Autocomplete user names and identifier',
    'page callback' => 'to_do_autocomplete_users',
    // access not properly checked on callbacks (see function)
    'access callback' => '_to_do_access_autocomplete_callback',
    'file' => 'to_do.users.autocomplete.inc',
    'type' => MENU_CALLBACK,
  );

  return $menu;
}

/**
 * Generate the TO DO administration form.
 */
function to_do_settings_form($form_state) {
  $form['to_do_description_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Description title'),
    '#description' => t('Title shown right before the description of your To Do list items.'),
    '#default_value' => variable_get('to_do_description_title', t('Details')),
  );

  $form['to_do_fieldset_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Field set title'),
    '#description' => t('Title used for the task <em>fieldset</em>. If empty, do not use a fieldset.'),
    '#default_value' => variable_get('to_do_fieldset_title', t('To Do Listing')),
  );

  $form['to_do_date_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Date Format'),
    '#description' => t("Enter the format to be used for dates displayed by the To Do List module. Use a !php_date format string.<br />Note: No hours, minutes, or seconds are provided with dates in this module, so don't include these in your format string.", array('!php_date' => '<a href="http://www.php.net/date">' . t('PHP date') . '</a>')),
    '#default_value' => variable_get('to_do_date_format', 'Y/n/j'),
  );

  $form['to_do_number_of_listings'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of items in lists shown in user accounts'),
    '#description' => t('Enter the number of listings to be displayed on pages that display lists of to do listings. This number is independent from block settings, which are set on a per/user basis in their control panel'),
    '#default_value' => variable_get('to_do_number_of_listings', 10),
  );

  $form['to_do_show_action_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show action buttons'),
    '#description' => t('When set, show action buttons on the page that the author or assigned users can click to change the status of the To Do item.'),
    '#default_value' => variable_get('to_do_show_action_button', 1),
  );

  $form['to_do_mark_finished_actions'] = array(
    '#type' => 'textfield',
    '#title' => t('Go To Action on Mark Finished'),
    '#description' => t('Enter a list of actions, the first that is valid at the time the user click Mark Finished is used. The default is to do nothing (empty). A comma separated list of the following works:')
      . '<br /><br />'
      . t('first, last, next, previous, front, list, /path/to/destination')
      . '<br /><br />'
      . t('For instance, to go to the previous task if there is one, the next otherwise and if that was the last task, go to the front page use: previous, next, front.')
      . '<br /><br />'
      . t('Note that front and /path/to/destination are terminal and should not be followed by anything. The path cannot include a space. list is terminal except for anonymous users.'),
    '#default_value' => variable_get('to_do_mark_finished_actions', ''),
  );

  $form['to_do_side_actions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show actions on the side'),
    '#description' => t('Check this box to move the actions to the side instead of the bottom.'),
    '#default_value' => variable_get('to_do_side_actions', 0),
  );

  $form['to_do_show_info_after'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show info after description'),
    '#description' => t('Show the To Do item information after the description when checked. WARNING: You may need to clear your cache to see the effect of this flag.'),
    '#default_value' => variable_get('to_do_show_info_after', 1),
  );

  $form['to_do_started_to_in_progress'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto-change "Started" to "In Progress"'),
    '#description' => t('When saving a To Do item with its status set to Started, you generate a Started event. To avoid this side effect each time you edit your To Do item, check this box.'),
    '#default_value' => variable_get('to_do_started_to_in_progress', 0),
  );

  // up to 3 reminders
  for ($i = 1; $i <= 3; ++$i) {
    $form['to_do_reminder' . $i] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('to_do_reminder' . $i, ''),
    );
  }
  $form['to_do_reminder1']['#title'] = t('Reminder events');
  $form['to_do_reminder3']['#description'] = t('Number of days before the event deadline is reached when an event reminder is posted. Leave empty to not send a reminder.');

  $form['to_do_unpublish_after'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of days before unpublishing'),
    '#description' => t('Once a To do item was marked finished, it will automatically be unpublished after this many days. Enter nothing to avoid this feature.'),
    '#default_value' => variable_get('to_do_unpublish_after', ''),
  );

  $form['to_do_delete_after'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of days before deleting'),
    '#description' => t('Once a To do item was marked finished, it will automatically be deleted after this many days. Enter nothing to avoid this feature.'),
    '#default_value' => variable_get('to_do_delete_after', ''),
  );

  $form['to_do_delete_any'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete any To do item'),
    '#description' => t('By default, only items that were unpublished get deleted. Select this option to delete all items even those that are still published.'),
    '#default_value' => variable_get('to_do_delete_any', 0),
  );

  return system_settings_form($form);
}


/**
 * Make sure that the number of listings is a number.
 */
function to_do_settings_form_validate($form, &$form_state) {
  // as we're at it, reset the Block page to 1
  //$_SESSION['to_do_page'] = 1; -- this would be helpful only to the admin...
  // instead we added a fix in the generation of the block, slow but works for everyone
  // and even when just an item is marked finished!

  if (!preg_match('/^[0-9]+$/', $form_state['values']['to_do_number_of_listings'])) {
    form_set_error('to_do_number_of_listings', t('The number of listings must be a number.'));
  }

  for ($i = 1; $i <= 3; ++$i) {
    if (!preg_match('/^[0-9]*$/', $form_state['values']['to_do_reminder' . $i])) {
      form_set_error('to_do_reminder' . $i, t('The number of days before the deadline reminder must be a positive number.'));
    }
  }

  $compare_days = TRUE;
  if (!preg_match('/^[0-9]*$/', $form_state['values']['to_do_unpublish_after'])) {
    form_set_error('to_do_unpublish_after', t('The number of days before unpublishing must be a number.'));
    $compare_days = FALSE;
  }
  if (!preg_match('/^[0-9]*$/', $form_state['values']['to_do_delete_after'])) {
    form_set_error('to_do_delete_after', t('The number of days before deleting must be a number.'));
    $compare_days = FALSE;
  }

  // note that if empty we accept the entry since it means that we won't act
  if ($compare_days
      && !empty($form_state['values']['to_do_unpublish_after'])
      && !empty($form_state['values']['to_do_delete_after'])
      && $form_state['values']['to_do_unpublish_after'] > $form_state['values']['to_do_delete_after']) {
    form_set_error('to_do_delete_after', t('The number of days before deleting must be larger than the number of days before unpublishing.'));
  }

  $valid_actions = array(
    'first' => 1,
    'last' => 1,
    'next' => 1,
    'previous' => 1,
    'front' => 1,
    'list' => 1,
  );
  $list = explode(',', str_replace(' ', '', $form_state['values']['to_do_mark_finished_actions']));
  foreach ($list as $action) {
    if (empty($action)) {
      // accept empty actions (ending comma or double comma)
      continue;
    }
    if (empty($valid_actions[$action]) && $action[0] != '/') {
      form_set_error('to_do_mark_finished_actions',
        t('The list of actions is invalid: "@s". Please check your spelling.', array('@s' => $action)));
    }
  }
}

/**
 * Generate the TO DO default settings form.
 */
function to_do_defaults_form($form_state) {
  $form['to_do_default_status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#description' => t('Default status for new to do items.'),
    '#options' => _to_do_status_list(),
    '#default_value' => variable_get('to_do_default_status', TO_DO_STATUS_NOT_STARTED),
  );

  $form['to_do_default_priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#description' => t('Default priority for new to do items.'),
    '#options' => _to_do_priority_list(),
    '#default_value' => variable_get('to_do_default_priority', TO_DO_PRIORITY_MEDIUM),
  );

  $form['to_do_default_include_start'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include start date'),
    '#description' => t('Include a start date by default for new to do items.'),
    '#default_value' => variable_get('to_do_default_include_start', 0),
  );

  $form['to_do_default_include_deadline'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include deadline'),
    '#description' => t('Include a deadline by default for new to do items.'),
    '#default_value' => variable_get('to_do_default_include_deadline', 0),
  );

  $form['to_do_default_auto_close'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto-close'),
    '#description' => t('Auto-close new to do items after the deadline by default.'),
    '#default_value' => variable_get('to_do_default_auto_close', 0),
  );

  $form['to_do_default_assign_self'] = array(
    '#type' => 'checkbox',
    '#title' => t('Assign to self'),
    '#description' => t('Default setting for "assign to self" for new to do items.'),
    '#default_value' => variable_get('to_do_default_assign_self', 0),
  );

  $form['to_do_default_mark_perms'] = array(
    '#type' => 'radios',
    '#title' => t('Mark permissions'),
    '#description' => t('Default setting for who can mark the new to do finished.'),
    '#options' => array(
      0 => t('Only me'),
      1 => t('Any assigned user'),
    ),
    '#default_value' => variable_get('to_do_default_mark_perms', 0),
  );

  return system_settings_form($form);
}

// vim: ts=2 sw=2 et syntax=php
