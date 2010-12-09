<?php
// $Id: activity_comments.views.inc,v 1.1.2.1 2010/01/13 05:25:29 scottreynolds Exp $

/**
 * Implementation of hook_views_data().
 */
function activity_comments_views_data() {
  $data['activity_comments']['table']['group'] = t('Activity comments');
  $data['activity_comments_field']['table']['join'] = array(
    'activity' => array(
      'left_field' => 'aid',
      'field' => 'aid',
    ),
  );

  $data['activity_comments_field']['comment_form'] = array(
    'title' =>  t('Comments'),
    'help' => t('Comments to activities.'),
    'group' => t('Activity Comments'),
    'field' => array(
      'handler' => 'activity_comments_handler_field_comments',
    ),
  );
  
  $data['activity_comments_stats']['table']['group'] = t('Activity comments');

  $data['activity_comments_stats']['table']['join'] = array(
    'activity' => array(
      'left_field' => 'aid',
      'field' => 'aid',
    ),
  );
  
  $data['activity_comments_stats']['changed'] = array(
    'title' => t('Changed time'),
    'help' => t('The time when last activity comment was added'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  
  $data['activity_comments_stats']['comment_count'] = array(
    'title' => t('Comment count'),
    'help' => t('The comment count on an activity'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function activity_comments_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'activity_comments') .'/views',
    ),
    'handlers' => array(
      'activity_comments_handler_field_comments' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
