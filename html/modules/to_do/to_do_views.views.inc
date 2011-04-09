<?php
// $Id: to_do_views.views.inc,v 1.1.2.7 2010/10/01 21:14:42 alexiswilke Exp $

/**
 * @file
 * To do views declarations.
 */

/**
 * Implementation of hook_views_handlers().
 */
function to_do_views_views_handlers() {
  return array(
    'handlers' => array(
      'to_do_views_handler_field' => array(
        'parent' => 'views_handler_field',
      ),
      'to_do_views_handler_field_priority' => array(
        'parent' => 'to_do_views_handler_field',
      ),
      'to_do_views_handler_field_item_status' => array(
        'parent' => 'to_do_views_handler_field',
      ),
      'to_do_views_handler_field_buttons' => array(
        'parent' => 'to_do_views_handler_field',
      ),
      // WARNING: for the title the parent is the node
      //          (i.e. an extension)
      'to_do_views_handler_field_title' => array(
        'parent' => 'views_handler_field_node',
      ),
    ),
  );
}

function _to_do_views_extended_dates(&$data, $field, $label) {
  $entries = array(
    'fulldate' =>   array('help' => t('CCYYMMDD'),     'label' => t('full date')),
    'year_month' => array('help' => t('CCYYMM'),       'label' => t('year + month')),
    'year' =>       array('help' => t('CCYY'),         'label' => t('year')),
    'month' =>      array('help' => t('MM (01 - 12)'), 'label' => t('month')),
    'day' =>        array('help' => t('DD (01 - 31)'), 'label' => t('day')),
    'week' =>       array('help' => t('WW (01 - 53)'), 'label' => t('week')),
  );

  foreach ($entries as $f => $e) {
    $data['to_do'][$field . '_' . $f] = array(
      'title' => $label . ' ' . $e['label'],
      'help' => t('In the form of') . ' ' . $e['help'] . '.',
      'argument' => array(
        'field' => $field,
        'handler' => 'views_handler_argument_node_created_' . $f,
      ),
    );
  }
}

/**
 * Implementation of hook_views_data().
 */
function to_do_views_views_data() {
  $data = array();


  // Views: to do
  $data['to_do']['table']['group'] = t('To do list');

  $data['to_do']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'users' => array(
      'left_table' => 'to_do_assigned_users',
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  _to_do_views_extended_dates($data, 'start_date', 'Start date');
  $data['to_do']['start_date'] = array(
    'title' => t('Start date'),
    'help' => t('Date when the To do task was started (before it was started, date when it was created.) May be NULL.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  _to_do_views_extended_dates($data, 'deadline', 'Deadline');
  $data['to_do']['deadline'] = array(
    'title' => t('Deadline'),
    'help' => t('Date when the To do task must be finished by. May be NULL.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  _to_do_views_extended_dates($data, 'date_finished', 'Date finished');
  $data['to_do']['date_finished'] = array(
    'title' => t('Date finished'),
    'help' => t('Date when the To do task was finished on. May be NULL.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );

  $data['to_do']['priority'] = array(
    'title' => t('Priority'),
    'help' => t('The priority of the To do task'),
    'field' => array(
      'handler' => 'to_do_views_handler_field_priority',
      'click sortable' => TRUE,
    ),
    'argument' => array( // XXX -- have our own validation handler
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['to_do']['item_status'] = array(
    'title' => t('Status'),
    'help' => t('The status of the To do task'),
    'field' => array(
      'handler' => 'to_do_views_handler_field_item_status',
      'click sortable' => TRUE,
    ),
    'argument' => array( // XXX -- have our own validation handler
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['to_do']['auto_close'] = array(
    'title' => t('Auto-close'),
    'help' => t('Whether the To do task will be auto-closed on Deadline.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Auto-close'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['to_do']['buttons'] = array(
    'title' => t('Buttons'),
    'help' => t('The action buttons of the To do task'),
    'real field' => 'item_status',
    'field' => array(
      'handler' => 'to_do_views_handler_field_buttons',
    ),
  );



  // Views: to do assigned users
  $data['to_do_assigned_users']['table']['group'] = t('To do assigned users');

  $data['to_do_assigned_users']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'to_do' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  $data['to_do_assigned_users']['uid'] = array(
    'title' => t('Assigned users'),
    'help' => t('Users assigned a To do item.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'label' => t('Assigned user'),
    ),
  );

  $data['to_do_assigned_users']['vid'] = array(
    'title' => t('All user assignments'),
    'help' => t('To do item revisions assigned to a user (all revisions!)'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'vid',
      'label' => t('To do revision'),
    ),
  );

  $data['to_do_assigned_users']['nid'] = array(
    'title' => t('Current user assignments'),
    'help' => t('To do item nodes assigned to a user (only the current revision.)'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'vid',
      'label' => t('To do node'),
    ),
  );

  $data['to_do_assigned_users']['uid_field'] = array(
    'real field' => 'uid',
    'title' => t('Assigned user ID'),
    'help' => t('The identifier of users assigned a To do item.'),
    'field' => array(
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
    'arguement' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name',
    ),
    'filter' => array(
      'title' => t('Name'),
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['to_do_assigned_users']['vid_field'] = array(
    'real field' => 'vid',
    'title' => t('Vid'),
    'help' => t('The revision ID of the To do revision.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_vid',
      'parent' => 'views_handler_argument_numeric',
      'click sortable' => TRUE,
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['to_do_assigned_users']['nid_field'] = array(
    'real field' => 'nid',
    'title' => t('Node ID'),
    'help' => t('The node ID of the To do revision.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'parent' => 'views_handler_argument_numeric',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // title -- like node + <del> when marked finished/canceled
  //          and <ins> when not started
  $data['node']['to_do_title'] = array(
    'title' => t('Task brief description'),
    'help' => t('The short description (node title) of the to do task.'),
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'title',
      'group' => t('To do list'),
      'handler' => 'to_do_views_handler_field_title',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    // Information for accepting a title as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}

// vim: ts=2 sw=2 et syntax=php
