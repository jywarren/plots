<?php
// $Id: simpleviews.views_default.inc,v 1.3 2008/09/25 22:08:09 eaton Exp $

/**
 * Implementation of hook_views_default_views().
 *
 * Loads all simpleviews entries and content type settings, then generates a
 * view for each one. We do this so that the simple views can be overridden by
 * views_ui and customized by advanced users.
 */
function simpleviews_views_default_views() {
  $views = array();

  $results = db_query("SELECT * FROM {simpleviews}");
  $items = array();
  while ($item = db_fetch_array($results)) {
    $item['name'] = $item['module'] . '_' . $item['svid'];
    $view = simpleviews_build_view($item);
    $views[$view->name] = $view;
  }

  foreach (node_get_types('types', NULL, TRUE) as $type) {
    if (variable_get('simpleviews_type_' . $type->type, FALSE)) {
      $type_url_str = str_replace('_', '-', $type->type);
      $record = array(
        'name' => 'simpleviews_' . $type->type,
        'path' => $type_url_str,
        'title' => $type->name . ' content',
        'filter' => 'node:' . $type->type,
        'style' => 'teasers',
        'sort' => 'newest',
      );
      $view = simpleviews_build_view($record);
      $views[$view->name] = $view;
    }
  }
  return $views;
}
