<?php
// $Id: to_do.pages.inc,v 1.1.2.14 2010/11/23 08:27:51 alexiswilke Exp $

/**
 * @file
 * Generate the to_do pages and teasers.
 */

/**
 * Generate the form for a To Do node.
 */
function _to_do_form(&$node, $form_state) {
  global $user;

  // deleting?
  // (note: if you use the mini module, the 'delete' button is removed and thus undefined here)
  if (isset($form_state['values']['op']) && isset($form_state['values']['delete'])
      && $form_state['values']['op'] == $form_state['values']['delete']) {
    drupal_goto('node/' . $node->nid . '/delete');
  }

  _to_do_fix_dates($node);

  // initialize the form with the default node data
  $form = node_content_form($node, $form_state);

  // TO DO fields
  $form['to_do'] = array(
    '#type' => 'fieldset',
    '#title' => t('To do'),
  );

  // if the item_status field is defined, we're editing
  if (isset($node->item_status)) {
    if ($node->item_status == TO_DO_STATUS_STARTED
        && variable_get('to_do_started_to_in_progress', 0)) {
      $item_status = TO_DO_STATUS_IN_PROGRESS;
    }
    else {
      $item_status = $node->item_status;
    }
    $priority = $node->priority;
    $start_date_flag = $node->start_date != 0;
    $deadline_flag = $node->deadline != 0;
    $auto_close = $node->deadline != 0 && $node->auto_close;
    $self = $node->self;
    $mark_permissions = $node->mark_permissions;
  }
  else {
    // use defaults in this case
    $item_status = variable_get('to_do_default_status', TO_DO_STATUS_NOT_STARTED);
    $priority = variable_get('to_do_default_priority', TO_DO_PRIORITY_MEDIUM);
    $start_date_flag = variable_get('to_do_default_include_start', 0);
    $deadline_flag = variable_get('to_do_default_include_deadline', 0);
    $auto_close = variable_get('to_do_default_auto_close', 0);
    $self = variable_get('to_do_default_assign_self', 0);
    $mark_permissions = variable_get('to_do_default_mark_perms', 0);
  }

  $form['to_do']['item_status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => _to_do_status_list(),
    '#default_value' => $item_status,
  );
  $form['to_do']['priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#options' => _to_do_priority_list(),
    '#default_value' => $priority,
  );

  // TO DO dates
  drupal_add_js(drupal_get_path('module', 'to_do') . '/scripts/to_do_form.js');

  $form['dates'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dates'),
    '#prefix' => '<div class="to-do-dates">',
    '#suffix' => '</div>',
  );
  $form['dates']['start_date_wrapper'] = array(
    '#prefix' => '<div class="to-do-date-wrapper">',
    '#suffix' => '</div>',
  );
  $form['dates']['start_date_wrapper']['include_start_date'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include a start date'),
    '#default_value' => $start_date_flag,
    '#prefix' => '<div class="to-do-date-switch">',
    '#suffix' => '</div>',
  );
  $start_date = $node->start_date == 0 ? time() : $node->start_date;
  $form['dates']['start_date_wrapper']['start_date'] = array(
    '#type' => 'date',
    '#title' => t('Start date'),
    '#default_value' => array(
              'day'   => format_date($start_date, 'custom', 'j'),
              'month' => format_date($start_date, 'custom', 'n'),
              'year'  => format_date($start_date, 'custom', 'Y')),
    '#prefix' => '<div class="to-do-conditional to-do-start-date">',
    '#suffix' => '</div>',
  );
  $form['dates']['deadline_wrapper'] = array(
    '#prefix' => '<div class="to-do-date-wrapper">',
    '#suffix' => '</div>',
  );
  $form['dates']['deadline_wrapper']['include_deadline'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include a deadline'),
    '#default_value' => $deadline_flag,
    '#prefix' => '<div class="to-do-date-switch">',
    '#suffix' => '</div>',
  );
  // the deadline and the auto-close flag are both to be hidden when the switch is off
  $form['dates']['deadline_wrapper']['deadline_conditional_wrapper'] = array(
    '#prefix' => '<div class="to-do-conditional">',
    '#suffix' => '</div>',
  );
  // by default, use current time +1 day
  $deadline = $node->deadline == 0 ? time() + 86400 : $node->deadline;
  $form['dates']['deadline_wrapper']['deadline_conditional_wrapper']['deadline'] = array(
    '#type' => 'date',
    '#title' => t('Deadline'),
    '#default_value' => array(
              'day'   => format_date($deadline, 'custom', 'j'),
              'month' => format_date($deadline, 'custom', 'n'),
              'year'  => format_date($deadline, 'custom', 'Y')),
    '#prefix' => '<div class="to-do-deadline">',
    '#suffix' => '</div>',
  );
  // clear the auto-close if the deadline was cleared
  $form['dates']['deadline_wrapper']['deadline_conditional_wrapper']['auto_close'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically mark this listing closed after the deadline'),
    '#default_value' => $auto_close,
  );



  // TO DO assignments
  $form['assignment'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assignment'),
    '#prefix' => '<div id="assignment_wrapper">',
    '#suffix' => '</div>',
  );
  $form['assignment']['self'] = array(
    '#type' => 'checkbox',
    '#title' => t('Assign to self'),
    '#default_value' => $self,
  );

  // when there is a list of users assigned to the node,
  // display that list so one can remove some users
  // (note that we ignore "self" from that list.)
  if (isset($node->assigned_users) && count($node->assigned_users)) {
    $users_selection = array();
    $users_options = array();
    foreach ($node->assigned_users as $uid => $name) {
      if ($uid != $user->uid) {
        $users_selection[$uid] = $uid;
        $users_options[$uid] = $name;
      }
    }
    if (count($users_selection)) {
      $form['assignment']['assigned_users'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Assigned to'),
        '#default_value' => $users_selection,
        '#options' => $users_options,
        '#description' => t('Unselect the users that should be have this To do item assigned to them.'),
      );
    }
  }

  // then we offer 3 entries that the user can fill up to assign this task to new users
  for ($i = 0; $i < 3; ++$i) {
    $form['assignment']['new_user' . $i] = array(
      '#type' => 'textfield',
      '#size' => 30,
      '#maxlength' => 60,
      '#autocomplete_path' => 'to_do/autocomplete/users',
    );
  }
  $form['assignment']['new_user0']['#title'] = t('Add users');
  $form['assignment']['new_user2']['#description'] = t('Enter the user names of the users to assign this To do item. If you want to assign more than 3 users, save this many and click Edit to add another 3.');

  $form['assignment']['mark_permissions'] = array(
    '#type' => 'radios',
    '#title' => t('The following user(s) can mark this listing as finished'),
    '#options' => array(
      0 => t('Only me'),
      1 => t('Any assigned user'),
    ),
    '#default_value' => $mark_permissions,
  );

  return $form;
}

