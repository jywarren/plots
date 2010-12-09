<?php
// $Id: activity.views_default.inc,v 1.1.2.15 2010/07/18 21:17:20 scottreynolds Exp $

/**
 * Implementation of hook_views_default_views().
 *
 * Construct a basic default activity view that can be used as a basis for other
 * views.
 */
function activity_views_default_views() {
  $view = new view;
  $view->name = 'all_activity';
  $view->description = 'All activity on a site.';
  $view->tag = 'activity';
  $view->view_php = '';
  $view->base_table = 'activity';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Node',
        'required' => 0,
        'id' => 'nid',
        'table' => 'activity',
        'field' => 'nid',
        'relationship' => 'none',
      ),
      'uid' => array(
        'label' => 'User',
        'required' => 1,
        'id' => 'uid',
        'table' => 'activity',
        'field' => 'uid',
        'relationship' => 'none',
      ),
    ));
  $handler->override_option('fields', array(
      'message' => array(
        'label' => '',
        'alter' => array(
          'alter_text' => 0,
          'text' => '',
          'make_link' => 0,
          'path' => '',
          'alt' => '',
          'prefix' => '',
          'suffix' => '',
          'help' => '',
          'trim' => 0,
          'max_length' => '',
          'word_boundary' => 1,
          'ellipsis' => 1,
          'html' => 0,
        ),
        'exclude' => 0,
        'id' => 'message',
        'table' => 'activity_messages',
        'field' => 'message',
        'relationship' => 'none',
      ),
    ));
  $handler->override_option('sorts', array(
      'aid' => array(
        'order' => 'DESC',
        'id' => 'aid',
        'table' => 'activity',
        'field' => 'aid',
        'relationship' => 'none',
      ),
    ));
  $handler->override_option('filters', array(
      'status' => array(
        'operator' => '=',
        'value' => '1',
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'id' => 'status',
        'table' => 'activity',
        'field' => 'status',
        'relationship' => 'none',
      ),
    ));
  $handler->override_option('access', array(
      'type' => 'none',
    ));
  $handler->override_option('cache', array(
      'type' => 'none',
    ));
  $handler->override_option('use_ajax', TRUE);
  $handler->override_option('items_per_page', 25);
  $handler->override_option('use_pager', '1');
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'activity/all');
  $handler->override_option('menu', array(
      'type' => 'none',
      'title' => '',
      'description' => '',
      'weight' => 0,
      'name' => 'navigation',
    ));
  $handler->override_option('tab_options', array(
      'type' => 'none',
      'title' => '',
      'description' => '',
      'weight' => 0,
      'name' => 'navigation',
    ));
  $views[$view->name] = $view;
  
  // Activity Maintenance
  $view = new view;
  $view->name = 'activity_batch_controls';
  $view->description = 'Control panel for activity. View delete sort. Admins only.';
  $view->tag = 'activity';
  $view->view_php = '';
  $view->base_table = 'activity';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'uid' => array(
      'label' => 'User',
      'required' => 1,
      'id' => 'uid',
      'table' => 'activity',
      'field' => 'uid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_user' => 1,
      'overwrite_anonymous' => 0,
      'anonymous_text' => '',
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'uid',
    ),
    'message' => array(
      'label' => 'Message',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'exclude' => 0,
      'id' => 'message',
      'table' => 'activity_messages',
      'field' => 'message',
      'relationship' => 'none',
    ),
    'type' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'exclude' => 0,
      'id' => 'type',
      'table' => 'activity',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'operation' => array(
      'group_type' => 'group',
      'label' => 'Operation',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'operation',
      'table' => 'activity',
      'field' => 'operation',
      'relationship' => 'none',
    ),
    'created' => array(
      'label' => 'Activity Creation Date',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'created',
      'table' => 'activity',
      'field' => 'created',
      'relationship' => 'none',
    ),
    'delete_activity' => array(
      'label' => 'Delete link',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'text' => 'delete',
      'exclude' => 0,
      'id' => 'delete_activity',
      'table' => 'activity',
      'field' => 'delete_activity',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'uid' => array(
      'order' => 'ASC',
      'id' => 'uid',
      'table' => 'activity',
      'field' => 'uid',
      'relationship' => 'none',
    ),
    'created' => array(
      'order' => 'ASC',
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'activity',
      'field' => 'created',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'uid' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_op',
        'identifier' => 'uid',
        'label' => 'Activity: User',
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid',
      'table' => 'activity',
      'field' => 'uid',
      'relationship' => 'none',
    ),
    'type' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_op',
        'identifier' => 'type',
        'label' => 'Activity: Type',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 1,
      'id' => 'type',
      'table' => 'activity',
      'field' => 'type',
      'relationship' => 'none',
    ),
    'operation' => array(
      'group_type' => 'group',
      'operator' => 'in',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'operation_op',
        'identifier' => 'operation',
        'label' => 'Activity: Operation',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'operation',
      'table' => 'activity',
      'field' => 'operation',
      'relationship' => 'none',
    ),
    'nid' => array(
      'operator' => '=',
      'value' => array(
        'value' => '',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'nid_op',
        'identifier' => 'nid',
        'label' => 'Activity on Node: Nid',
        'optional' => 1,
        'remember' => 0,
      ),
      'id' => 'nid',
      'table' => 'node',
      'field' => 'nid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'administer activity',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('header', 'You can filter these results by providing criteria in the boxes below.
              <ul><li><b>Activity: User</b> - filter by the user who created the activity.</li>
              <li><b>Activity: Operation</b> - filter by the Operation field.</li>
              <li><b>Activity: Type</b> - filter by the Type field.</li>
              <li><b>Activity on Node: Nid</b> - filter by a particular node id.</li></ul>');
  $handler->override_option('header_format', '2');
  $handler->override_option('header_empty', 1);
  $handler->override_option('use_ajax', TRUE);
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 1,
    'order' => 'desc',
    'columns' => array(
      'name' => 'name',
      'message' => 'message',
      'type' => 'type',
      'op' => 'op',
      'created' => 'created',
      'delete_activity' => 'delete_activity',
    ),
    'info' => array(
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'message' => array(
        'separator' => '',
      ),
      'op' => array(
        'separator' => '',
      ),
      'type' => array(
        'separator' => '',
      ),
      'created' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'delete_activity' => array(
        'separator' => '',
      ),
    ),
    'default' => 'created',
  ));
  $handler->override_option('exposed_form', array(
    'type' => 'basic',
    'options' => array(),
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'admin/settings/activity/maintenance');
  $handler->override_option('menu', array(
    'type' => 'tab',
    'title' => 'Maintenance',
    'description' => 'Maintain your activity records',
    'weight' => '10',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;
  
  return $views;
}
