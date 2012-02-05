<?php
// $Id: inline.theme.inc,v 1.2 2008/08/17 14:43:39 sun Exp $

/**
 * @file
 * Output theming functions for Inline.
 */

/**
 * Return HTML for a link to a file.
 */
function theme_inline_as_link($file) {
  // Prepare link text with title or filename.
  $linktext = ($file->title ? $file->title : $file->filename);

  return l($linktext, file_create_url($file->filepath), array('attributes' => array('title' => t('Download: @name (@size)', array('@name' => $file->filename, '@size' => format_size($file->filesize))))));
}

/**
 * Return HTML for an image.
 */
function theme_inline_img($file, $field) {
  // Prepare link text with inline title, file description or filename.
  $title = (!empty($file->title) ? $file->title : (!empty($file->description) ? $file->description : $file->filename));
  $inline_preset = $field == 'teaser' ? 'inline_teaser_preset' : 'inline_full_preset';

  if (module_exists('imagecache') && variable_get($inline_preset, '') != '') {
    $image = theme('imagecache',
      variable_get($inline_preset, ''),
      $file->filepath,
      $title,
      $title,
      array('class' => 'inline')
    );
  }
  else {
    $image = theme('image',
      $file->filepath,
      $title,
      $title,
      array('class' => 'inline')
    );
  }

  if (variable_get('inline_link_img', '1')) {
    $attributes = array(
      'class' => 'inline-image-link',
      'title' => t("View") .': '. $title,
    );
    $html = l($image, $file->filepath, array('attributes' => $attributes, 'html' => TRUE));
  }
  else {
    $html = $image;
  }

  return $html;
}

/**
 * Insert an image in front of node teaser.
 * 
 * @param object $node
 *   The node object to process.
 * @param object $file
 *   A file object of an image to insert.
 * @param string $field
 *   The field name to prepend with the image.
 */
function theme_inline_add_to_teaser($node, $file, $field) {
  return theme('inline_img', $file, $field) . $node->teaser;
}

/**
 * Insert an image in front of node body.
 * 
 * @param object $node
 *   The node object to process.
 * @param object $file
 *   A file object of an image to insert.
 * @param string $field
 *   The field name to prepend with the image.
 */
function theme_inline_add_to_body($node, $file, $field) {
  return theme('inline_img', $file, $field) . $node->body;
}


