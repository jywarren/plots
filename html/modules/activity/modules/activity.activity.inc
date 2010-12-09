<?php
// $Id: activity.activity.inc,v 1.1.2.1 2010/01/24 04:38:23 scottreynolds Exp $
/**
 * @file:
 * Provides Activity2 hooks for itself.
 */

/**
 * Implementation of hook_activity_grants().
 */
function activity_activity_grants($activity) {
  $grants = array();

  // Just in case uid 0 can do Activities.
  if (!empty($activity->uid)) {
    $grants = array(
      'activity_actor' => array(
        $activity->uid
      ),
    );
  }

  return $grants;
}

/**
 * Implementation of hook_activity_access_grants().
 */
function activity_activity_access_grants($account) {
  return array(
    'activity_actor' => array(
      $account->uid
    ),
  );
}