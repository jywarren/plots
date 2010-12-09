<?php
// $Id: activity.views.inc,v 1.1.2.24 2010/07/18 21:17:20 scottreynolds Exp $
/**
 * @file: provides views data integration for activity module as the base table
 */

/**
 * Implementation of hook_views_data().
 */
function activity_views_data() {
  $data['activity']['table']['group'] = t('Activity');
  
  $data['activity']['table']['base'] = array(
    'field' => 'aid',
    'title' => t('Activity'),
    'help' => t('Activity are items that are recorded for a users activity within the site'),
  );
  
  // Database columns
  $data['activity']['aid'] = array(
    'title' => t('Activity Unique ID'),
    'help' => t('The unique id for each activity'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['activity']['uid'] = array(
    'title' => t('Activity Actor'),
    'help' => t('The ID of the person who recorded the activity'),
    'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
    ),
    'filter' => array(
      'title' => t('Name'),
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  // Views doesn't allow for a field of op. It is used elsewhere by Views
  $data['activity']['operation'] = array(
    'title' => t('Operation'),
    'help' => t('The operation being performed (update, insert, etc.)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'field' => 'op',
    ),
    'filter' => array(
      'handler' => 'activity_handler_filter_operations',
      'field' => 'op',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'field' => 'op',
    ),
  );
  $data['activity']['type'] = array(
    'title' => t('Type'),
    'help' => t('The type of object being acted upon (node, user, etc.)'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['activity']['created'] = array(
    'title' => t('Activity Creation Date'),
    'help' => t('The time that the activity was created'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  $data['activity']['status'] = array(
    'title' => t('Published'),
    'help' => t('Whether or not the activity is published.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['activity_access']['table']['group'] = t('Activity Access');
  $data['activity_access']['table']['join'] = array(
    'activity' => array(
      // We cannot guarantee that there is activity_access records.
      'type' => 'LEFT',
      'left_field' => 'aid',
      'field' => 'aid',
    ),
  );
  
  // Sets the context accound for all access filters going forward
  $data['activity_access']['context_user'] = array(
    'title' => t('Activity Context User'),
    'help' => t('This sets Activity User context so all the access filters can key off of it'),
    'argument' => array(
      'handler' => 'activity_handler_argument_activity_user',
    ),
  );
    
  foreach (activity_get_module_info() as $module => $info) {
    if (!empty($info->realms)) {
      $data['activity_access']["value_$module"] = array(
        'title' => t("@module module's access control", array("@module" => drupal_ucfirst($module))),
        'help' => t('Allow @module to filter activity messages', array('@module' => $module)),
        'filter' => array(
          'handler' => 'activity_views_handler_filter_access',
          'module' => $module,
          'field' => 'value',
        ),
        'argument' => array(
          'handler' => 'activity_views_handler_argument_access',
          'module' => $module,
          'field' => 'value',
        ),
      );
    }
  }
  
  // describe the targets table so Views knows how to join it in
  // this table will be used to join the activity table to the message table
  $data['activity_targets']['table'] = array(
    'group' => t('Activity'),
    'join' => array(
      'activity' => array(
        'type' => 'INNER',
        'left_field' => 'aid',
        'field' => 'aid',
        'extra' => array(
          array(
            'value' => 0, // grab the non-personal message
            'numeric' => TRUE,
            'field' => 'uid',
          ),
          array(
            'value' => '***ACTIVITY_LANGUAGE***',
            'field' => 'language',
          ),
        ),
      ),
    ),
  );
  
  // describe the personal targets table so Views knows how to join it in
  // this table will be used to join the activity table to the personal message table
  $data['activity_personal_targets']['table'] = array(
    'group' => t('Activity'),
    'join' => array(
      'activity' => array(
        'type' => 'LEFT',
        'left_field' => 'aid',
        'field' => 'aid',
        'extra' => array(
          array(
            'value' => '***CURRENT_USER***', // grab the personal message
            'numeric' => TRUE,
            'field' => 'uid',
          ),
          array(
            'value' => '***ACTIVITY_LANGUAGE***',
            'field' => 'language',
          ),
        ),
        'table' => 'activity_targets',
      ),
    ),
  );
  
  // add in the activity message table
  $data['activity_messages']['table']['group'] = t('Activity');
  $data['activity_messages']['table']['join'] = array(
    'activity' => array(
      'left_table' => 'activity_targets',
      'left_field' => 'amid',
      'field' => 'amid',
      'type' => 'INNER',
    ),
  );
  
  // create a faux table, that is really activity messages
  $data['activity_personal_messages']['table']['join'] = array(
    'activity' => array(
      'left_table' => 'activity_personal_targets',
      'left_field' => 'amid',
      'field' => 'amid',
      'type' => 'LEFT',
      'table' => 'activity_messages',
    ),
  );
  
  $data['activity_messages']['message'] = array(
    'title' => t('Message'),
    'help' => t('The message may or may not be personalized for the user. Just depends on if the user was involved in the Action'),
    'field' => array(
      'handler' => 'activity_views_handler_field_message',
    ),
  );
  
  // Links to operate upon the activity records
  $data['activity']['delete_activity'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the activity.'),
      'handler' => 'activity_handler_field_activity_link_delete',
    ),
  );
  
  // Relationships to other base tables
  $data['activity']['nid']['relationship'] = array(
    'group' => t('Activity'),
    'title' => t('Node'),
    'help' => t('Join the node table to the Activity table'),
    'handler' => 'views_handler_relationship',
    'base' => 'node',
    'base field' => 'nid',
    'label' => t('Node'),
  );
  $data['activity']['uid']['relationship'] = array(
    'group' => t('Activity'),
    'title' => t('User'),
    'help' => t('Join the users table to the Activity table'),
    'handler' => 'views_handler_relationship',
    'base' => 'users',
    'base field' => 'uid',
    'label' => t('User'),
  );
  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function activity_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'activity') . '/views',
    ),
    'handlers' => array(
      'activity_views_handler_field_message' => array(
        'parent' => 'views_handler_field',
      ),
      'activity_views_handler_filter_access' => array(
        'parent' => 'views_handler_filter',
      ),
      'activity_views_handler_argument_access' => array(
        'parent' => 'views_handler_argument',
      ),
      'activity_views_handler_filter_uid_access' => array(
        'parent' => 'views_handler_filter',
      ),
      'activity_handler_filter_operations' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
      'activity_handler_field_activity_link' => array(
        'parent' => 'views_handler_field',
      ),
      'activity_handler_field_activity_link_delete' => array(
        'parent' => 'activity_handler_field_activity_link',
      ),
      'activity_handler_argument_activity_user' => array(
        'parent' => 'views_handler_argument_user_uid',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_query_alter().
 */
function activity_views_query_alter(&$view, &$query) {
  if ($query->get_table_info('activity_access')) {
    $query->set_distinct();
  }
}

/**
 * return the group id for the where clause
 */
function activity_views_group($new_group = NULL) {
  static $group_id = NULL;
  if (isset($new_group)) {
    $group_id = $new_group;
  }
  
  return $group_id;
}

/**
 * Context getter setter for Activity Access
 */
function activity_views_access_user($account = NULL) {
  static $access_account = NULL;
  if (!empty($account)) {
    $access_account = $account;
  }
  return (empty($access_account) ? $GLOBALS['user'] : $access_account);
}

/**
 * Implemenation of hook_views_query_substitutions().
 */
function activity_views_query_substitutions($view) {
  // @see http://drupal.org/node/328325
  global $user, $language;
  // Find the user's preferred language, if any.
  $preferred_language = $user->uid ? user_preferred_language($user) : $language;
  return array(
    '***ACTIVITY_LANGUAGE***' => $preferred_language->language,
  );
}
