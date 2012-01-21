<?php
// $Id: node_images.pages.inc,v 1.1.2.6 2009/01/30 13:21:02 stefano73 Exp $

/**
 * @file
 * Page callbacks for adding, editing and deleting node images.
 */

function _node_images_edit_page(&$form_state, $node, $form = array()) {
  global $user;

  $page = empty($form);

  // Wrapper for fieldset contents (used by ahah.js).
  $form['node_images']['wrapper'] = array(
    '#prefix' => '<div id="node_images-attach-wrapper">',
    '#suffix' => '</div>',
  );

  if (!_node_images_check_directory(NULL, $user, $node)) {
    $form['node_images']['#description'] =  t('Node images are disabled. The file directories have not been properly configured.');
    if (user_access('administer site configuration')) {
      $form['node_images']['#description'] .= ' '. t('Please visit the <a href="@admin-node-images">node_images configuration page</a>.', array('@admin-node-images' => url('admin/settings/node_images')));
    }
    else {
      $form['node_images']['#description'] .= ' '. t('Please contact the site administrator.');
    }
    if ($page) drupal_set_message($form['node_images']['#description'], 'error');
  }
  else {
    if ($page) {
      $form['node_images']['wrapper'] += _node_images_edit_form($form_state, $node);
      if (variable_get('node_images_position_'.$node->type, 'hide') == 'hide') {
        drupal_set_message(t('Node images are disabled in the view page for the current node type.'), 'warning');
      }
      $form['nid'] = array('#type' => 'value', '#value' => $node->nid);
    }
    else {
      $form['node_images']['wrapper'] += _node_images_attach_form($form_state, $node);
      $form['#submit'][] = 'node_images_node_form_submit';
    }

    $form['#attributes']['enctype'] = 'multipart/form-data';
  }

  return $form;
}

function _node_images_edit_form(&$form_state, $node) {
  global $user;
  $node_images = node_images_load($node);
  $is_translation_source = _node_images_is_translation_source($node);

  $form = array(
    '#cache' => TRUE,
  );

  if (_node_images_access('create', $node) && $is_translation_source) {
    $form['new'] = array(
      '#type' => 'fieldset',
      '#title' => t('Upload a new image'),
      '#collapsible' => TRUE,
      '#collapsed' => !empty($node_images),
      '#weight' => -30,
      '#theme' => 'node_images_form_upload',
    );
    $limits = _node_images_file_limits($user);
    $form['new']['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#size' => 50,
      '#maxlength' => 255,
      '#default_value' => '',
      '#description' => t('Enter a description for the new image, max 255 chars.'),
    );
    $form['new']['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
    );
    $form['new']['list'] = array(
      '#type' => 'radios',
      '#title' => t('List'),
      '#options' => array(1 => t('Yes'), 0 => t('No')),
      '#default_value' => 1,
    );
    $form['new']['node_images_file'] = array(
      '#type' => 'file',
      '#title' => t('Attach new image'),
      '#size' => 40,
      '#description' => ($limits['resolution'] ? t('Images larger than %resolution will be resized. ', array('%resolution' => $limits['resolution'])) : '') . t('The maximum upload size is %filesize. Only files with the following extensions may be uploaded: %extensions. ', array('%extensions' => $limits['extensions'], '%filesize' => format_size($limits['file_size']))),
    );
    $form['new']['node_images_attach'] = array(
      '#type' => 'submit',
      '#value' => t('Upload'),
      '#name' => 'attach',
      '#ahah' => array(
        'path' => 'node_images/js',
        'wrapper' => 'node_images-attach-wrapper',
        'progress' => array('type' => 'bar', 'message' => t('Please wait...')),
      ),
      '#submit' => array('_node_images_upload_submit'),
    );
    // This value is used in _node_images_js().
    $form['new']['nid'] = array('#type' => 'hidden', '#value' => $node->nid);
  }

  $form['node_images'] = array(
    '#theme' => 'node_images_form_list',
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Image list'),
    '#weight' => 10,
  );

  if (!empty($node_images) && is_array($node_images)) {
    // set an appropriate value for delta in weight selectbox
    $delta = db_result(db_query('SELECT MAX(ABS(weight)) FROM {node_images} WHERE nid=%d', $node->nid));
    $delta += count($node_images);
    $authors = array();
    $submit = FALSE;
    $form['node_images']['is_translation_source'] = array('#type' => 'value', '#value' => $is_translation_source);

    foreach ($node_images as $key => $file) {
      $file = (object)$file;
      $update_access = _node_images_access('update', $node, $file);
      $delete_access = _node_images_access('delete', $node, $file);
      if (!$submit && ($update_access || $delete_access)) $submit = TRUE;
      
      if (!isset($authors[$file->uid])) {
        $authors[$file->uid] = user_load(array('uid' => $file->uid));
      }
      $url = file_create_url($file->filepath);
      $thumb = file_create_url($file->thumbpath);
      $row = array();
      $row['id'] = array('#type' => 'value',  '#value' => $file->id);
      $row['thumbnail'] = array('#value' => '<img src="'.$thumb.'" alt="" />');
              
      if ($is_translation_source) {
        $row['delete'] = array('#type' => 'checkbox', '#attributes' => ($delete_access ? array() : array('disabled' => 'true')));
        $row['list'] = array('#type' => 'checkbox', '#default_value' => $file->list,
          '#attributes' => ($update_access && $is_translation_source ? array() : array('disabled' => 'true')));    
        _node_images_translate($file);
        $element = array('#type' => 'textfield', '#default_value' => $file->description, '#maxlength' => 255, '#size' => 40);
      }
      else {
        _node_images_translate($file);
        $element = array('#value' => $file->description);
      }
      $row['description'] = $element;

      $author = $authors[$file->uid];    
      $row['author'] = array('#value' => theme('username', $author));
      
      $row['filepath'] = array('#value' => l($file->filepath, file_create_url($file->filepath), array('attributes' => array('target' => '_blank'))));
      $row['filesize'] = array('#value' => format_size($file->filesize));
      $row['date'] = array('#value' => format_date($file->timestamp));
      $row['weight'] = array('#type' => 'weight', '#delta' => $delta, '#default_value' => $file->weight);
      $form['node_images']['images'][$key] = $row;
    }

    if ($submit && $is_translation_source) {
      $form['node_images']['submit'] = array('#type' => 'submit', '#value' => t('Save changes'), '#submit' => array('_node_images_form_submit'));
    }
  }

  return $form;
}

