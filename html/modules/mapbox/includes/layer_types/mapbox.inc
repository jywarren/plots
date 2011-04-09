<?php
// $Id: mapbox.inc,v 1.13 2010/04/29 22:05:20 tmcw Exp $

/**
 * @file MapBox layer type definition for OpenLayers
 */
class openlayers_layer_type_mapbox extends openlayers_layer_type {
  /**
   * Provide initial values for options.
   */
  function options_init() {
    return array(
      'serverResolutions' => openlayers_get_resolutions('900913'),
      'maxExtent' => openlayers_get_extent('900913'),
      'projection' => array('900913'),
      'osm' => FALSE,
      'baselayer' => TRUE,
      'layer_handler' => 'MapBox',
      'type' => 'png',
    );
  }

  /**
   * Options form which generates layers
   */
  function options_form() {
    if (!$this->data) {
      $this->data = $this->options_init();
    }
    return array(
      'layername' => array(
        '#type' => 'textfield',
        '#title' => t('MapBox Layer Name'),
        '#description' => t('The layer name specified on MapBox.com'),
        '#default_value' => $this->data['layername']
      ),
      'layer_type' => array(
        '#type' => 'hidden',
        '#value' => 'openlayers_layer_type_mapbox'
      ),
      'osm' => array(
        '#type' => 'checkbox',
        '#default_value' => $this->data['osm'],
        '#title' => t('OSM Attribution'),
        '#description' => t('Required if OpenStreetMap data is used')
      ),
      'baselayer' => array(
        '#type' => 'checkbox',
        '#default_value' => $this->data['baselayer'],
        '#title' => t('BaseLayer'),
        '#description' => t('Uncheck to make this map an overlay')
      ),
      'type' => array(
        '#title' => t('File extension'),
        '#type' => 'select',
        '#default_value' => $this->data['type'],
        '#options' => drupal_map_assoc(array('png', 'jpg')),
      ),
      /*
      'mini_attribution' => array(
        '#type' => 'checkbox',
        '#default_value' => $this->data['mini_attribution'] ? 
          $this->data['mini_attribution'] : FALSE,
        '#title' => t('Mini Attribution'),
        '#description' => t('Use small logo and text, for mini maps.')
      ),
       */
      'resolutions' => array(
        '#type' => 'select',
        '#multiple' => TRUE,
        '#options' => array_combine(
          array_map('strval', openlayers_get_resolutions('900913')), 
          range(0, 18)
        ),
        '#title' => t('Zoom Level Range'),
        '#default_value' => $this->data['resolutions'],
      ),
      'projection' =>
        array(
         '0' => array(
          '#type' => 'hidden',
          '#value' => '900913',
        ),
      ),
      '#default_value' => ($this->data['resolutions']) ? 
        $this->data['resolutions'] : 
        array_map('strval', openlayers_get_resolutions('900913'))
    );
  }

  /**
   * Render.
   */
  function render(&$map) {
    drupal_add_js(drupal_get_path('module', 'mapbox') .'/includes/layer_types/mapbox.js');
    drupal_add_css(drupal_get_path('module', 'mapbox') .'/js/mapbox.css');
    return $this->options;
  }
}

