<?php
// $Id: to_do.users.autocomplete.inc,v 1.1.2.4 2010/08/12 22:15:37 alexiswilke Exp $

/**
 * @file
 * Function used to auto-complete user assignments.
 */

function to_do_autocomplete_users($string = '_') {
  global $user;

  // If string is empty, don't bother making any db requests (for testing, '_' gets all users except anon)
  if (!$string) {
    echo drupal_to_js(array());
    exit();
  }

  // Grabs roles with relevant permissions.
  $roles = _to_do_roles();
  if (empty($roles)) {
    echo drupal_to_js(array());
    exit();
  }

  // Ignore editing user, as they have their own check box (assign to self)
  // If the authenticated user role (2) has the appropriate permissions, then so do all roles, no need to check roles.
  if ( in_array( 2 , array_keys($roles) ) ) {
    $sql = "SELECT u.uid, u.name FROM {users} u WHERE u.uid <> %d AND LOWER(u.name) LIKE LOWER('%s')";
    $place_holders = array( $user->uid, "%" . $string . "%");
  } 
  else {
    $sql = "SELECT u.uid, u.name FROM {users} u, {users_roles} r WHERE u.uid <> %d AND u.uid = r.uid AND r.rid IN ("
      . db_placeholders($roles, 'int') . ") AND LOWER(u.name) LIKE LOWER('%s')";
    $place_holders = array_merge(array($user->uid), array_flip($roles), array("%" . $string . "%"));
  }

  // Grab the first 10 users who can be assigned a To do list item
  $result = db_query_range($sql, $place_holders, 0, 10);

  $matches = array();
  while ($account = db_fetch_object($result)) {
    $matches[$account->name] = check_plain($account->name) . ' [uid:' . $account->uid . ']';
  }

  echo drupal_to_js($matches);
  exit();
}

// vim: ts=2 sw=2 et syntax=php