/**
 * Save changes to the image list.
 *
 * @param $form
 *   The form structure.
 * @param $form_state
 *   An associative array containing the current state of the form.
 */
function _node_images_form_submit($form, &$form_state) {
  global $user, $language;

  $node = node_load($form_state['values']['nid']);
  if (!isset($node->nid)) return drupal_set_message(t('Unable to load node.'), 'error');

  // Get languages
  $current_language = $language->language;
  $default_language = language_default('language');

  foreach($form_state['values']['node_images']['images'] as $id => $values) {
    $file = db_fetch_object(db_query('SELECT id, filepath, thumbpath, uid, nid FROM {node_images} WHERE id=%d AND nid=%d',
      $id, $node->nid));
    if ($values['delete']) {
      // check delete access for the current image
      $delete_access = _node_images_access('delete', $node, $file);
      if (!$delete_access) continue;
      
      // delete selected image
      file_delete($file->filepath);
      file_delete($file->thumbpath);
      db_query('DELETE FROM {node_images} WHERE id=%d AND nid=%d', $id, $node->nid);
      module_invoke('i18nstrings', 'remove_string', _node_images_build_i18nstrings_context($file));
    }
    else {
      // check update access for the current image
      $update_access = _node_images_access('update', $node, $file);
      if (!$update_access) continue;

      $update = array('id', 'nid');
      $object = (object)$values;
      $object->nid = $node->nid;
      _node_images_translate($object, TRUE);
      drupal_write_record('node_images', $object, $update);
    }
  }
  drupal_set_message(t('The changes have been saved.'));
}

/**
 * Theme the upload form.
 *
 * @ingroup themeable
 */
