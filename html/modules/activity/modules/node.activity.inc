<?php
// $Id: node.activity.inc,v 1.1.2.22 2010/08/03 15:48:10 scottreynolds Exp $

/**
 * @file
 * Activity definition file for node.module
 */

/**
 * Implementation of hook_activity_type_check().
 */
function node_activity_type_check($token_objects, $types) {
  return (in_array($token_objects['node']->type, $types));
}

/**
 * Implementation of hook_activity_token_list().
 */
function node_activity_token_list($type = 'all') {
  if ($type == 'node' || $type == 'all') {
    return array(
      'node' => array(
        'activity-node-link' => t('Link to the node'),
        'activity-node-creator-link' => t('Link to the node creator'),
      ),
    );
  }
  return array();
}

/**
 * Implementation of hook_activity_token_values().
 */
function node_activity_token_values($type, $object = NULL, $options = array()) {
  if ($type == 'node') {
    return array(
      'activity-node-link' => l($object->title, "node/{$object->nid}"),
      'activity-node-creator-link' => theme('activity_username', $object),
    );
  }
  return array();
}

/**
 * Implementation of hook_activity_grants().
 */
function node_activity_grants($activity) {
  $grants = array();
  if (!empty($activity->nid)) {
    $grants = array(
      'node_author' => array(
        $activity->nid
      ),
    );
  }
  return $grants;
}

/**
 * Implementation of hook_activity_access_grants().
 */
function node_activity_access_grants($account) {
  $grants = array();

  // Select the nodes you have created.
  $created_result = db_query("SELECT nid FROM {node} WHERE uid = %d", $account->uid);
  while ($created = db_fetch_object($created_result)) {
    $grants['node_author'][] = $created->nid;
  }

  return $grants;
}

/**
 * List all the Activity Actions that match the hook and op.
 *
 * @param string $hook
 *  The hook that is to be fired.
 * @param string $op
 *  The op to be used in the hook.
 * @param int $max_age
 *  The max age from now.
 *
 * @return array
 *  An array of arrays with 'id', 'created' and 'actor' keys.
 */
function node_list_activity_actions($hook, $op, $max_age) {
  $actions = array();

  if (!empty($max_age)) {
    $min_time = time() - $max_age;
  }
  else {
    $min_time = 0;
  }


  if ($op == 'insert' ) {
    $sql = "SELECT nid as id, created as created, uid as actor FROM {node} WHERE created > %d";
  }
  elseif ($op == 'view') {
    $sql = "SELECT nid as id, timestamp as created, uid as actor FROM {history} WHERE timestamp > %d";
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
 * @see trigger_nodeapi
 */
function node_load_activity_context($hook, $op, $id) {
  $node = node_load($id);
  $context = array();
  if (!empty($node)) {
    $context = array(
      'hook' => $hook,
      'op' => $op,
      'node' => $node,
    );
  }

  return $context;
}