/**
 * Verify the data in a To do form.
 */
function _to_do_validate($node, $form) {
  // we fix the dates as if we were saving the data which we're about to do...
  // a start date or deadline of 0 is valid and in that case they are ignored for
  // the test (i.e. a deadline of 0 is not considered earlier than any start date)
  _to_do_fix_dates($node);
  if ($node->start_date && $node->deadline && $node->deadline < $node->start_date) {
    form_set_error('dates', t('The deadline cannot be earlier than the start date'));
  }

  // verify that it was properly assigned (also if a user gets deleted...)
  _to_do_validate_assignment($node);
}

/**
 * Check whether a valid assignment of the TO DO item was made
 * in the form.
 */
function _to_do_validate_assignment($node) {
  // admin can always assign to anyone but if there are no roles
  // who can get an assignment, he still must assign to oneself.
  //if (!isset($node->roles)) {
  //  // when no roles, we must assign to ourselves
  //  // since we don't have the right to do assignments to anyone else
  //  form_set_error('assignment', t('You must assign this listing to yourself'));
  //  return;
  //}

  // check whether a new user is being added (they should be valid users...)
  $found = $node->self;
  $roles = _to_do_roles();
  if (!empty($roles)) for ($i = 0; $i < 3; ++$i) {
    $field = 'new_user' . $i;
    if (!empty($node->$field)) { // this means a user named "0" cannot be accepted...
      // if authenticated user (role 2) has permission, all roles do, don't need to check db
      if ( in_array( 2 , array_keys($roles) ) ) {
        $found = TRUE;
      }
      else {
        $sql = "SELECT u.uid FROM {users} u, {users_roles} r"
          . " WHERE u.name = '%s' AND r.uid = u.uid"
            . " AND r.rid IN (" . db_placeholders($roles, 'int') . ")";
        $uid = db_result(db_query($sql, array_merge(array($node->$field), array_flip($roles))));
        if ($uid) {
          $found = TRUE;
        }
        else {
          form_set_error($field,
            t('User "@user" either does not exist or he does not have permission to access this To do items.',
              array('@user' => $node->$field)));
          unset($node->$field);
        }
      }
    }
  }
  if ($found) {
    return;
  }

  // check whether a user was previously assigned this To do item
  // and is still assigned this item
  if (isset($node->assigned_users)) {
    foreach ($node->assigned_users as $item) {
      // Note: $item may be the $uid or the $name of the users
      if ($item) {
        return;
      }
    }
  }

  // no one was assign anything!
  form_set_error('assignment',
    t('You must assign this To do item to at least one user, this can either be you or another user.'));
}

