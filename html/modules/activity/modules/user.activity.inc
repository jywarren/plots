<?php
// $Id: user.activity.inc,v 1.1.2.18 2010/07/18 19:03:39 scottreynolds Exp $

/**
 * @file
 * Activity definition file for user.module
 */

/**
 * Implementation of hook_activity_type_check().
 */
function user_activity_type_check($token_objects, $types) {
  // $types has the role id as values and $token_objects['account']->roles has
  // the role id as keys. Need to get them to match up before checking for
  // intersection.
  $intersection = array_intersect(array_keys($token_objects['account']->roles), $types);
  return !empty($intersection);
}

/**
 * Implementation of hook_activity_token_list().
 */
function user_activity_token_list($type = 'all') {
  if ($type == 'user' || $type == 'all') {
    return array(
      'user' => array(
        'activity-user-link' => t('Link to the user. Used only when doing user based activity (i.e \'login\' \'update\' etc)'),
      ),
    );
  }
  return array();
}

/**
 * Implementation fo hook_activity_token_values().
 */
function user_activity_token_values($type, $object = NULL, $options = array()) {
  if ($type == 'user') {
    return array(
      'activity-user-link' => theme('activity_username', $object),
    );
  }
  return array();
}

/**
 * List all the Activity Actions that match the hook and op.
 *
 * @param string $hook
 *  The hook that is to be fired.
 * @param string $op
 *  The op to be used in the hook.
 * @param int $max_age
 *  The max age from right now to list activities.
 *
 * @return array
 *  An array of arrays with 'id', 'created' and 'actor' keys.
 */
function user_list_activity_actions($hook, $op, $max_age) {
  $actions = array();
  
  if (!empty($max_age)) {
    $min_time = time() - $max_age;
  }
  else {
    $min_time = 0;
  }

  if ($op == 'insert') {
    $sql = "SELECT uid as id, created as created, uid as actor FROM {users} WHERE uid <> 0 AND created > %d";
  }

  if (isset($sql)) {
    $result = db_query($sql, $min_time);
    while ($row = db_fetch_array($result)) {
      $actions[] = $row;
    }
  }

  return $actions;
}

/**
 * Load up the context array to pass to activity_record.
 *
 * @param string $hook
 *  The hook that is being fired.
 * @param string $op
 *  The op for that hook.
 * @param string $id
 *  The id for the action.
 *
 * @return array
 *   The context array for activity_record.
 * @see trigger_user
 */
function user_load_activity_context($hook, $op, $id) {
  $account = user_load($id);
  $context = array();
  if (!empty($account)) {
    $context = array(
      'hook' => $hook,
      'op' => $op,
      'form_values' => array(),
      'account' => $account,
    );
  }

  return $context;
}