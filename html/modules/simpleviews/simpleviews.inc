<?php
// $Id: simpleviews.inc,v 1.1.2.1 2008/09/26 19:28:46 eaton Exp $

/**
 * @file
 * SimpleViews utility library.
 *
 * Provides helper functions to generate standard Views based on a small number
 * of parameters.
 */

/**
 * Build a view object.
 *
 * This function converts a simple set of parameters into a full View definition.
 *
 * @param $simpleview
 *   An array of votes, each with the following structure:
 *   $simpleview['path']      (Required)
 *   $simpleview['title']     (Required)
 *   $simpleview['header']    (Optional)
 *   $simpleview['filter']    (Optional, defaults to 'all')
 *   $simpleview['style']     (Optional, defaults to 'teasers')
 *   $simpleview['sort']      (Optional, defaults to 'newest')
 *   $simpleview['argument']  (Optional)
 *   $simpleview['rss']       (Optional, defaults to TRUE)
 *   $simpleview['block']     (Optional, defaults to TRUE)
 *   $simpleview['module']    (Optional, defaults to 'simpleviews')
 *
 *   For individual properties inside $simplview, accepted values include:
 *   $simpleview['filter']    'all', or 'node:$nodetype'
 *   $simpleview['style']     'teasers', 'full', 'titles', or 'gallery'
 *   $simpleview['sort']      'newest', 'oldest', 'a-z', 'rating', or 'hits'
 *   $simpleview['argument']  'author', 'date', 'term:$vid', or 'field:$fieldname'

 * @return
 *   A newly constructed View object. To be properly handled by the Views module,
 *   the $view->name property must be set.
 */
function _simpleviews_build_view($simpleview = array()) {
  $simpleview += simpleviews_default_data();
  
  // Now we'll set up the basic starting structure.
  $view = new view;
  $view->name = empty($simpleview['name']) ? $simpleview['module'] . '_' . $simpleview['svid'] : $simpleview['name'];
  $view->description = "Generated automatically by the {$simpleview['module']} module.";
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;

  // Bootstrap the default display for the view. Here's where the action happens.
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('title', $simpleview['title']);
  $handler->override_option('row_plugin', 'node');
  $handler->override_option('row_options', array(
    'teaser' => ($simpleview['style'] == 'full') ? 0 : 1,
    'links' => 1,
    'comments' => 0,
  ));
  $handler->override_option('use_pager', '1');

  _simpleviews_add_filter($handler, $simpleview);
  _simpleviews_add_sort($handler, $simpleview);
  _simpleviews_add_argument($handler, $simpleview);
  
  // This next step might be tricky if we want to allow just-block or just-feed
  // views in the future, but for now we will always add a page.
  _simpleviews_add_page($view, $simpleview);
  
  _simpleviews_add_extras($view, $simpleview);

  drupal_alter('simpleview', $view, $simpleview);

  // We will NOT save the view. Instead we're going to return it so others can
  // expose it as a default, save it to the DB, or whatever they'd like to do.
  return $view;
}

function _simpleviews_add_filter(&$handler, $simpleview) {
  // We always want to add the status filter.
  $filters['status'] = array(
    'operator' => '=',
    'value' => 1,
    'group' => '0',
    'exposed' => FALSE,
    'expose' => array(
      'operator' => FALSE,
      'label' => '',
    ),
    'id' => 'status',
    'table' => 'node',
    'field' => 'status',
    'relationship' => 'none',
  );

  // Currently, the only supported filters are per-node-type.
  // 'all' is actually just a passthrough option that doesn't apply
  // filtering beyond the standard 'published' check.
  $filter = $simpleview['filter'];
  if (strstr($filter, 'node') !== FALSE && $type = end(explode(':', $filter))) {
    $filters['type'] = array(
      'operator' => 'in',
      'value' => array($type => $type),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array('operator' => FALSE, 'label' => ''),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    );
  }
  
  $handler->override_option('filters', $filters);
}

function _simpleviews_add_sort(&$handler, $simpleview) {
  switch ($simpleview['sort']) {
    case 'newest':
      $sorts['created'] =  array(
        'order' => 'DESC',
        'granularity' => 'second',
        'id' => 'created',
        'table' => 'node',
        'field' => 'created',
        'relationship' => 'none',
      );
      break;

    case 'oldest':
      $sorts['created'] =  array(
        'order' => 'ASC',
        'granularity' => 'second',
        'id' => 'created',
        'table' => 'node',
        'field' => 'created',
        'relationship' => 'none',
      );
      break;

    case 'a-z':
      $sorts['title'] = array(
        'order' => 'ASC',
        'id' => 'title',
        'table' => 'node',
        'field' => 'title',
        'relationship' => 'none',
      );
      break;

    case 'top-rated':
      if (module_exists('fivestar')) {
        // Use percentages
        $handler->override_option('relationships', array(
          'votingapi_cache' => array(
            'label' => 'Average',
            'required' => 0,
            'votingapi' => array(
              'value_type' => 'percent',
              'tag' => 'vote',
              'function' => 'average',
            ),
            'id' => 'votingapi_cache',
            'table' => 'node',
            'field' => 'votingapi_cache',
            'relationship' => 'none',
          ),
        ));
      }
      else if (module_exists('plus1') || module_exists('drigg')) {
        // Use points
        $handler->override_option('relationships', array(
          'votingapi_cache' => array(
            'label' => 'Average',
            'required' => 0,
            'votingapi' => array(
              'value_type' => 'points',
              'tag' => 'vote',
              'function' => 'sum',
            ),
            'id' => 'votingapi_cache',
            'table' => 'node',
            'field' => 'votingapi_cache',
            'relationship' => 'none',
          ),
        ));
      }
      
      $sorts['value'] = array(
        'order' => 'DESC',
        'id' => 'value',
        'table' => 'votingapi_cache',
        'field' => 'value',
        'relationship' => 'votingapi_cache',
      );
      break;

    case 'popular':
      $sorts['totalcount'] = array(
        'order' => 'DESC',
        'id' => 'totalcount',
        'table' => 'node_counter',
        'field' => 'totalcount',
        'relationship' => 'none',
      );
      break;
  }

  if (!empty($sorts)) {
    $handler->override_option('sorts', $sorts);
  }
}