function theme_node_images_form_upload(&$form) {
  $output = '';
  foreach (array('description', 'weight') as $key) { 
    $output .= '<div style="float:left; margin-right:20px;">';
    $output .= drupal_render($form[$key]);
    $output .= '</div>';
  }
  $output .= '<div style="float:left;">';
  $output .= drupal_render($form['list']);
  $output .= '</div>';

  $output .= '<div style="clear:both;"></div>';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Theme the image list.
 *
 * @ingroup themeable
 */
function theme_node_images_form_list(&$form) {
  $is_translation_source = $form['is_translation_source']['#value'];

  $header = array();
  if ($is_translation_source) {
    $header = array('', t('Delete'), t('List'));
  }
  $header =  array_merge($header, array(t('Thumbnail'), t('Description and info'), t('Weight'), t('Size')));
  drupal_add_tabledrag('node_images_list', 'order', 'sibling', 'node_images-weight');

  foreach (element_children($form['images']) as $key) {
    if (!$form['images'][$key]['thumbnail']) continue;

    // Add class to group weight fields for drag and drop.
    $form['images'][$key]['weight']['#attributes']['class'] = 'node_images-weight';

    $info = '<div class="node_images_info">'.t('Author: !name', array('!name' => drupal_render($form['images'][$key]['author']))).'</div>';
    $info .= '<div class="node_images_info">'.t('Uploaded on: %date', array('%date' => drupal_render($form['images'][$key]['date']))).'</div>';
    $info .= '<div class="node_images_info">'.t('Path: !path', array('!path' => drupal_render($form['images'][$key]['filepath']))).'</div>';

    $row = array();
    if ($is_translation_source) {
      $row[] = '';
      $row[] = drupal_render($form['images'][$key]['delete']);
      $row[] = drupal_render($form['images'][$key]['list']);
    }
    $row[] = drupal_render($form['images'][$key]['thumbnail']);
    $row[] = array('data' => drupal_render($form['images'][$key]['description']).$info, 'width' => '100%');
    $row[] = drupal_render($form['images'][$key]['weight']);
    $row[] = array('data' => drupal_render($form['images'][$key]['filesize']), 'class' => 'nowrap');
    if ($is_translation_source) {
      $rows[] = array('data' => $row, 'class' => 'draggable');
    }
    else {
      $rows[] = $row;
    }
  }

  $output = '&nbsp;';
  if (!empty($rows)) $output .= theme('table', $header, $rows, array('id' => 'node_images_list'));
  $output .= drupal_render($form['translation_warning']);
  $output .= drupal_render($form);
  return $output;
}


/************************************************************
 * Gallery functions
 ************************************************************/

function _node_images_gallery($node) {
  $node->node_images = node_images_load($node, 0, 1);
  if (empty($node->node_images)) {
    drupal_set_message(t('No images uploaded for this content.'));
    if (user_access('create node images') && node_access('update', $node)) {
      $output = t('Click <a href="!url">here</a> to upload new images.', array('!url' => url('node/'.$node->nid.'/images')));
    }
    return '<p>'.$output.'</p>';
  }
  
  $settings = array('images' => array());

  $i = 1;
  $thumbs = array();
  foreach ($node->node_images as $id => $image) {
    // i18n support for description
    if (module_exists('i18nstrings')) {
      // translate description
      to('node_images:node_image:'.$image->id.':description', $image, array('description'));
    }

    $thumb_path = file_create_url($image->thumbpath);
    $thumbs[$i] = array(
      'src' => $thumb_path,
      'title' => $image->description,
    );

    $path = $image->filepath;
    $info = image_get_info($path);
    $settings['images'][$i++] = array(
      'src' => file_create_url($path),
      'title' => $image->description,
      'description' => $image->description,
      'width' => $info['width'],
      'height' => $info['height'],
      'thumb' => $thumb_path,
    );
  }
  $total = count($settings['images']);

  if ($total > 0) {
    $current = (int)$_GET['page'];
    $current = ($current < 1 || $current > $total) ? 1 : $current;

    $settings['current'] = $current;
    $settings['total'] = $total;

    $path = drupal_get_path('module', 'node_images');
    drupal_add_js($path.'/node_images.js');
    drupal_add_js(array('node_images_slideshow' => array($node->nid => $settings)), 'setting');
    drupal_add_css($path.'/node_images.css');

    $slideshow = array(
      '#type' => 'node_images_gallery',
      '#title' => $settings['images'][$current]['title'],
      '#attributes' => array('id' => 'slideshow-'. $node->nid),
      '#status' => array(
        'current' => $current,
        'total' => $total,
        'previous' => $current <= 1 ? $total : $current - 1,
        'next' => $current >= $total ? 1 : $current + 1,
      ),
      '#image' => $settings['images'][$current],
      '#weight' => -5,
    );
  }

  $output = '<div class="node_images_slideshow">';
  $output .= '<div class="large">'.drupal_render($slideshow).'</div>';
  $output .= '<div class="thumbs">'.theme('node_images_gallery_thumbs', $thumbs).'</div>';
  $output .= '</div>';
  $output .= '<hr style="clear:both;" />'.node_view($node);

  $title = t('Photo gallery for %title', array('%title' => $node->title));
  drupal_set_title($title);
  return $output;
}

function theme_node_images_gallery($element) {
  $output = '<div'. drupal_attributes($element['#attributes']) .'>
    <div class="header">
      '. l(t('Previous'), $_GET['q'], array('attributes' => array('class' => 'previous'), 'query' => 'page='. $element['#status']['previous'])) .' |
      '. t('Image !current of !total', array(
        '!current' => '<span class="current">'. $element['#status']['current'] .'</span>',
        '!total' => '<span class="total">'. $element['#status']['total'] .'</span>')) .' |
      '. l(t('Next'), $_GET['q'], array('attributes' => array('class' => 'next'), 'query' => 'page='. $element['#status']['next'])) .'
    </div>
    <img src="'. url($element['#image']['src']) .'" class="polaroid" />
    <p class="description">'. $element['#title'] .'</p>
    </div>';
  return $output;
}

function theme_node_images_gallery_thumbs($thumbs, $cols = 2) {
  $output = '';
  foreach($thumbs as $id => $thumb) {
    $url = url($_GET['q'], array('query' => 'page='.$id, 'absolute' => TRUE));
    $description = '<div class="thumb-description">'.truncate_utf8($thumb['title'], 40, FALSE, TRUE).'</div>';
    $output .= '<div class="thumb">'.l('<img src="'.$thumb['src'].'" class="slideshow-thumb" id="thumb-'.$id.'" />', $url, array('attributes' => array('title' => $thumb['title']), 'html' => TRUE)).$description.'</div>';
  }
  return $output;
}