<?php
// $Id: protected_node.settings.inc,v 1.1 2008/10/14 09:35:29 tolmi Exp $
/**
 * @file
 * Configuration file for the protected_node module.
 */

/**
 * Define the settings form
 *
 * @return $form The settings form
 */
function protected_node_admin_settings( ) {
  $form['protected_node_info'] = array(
    '#type' => 'textarea',
    '#title' => t( 'Password page info' ),
    '#default_value' => variable_get( 'protected_node_info', '' ),
    '#description' => t( 'You can use node type tokens from the token module if you have installed it previously.' )
  );

  return system_settings_form( $form );
}