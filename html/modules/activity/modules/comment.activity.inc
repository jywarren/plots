<?php
// $Id: comment.activity.inc,v 1.1.2.26 2010/09/26 21:15:55 scottreynolds Exp $

/**
 * @file
 * Activity definition file for comment.module
 */

/**
 * Implementation of hook_activity_grants().
 */
function comment_activity_grants($activity) {
  $grants = array();
  if (!empty($activity->nid)) {
    // tell activity to record 'comment' | nid
    $grants = array(
      'comment' => array($activity->nid),
    );
  }
  return $grants;
}

/**
 * Implementation of hook_activity_access_grants().
 */
function comment_activity_access_grants($account) {
  // tell activity what activities this user has based on the 'comment' realm
  $grants = array();
  
  // select the nodes you commented on
  $commented_result = db_query("SELECT nid FROM {comments} WHERE uid = %d", $account->uid);
  while ($commented = db_fetch_object($commented_result)) {
    $grants['comment'][] = $commented->nid;
  }
  return $grants;
}

/**
 * Implementation of hook_activity_type_check().
 */
function comment_activity_type_check($token_objects, $types) {
  return (in_array($token_objects['node']->type, $types));
}

/**
 * Implementation of hook_activity_token_list().
 */
function comment_activity_token_list($type = 'all') {
  if ($type == 'comment' || $type == 'all') {
    return array(
      'comment' => array(
        'activity-comment-author-link' => t('Link to the author of the comment'),
        'comment-link' => t('Link to the comment'),
      ),
    );
  }
  return array();
}

/**
 * Implementation of hook_activity_token_values().
 */
function comment_activity_token_values($type, $object = NULL, $options = array()) {
  if ($type == 'comment') {
    $node = node_load($object->nid);
    $page = comment_new_page_count($node->comment_count, 1, $node);
    $query = array();

    if ($page > 0) {
      $query['page'] = $page;
    }

    return array(
      'activity-comment-author-link' => theme('activity_username', $object),
      'comment-link' => l(check_plain($object->subject), 'node/' . $object->nid, array('query' => $query, 'fragment' => 'comment-' . $object->cid)),
    );
  }
  return array();
}

/**
 * Implementation of hook_activity_objects_alter().
 */
function comment_activity_objects_alter(&$token_objects, $activity_type) {
  if ($activity_type == 'comment') {
    $token_objects['node'] = node_load($token_objects['comment']->nid);

    // If the comment and the node are the same, provide an object for it.
    if (isset($token_objects['comment']) && $token_objects['node']->uid == $token_objects['comment']->uid) {
      $token_objects['node_comment_author'] = $token_objects['comment'];
    }
  }
}

/**
 * List all the Activity Actions that match the hook and op.
 *
 * @param string $hook
 *  The hook that is to be fired.
 * @param string $op
 *  The op to be used in the hook.
 * @param int $max_age
 *
 * @return array
 *  An array of arrays with 'id', 'created' and 'actor' keys.
 */
function comment_list_activity_actions($hook, $op, $max_age) {
  $actions = array();

  if (!empty($max_age)) {
    $min_time = time() - $max_age;
  }
  else {
    $min_time = 0;
  }

  if ($op == 'insert') {
    $sql = 'SELECT cid as id, timestamp as created, uid as actor FROM {comments} WHERE timestamp > %d';
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
 * @see trigger_comment
 */
function comment_load_activity_context($hook, $op, $id) {
  $comment = _comment_load($id);
  $context = array();
  if (!empty($comment)) {
    $context = array(
      'hook' => $hook,
      'op' => $op,
      'comment' => $comment,
    );
  }

  return $context;
}