function _simpleviews_add_argument(&$handler, $simpleview) {
  $argument = $simpleview['argument'];

  if ($argument == 'author') {
    $handler->override_option('arguments', array(
      'uid' => array(
        'default_action' => 'ignore',
        'style_plugin' => 'default_summary',
        'style_options' => array(),
        'wildcard' => 'all',
        'wildcard_substitution' => 'All',
        'title' => '',
        'default_argument_type' => 'fixed',
        'default_argument' => '',
        'validate_type' => 'none',
        'validate_fail' => 'not found',
        'break_phrase' => 0,
        'not' => 0,
        'id' => 'uid',
        'table' => 'users',
        'field' => 'uid',
        'relationship' => 'none',
        'default_options_div_prefix' => '',
      ),
    ));
  }
  else if ($argument = 'date') {
    $handler->override_option('arguments', array(
      'created_year_month' => array(
        'id' => 'created_year_month',
        'table' => 'node',
        'field' => 'created_year_month',
        'validate_type' => 'none',
        'validate_fail' => 'ignore',
        'default_argument_type' => 'fixed',
        'relationship' => 'none',
        'default_action' => 'ignore',
        'validate_argument_php' => '',
      ),
    ));
  }
  else if (strstr($argument, 'term:') !== FALSE && $vid = end(explode(':', $argument))) {
    $handler->override_option('arguments', array(
      'tid' => array(
        'default_action' => 'ignore',
        'style_plugin' => 'default_summary',
        'style_options' => array(),
        'wildcard' => 'all',
        'wildcard_substitution' => 'All',
        'title' => '',
        'default_argument_type' => 'fixed',
        'default_argument' => '',
        'validate_type' => 'taxonomy_term',
        'validate_fail' => 'ignore',
        'break_phrase' => 0,
        'add_table' => 0,
        'require_value' => 0,
        'reduce_duplicates' => 1,
        'set_breadcrumb' => 1,
        'id' => 'tid',
        'table' => 'term_node',
        'field' => 'tid',
        'relationship' => 'none',
        'validate_argument_vocabulary' => array((string)$vid => $vid),
        'validate_argument_type' => 'convert',
      ),
    ));
  }
  else if (strstr($argument, 'field:')) {
    // No handling for this at the moment. In the future, we want
    // to have some custom handling for CCK field based arguments.
    // For now? No love.
  }
}


function _simpleviews_add_page(&$view, $simpleview) {
  // Here we add the default page view. Good stuff.
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->override_option('path', $simpleview['path']);
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => $simpleview['title'],
    'weight' => '0',
    'default' => 'navigation' // Relies on drupal.org/node/284893 and drupal.org/node/285309
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  
  if ($simpleview['style'] == 'titles') {
    _simpleviews_set_display_to_title_list($handler, $simpleview);
    $handler->override_option('items_per_page', 20);
  }
}


function _simpleviews_set_display_to_title_list(&$handler, $simpleview) {
  // This lets us override the defaults for blocks and pages set to
  // list view.
  $handler->override_option('fields', array(
    'title' => array(
      'label' => '',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
      'override' => array('button' => 'Use default'),
    ),
  ));
  $handler->override_option('row_plugin', 'fields');
  $handler->override_option('style_plugin', 'list');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'type' => 'ul',
  ));
}


function _simpleviews_add_extras(&$view, $simpleview) {
  // Here we generate a block with a [more] link.
  if (!empty($simpleview['block'])) {
    $handler = $view->new_display('block', 'Block', 'block');
    $handler->override_option('items_per_page', 5);
    $handler->override_option('use_pager', '0');
    $handler->override_option('use_more', 1);
    $handler->override_option('block_description', $simpleview['title']);
    $handler->override_option('block_caching', -1);
    
    _simpleviews_set_display_to_title_list($handler, $simpleview);
  }
  
  // Here we generate an RSS feed at [path]/rss.xml
  if (!empty($simpleview['rss'])) {
    $handler = $view->new_display('feed', 'Feed', 'feed');
    $handler->override_option('row_plugin', 'node_rss');
    $handler->override_option('row_options', array(
      'item_length' => 'default',
    ));
    $handler->override_option('style_plugin', 'rss');
    $handler->override_option('style_options', array(
      'mission_description' => FALSE,
      'description' => '',
    ));
    
    $path = $simpleview['path'];
    $path .= empty($simpleview['argument']) ? '/rss.xml' : '/%/rss.xml';
    $handler->override_option('path', $path);
    $handler->override_option('displays', array('page' => 'page'));
  }
}