/**
 * Save a new To do item in the database.
 */
function _to_do_insert($node) {
  _to_do_fix_dates($node);

  // Insert a new to_do entry
  $query = 'INSERT INTO {to_do} (vid, nid, item_status, priority';
  $values = '%d, %d, %d, %d';
  $arguments = array($node->vid, $node->nid, $node->item_status, $node->priority);
  if ($node->start_date) {
    $query .= ', start_date';
    $values .= ", %d";
    $arguments[] = $node->start_date;
  }
  if ($node->deadline) {
    $query .= ', deadline, auto_close';
    $values .= ", %d, %d";
    $arguments[] = $node->deadline;
    $arguments[] = $node->auto_close;
  }
  if ($node->item_status == TO_DO_STATUS_FINISHED || $node->item_status == TO_DO_STATUS_CANCELED) {
    $query .= ', date_finished';
    $values .= ', %d';
    $arguments[] = time();
  }
  if (!empty($node->mark_permissions)) {
    $query .= ', mark_permissions';
    $values .= ', %d';
    $arguments[] = 1;
  }
  $query .= ') VALUES (' . $values . ')';
  db_query($query, $arguments);

  _to_do_save_users($node);

  _to_do_save_message($node, TRUE);
}

function _to_do_save_users($node) {
  global $user;

  // Insert a to_do_assigned_users
  $query = 'INSERT INTO {to_do_assigned_users} (nid, vid, uid) VALUES';
  $arguments = array();

  // avoid duplicates in the INSERT INTO
  $found_users = array();

  if (isset($node->assigned_users)) {
    foreach ($node->assigned_users as $uid => $name) {
      // WARNING: $name may be equal to $uid here
      if ($name) {
        if ($uid == $user->uid) {
          // make sure we don't user self twice
          $node->self = 1;
        }
        elseif ($uid > 0 && empty($found_users[$uid])) {
          // we do not need to verify the user
          $arguments[] = $node->nid;
          $arguments[] = $node->vid;
          $arguments[] = $uid;
          $found_users[$uid] = TRUE;
        }
      }
    }
  }

  for ($i = 0; $i < 3; ++$i) {
    // here we do not need to check the validity since it was
    // done in the to_do_validate() function already.
    $field = 'new_user' . $i;
    if (!empty($node->$field)) {
      $sql = "SELECT uid FROM {users} WHERE name = '%s'";
      $uid = db_result(db_query($sql, $node->$field));
      if ($uid == $user->uid) {
        $node->self = 1;
      }
      elseif ($uid > 0 && empty($found_users[$uid])) {
        $arguments[] = $node->nid;
        $arguments[] = $node->vid;
        $arguments[] = $uid;
        $found_users[$uid] = TRUE;
      }
    }
  }

  if ($node->self) {
    $arguments[] = $node->nid;
    $arguments[] = $node->vid;
    $arguments[] = $user->uid;
  }

  db_lock_table('to_do_assigned_users');

  $sql = "DELETE FROM {to_do_assigned_users} WHERE nid = %d AND vid = %d";
  db_query($sql, $node->nid, $node->vid);

  $user_count = count($arguments) / 3;
  if ($user_count) {
    $query .= substr(str_repeat(', (%d, %d, %d)', $user_count), 1);
    db_query($query, $arguments);
  }

  db_unlock_tables();
}

