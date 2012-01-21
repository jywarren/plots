<?php
// $Id: node_images.admin.inc,v 1.1.2.2 2008/08/23 08:53:35 stefano73 Exp $

/**
 * @file
 * Admin page callback file for the node_images module.
 */

/**
 * Menu callback for the node_images settings form.
 */
function node_images_admin_settings() {
  _node_images_check_settings();

  $form['node_images_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Node images path'),
    '#default_value' => variable_get('node_images_path', 'node_images'),
    '#maxlength' => 255,
    '#description' => t('Subdirectory in the directory %dir where the node images will be stored. This directory has to exist and be writable by Drupal. You can use the following variables: %uid, %username, %nid', array('%dir' => file_directory_path() .'/')),
    '#after_build' => array('_node_images_check_directory'),
  );
  $form['node_images_large_resolution'] = array(
    '#type' => 'textfield',
    '#title' => t('Resolution for large images'),
    '#default_value' => variable_get('node_images_large_resolution', '400x300'),
    '#size' => 19,
    '#maxlength' => 9,
    '#description' => t('The size for large images in the node gallery, expressed as WIDTHxHEIGHT (e.g. 400x300).'),
  );
  $form['node_images_thumb_resolution'] = array(
    '#type' => 'textfield',
    '#title' => t('Resolution for thumbnails'),
    '#default_value' => variable_get('node_images_thumb_resolution', '100x100'),
    '#size' => 15,
    '#maxlength' => 7,
    '#description' => t('The thumbnail size expressed as WIDTHxHEIGHT (e.g. 100x75).'),
  );
  $form['node_images_uploadsize_default'] = array(
    '#type' => 'textfield',
    '#title' => t('Default maximum file size per upload'),
    '#default_value' => variable_get('node_images_uploadsize_default', 1),
    '#size' => 5,
    '#maxlength' => 5,
    '#description' => t('The default maximum file size a user can upload. If a maximum resolution is set, the size will be checked after the file has been resized.'),
    '#field_suffix' => t('MB'),
  );
  $form['node_images_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Default permitted image extensions'),
    '#default_value' => variable_get('node_images_extensions', 'jpg jpeg gif png'),
    '#maxlength' => 255,
    '#description' => t('Default image extensions that users can upload. Separate extensions with a space and do not include the leading dot.'),
  );
  $form['node_images_max_images'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of images'),
    '#default_value' => variable_get('node_images_max_images', 4),
    '#size' => 6,
    '#maxlength' => 2,
    '#description' => t('The maximum number of images a user can upload for each node.'),
  );
  $form['node_images_md5name'] = array(
    '#type' => 'checkbox',
    '#title'=> t('MD5 filenames'),
    '#description' => t('Override uploaded filenames with the MD5 hash of the file.'),
    '#default_value' => variable_get('node_images_md5name', FALSE),
  );

  return system_settings_form($form);
}