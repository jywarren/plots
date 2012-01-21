<?php
// $Id: node_images.views.inc,v 1.3 2007/02/03 01:28:22 stefano73 Exp $

/**
 * @file
 * Views API hook implementations for the node_images module.
 */

function _node_images_views_tables() {
  $tables['node_images'] = array(
    'name' => 'node_images',
    'provider' => 'internal',
    'join' => array(
      'left' => array(
        'table' => 'node',
        'field' => 'nid'
      ),
      'right' => array(
        'field' => 'nid'
      ),
    ),
    'fields' => array(
      'node_images_display' => array(
        'name' => t('Node Images: Display node images'),
        'notafield' => TRUE,
        'query_handler' => '_node_images_query_handler_display',
        'handler' => array(
          '_node_images_views_handler_image_thumb' => t('Thumbnails'),
          '_node_images_views_handler_image_fullsize' => t('Full size'),
        ),
        'sortable' => false,
        'help' => t('Display all node images in one field.'),
      ),
    ),
    'filters' => array(
      'nid' => array(
        'name' => t('Node images: Has node images'),
        'operator' => array('=' => t('Exists')),
        'list' => 'views_handler_operator_yesno',
        'list-type' => 'select',
        'handler' => '_node_images_handler_images_exist',
        'help' => t('Filter whether the node has node images.'),
      ),
    ),
  );
  return $tables;
}

function _node_images_query_handler_display($field, $fieldinfo, &$query) {
  $query->add_field('nid', 'node');
}

/**
 * Views handler for displaying images.
 */
function _node_images_views_handler_image_thumb($fieldinfo, $fielddata, $value, $data, $thumb = TRUE) {
  $node = node_load(array('nid' => $data->nid));
  $format = ($thumb ? 'thumbs' :'fullsize');
  return theme('node_images_view', $node, TRUE, FALSE, $format);
}

/**
 * Views handler for displaying images in links to a popup window
 */
function _node_images_views_handler_image_fullsize($fieldinfo, $fielddata, $value, $data) {
  return _node_images_views_handler_image_thumb($fieldinfo, $fielddata, $value, $data, FALSE);
}

/**
 * Views handler for filtering
 */
function _node_images_handler_images_exist($op, $filter, $filterdata, &$query) {
  switch ($op) {
    case 'handler':
      $query->ensure_table('node_images');
      if ($filter['value']) {
        $query->set_distinct();
        $table_data = _views_get_tables();
	$joins = array('type' => 'inner');
	$joins = array_merge($joins, $table_data['node_images']['join']);
        $query->joins['node_images'][1] = $joins;
      }
      else {
        $query->add_where('ISNULL(node_images.id)');
      }
  }
}