/**
 * Update the data of a To do item.
 */
function _to_do_update($node) {
  global $user, $db_type;

  _to_do_fix_dates($node);

  if (!empty($node->revision)) {
    to_do_insert($node);
    return;
  }

  $sql = "SELECT item_status FROM {to_do} WHERE vid = %d";
  $old_status = db_result(db_query($sql, $node->vid));

  $query = "UPDATE {to_do} SET item_status = %d, priority = %d";
  $arguments = array($node->item_status, $node->priority);
  if ($node->start_date) {
    $query .= ", start_date = %d";
    $arguments[] = $node->start_date;
  }
  else {
    $query .= ", start_date = NULL";
  }
  if ($node->deadline) {
    $query .= ", deadline = %d, auto_close = %d";
    $arguments[] = $node->deadline;
    $arguments[] = $node->auto_close;
  }
  else {
    $query .= ", deadline = NULL, auto_close = 0";
  }
  if ($node->item_status == TO_DO_STATUS_FINISHED || $node->item_status == TO_DO_STATUS_CANCELED) {
    $query .= ', date_finished = %d';
    $arguments[] = !empty($node->date_finished) ? $node->date_finished : time();
  }
  else {
    $query .= ', date_finished = NULL';
  }
  $query .= ', mark_permissions = %d';
  $arguments[] = empty($node->mark_permissions) ? 0 : 1;
  $query .= ' WHERE vid = %d';
  $arguments[] = $node->vid;
  db_query($query, $arguments);
  if (db_affected_rows() == 0) {
    // if for whatever reason the UPDATE fails, do an INSERT
    // yet, MySQL may return 0 affected rows even when 1 existed
    // (i.e. "affected" rows and not "matched" rows)
    if ($db_type == 'mysql' || $db_type == 'mysqli') {
      // no need to use range because vid is the primary key
      $result = db_result(db_query("SELECT 1 FROM {to_do} WHERE vid = %d", $node->vid));
    }
    else {
      $result = 0;
    }
    if (!$result) {
      to_do_insert($node);
      return;
    }
    // if the row already exists, we need to continue!
  }

  _to_do_save_users($node);

  _to_do_save_message($node, $old_status != $node->status);
}

/**
 * Invoke other modules to_do_started or to_do_finished callbacks
 * and eventually post a message if the node status does not correspond
 * to the To Do item status.
 */
function _to_do_save_message($node, $invoke) {
  // signal other modules?
  if ($invoke) {
    if ($node->item_status == TO_DO_STATUS_STARTED) {
      module_invoke_all('to_do_started', 'to_do', $node);
    }
    elseif ($node->item_status == TO_DO_STATUS_FINISHED) {
      module_invoke_all('to_do_finished', 'to_do', $node);
    }
  }

  // node.status & to_do.status mismatch?
  if (isset($node->include_start_date) && !$node->status) {
    switch ($node->item_status) {
    case TO_DO_STATUS_NOT_STARTED:
    case TO_DO_STATUS_FINISHED:
    case TO_DO_STATUS_CANCELED:
      break;

    default:
      drupal_set_message('You just created or updated a To Do item which is unpublished and is not in a Finished or Canceled state.', 'warning');
      break;

    }
  }
}

// vim: ts=2 sw=2 et syntax=php
