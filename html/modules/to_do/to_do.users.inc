<?php
// $Id: to_do.users.inc,v 1.1.2.4 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * User related functions.
 * Auto-complete, list of to do items, etc.
 */

/**
 * Actual implementation of hook_user().
 */
function _to_do_user($op, $edit, $account) {
  switch ($op) {
  case 'delete':
    db_query('DELETE FROM {to_do_assigned_users} WHERE uid = %d', $account->uid);
    // XXX -- check for nodes that are now unassigned
    break;

  }
}

/**
 * Callback function for:
 *   to_do
 */
function to_do_list_redirect() {
  global $user;
  drupal_goto('user/' . $user->uid . '/to_do');
}

/**
 * callback function for following paths:
 *   user/%/to_do
 */
function to_do_list($uid) {
  drupal_set_title(t('My to do assignments'));

  $listing_query = "SELECT n.nid, n.vid, n.title,"
      . " td.item_status, td.priority, td.start_date, td.deadline, td.date_finished"
    . " FROM {to_do} td, {to_do_assigned_users} tdau, {node} n"
    . " WHERE n.type = 'to_do' AND n.vid = td.vid AND n.vid = tdau.vid AND tdau.uid = %d";

  $count_query = "SELECT COUNT(n.nid) FROM {to_do} td, {to_do_assigned_users} tdau, {node} n"
    . " WHERE n.type = 'to_do' AND n.vid = td.vid AND n.vid = tdau.vid AND tdau.uid = %d";

  return to_do_listing($uid, $listing_query, $count_query, TRUE);
}

/**
 * Callback function for:
 *    user/%/to_to/created
 */
function to_do_list_created($uid) {
  drupal_set_title(t('To do assignments I created'));

  $listing_query = "SELECT n.nid, n.vid, n.title,"
      . " td.item_status, td.priority, td.start_date, td.deadline, td.date_finished"
    . " FROM {to_do} td, {node} n"
    . " WHERE n.type = 'to_do' AND n.uid = %d AND n.vid = td.vid";

  $count_query = "SELECT COUNT(n.nid) FROM {node} n WHERE type = 'to_do' AND uid = %d";

  return to_do_listing($uid, $listing_query, $count_query);
}

/**
 * Callback function for:
 *    user/%/to_to/all
 */
function to_do_list_all($uid) {
  $listing_query = "SELECT n.nid, n.vid, n.title,"
      . " td.item_status, td.priority, td.start_date, td.deadline, td.date_finished"
    . " FROM {to_do} td, {node} n"
    . " WHERE n.type = 'to_do' AND n.vid = td.vid";

  $count_query = "SELECT COUNT(n.nid) FROM {to_do} td, {node} n"
    . " WHERE n.type = 'to_do' AND n.vid = td.vid";

  return to_do_listing($uid, $listing_query, $count_query);
}

/**
 * Generate a list.
 */
function to_do_listing($uid, $listing_query, $count_query, $this_user_only = FALSE) {
  $path = drupal_get_path('module', 'to_do');
  drupal_add_css($path . '/css/to_do_list.css');

  // initialize some variables
  $list_length = variable_get('to_do_number_of_listings', 10);
  $status_list = _to_do_status_list();
  $priority_list = _to_do_priority_list();

  $header = array();
  $header[] = array('data' => t('Title'),         'field' => 'title');
  if (!$this_user_only) {
    $header[] = array('data' => t('Assigned To'));
  }
  $header[] = array('data' => t('Status'),        'field' => 'item_status');
  $header[] = array('data' => t('Priority'),      'field' => 'priority', 'sort' => 'desc');
  $header[] = array('data' => t('Start Date'),    'field' => 'start_date');
  $header[] = array('data' => t('Deadline'),      'field' => 'deadline');
  $header[] = array('data' => t('Date Finished'), 'field' => 'date_finished');

  $rows = array();

  // tweak then send the query
  $listing_query .= tablesort_sql($header);
  $listing_query = db_rewrite_sql($listing_query, 'n', 'nid');
  $count_query = db_rewrite_sql($count_query, 'n', 'nid');
  $to_do_listing = pager_query($listing_query, $list_length, 0, $count_query, $uid);

  while ($list_item = db_fetch_array($to_do_listing)) {
    $row = array();

    // name of the assignment
    $row[] = array(
      'data' => l($list_item['title'], 'node/' . $list_item['nid']),
      'scope' => 'row',
    );

    // assigned users
    if (!$this_user_only) {
      $names = array();
      $uids = array();
      $users_query = "SELECT DISTINCT(uid), name FROM {users} WHERE uid IN"
        . " (SELECT uid FROM {to_do_assigned_users} WHERE vid = %d)"
        . " GROUP BY uid, name";
      $users_listing = db_query($users_query, $list_item['vid']);
      while ($account = db_fetch_array($users_listing)) {
        $names[] = $account['name'];
        $uids[] = $account['uid'];
      }
      $cell = '<ul>';
      for ($i = 0; $i < count($names); $i++) {
        $name = ($uids[$i] == $uid) ? '<em>' . $names[$i] . '</em>' : $names[$i];
        $cell .= '<li>' . l($name, 'user/' . $uids[$i], array('html' => TRUE)) . '</li>';
      }
      $cell .= '</ul>';
      $row[] = $cell;
    }

    // assign the status & dates
    $row[] = $status_list[$list_item['item_status']];
    $row[] = $priority_list[$list_item['priority']];
    $row[] = to_do_format_date($list_item['start_date']);
    $row[] = to_do_format_date($list_item['deadline']);
    $row[] = to_do_format_date($list_item['date_finished']);

    $rows[] = array(
      'data' => $row,
      'class' => to_do_item_status_class($list_item),
    );
  }

  if (count($rows) == 0) {
    $row[] = array(
      'data' => t('Excellent! There are no To do list items in this list.'),
      'colspan' => $this_user_only ? 6 : 7,
      'style' => 'text-align: center;',
    );
    $rows[] = $row;
  }

  // generate the table, happen the pager, and return the result
  $output = theme('table', $header, $rows);
  $output .= theme('pager', array('<<', '<', '', '>', '>>'), $list_length);

  return '<div id="to_do_list">' . $output . '</div>';
}

// vim: ts=2 sw=2 et syntax=php
