<?php
// $Id: simpleviews.pages.inc,v 1.1.2.1 2008/09/26 19:28:46 eaton Exp $

/**
 * Build the overview form.
 *
 * Loads all items and builds an overview form with weight elements for each
 * item.
 *
 * @ingroup forms
 * @see _simpleviews_overview_item_field()
 * @see simpleviews_overview_form_submit()
 * @see theme_simpleviews_overview_form()
 */
function simpleviews_overview_form(&$form_state) {
  $results = db_query("SELECT * FROM {simpleviews}");
  $items = array();
  while ($item = db_fetch_array($results)) {
    $items[$item['svid']] = $item;
  }

  $form['items']['#tree'] = TRUE;
  foreach ($items as $svid => $item) {
    $form['items'][$svid] = _simpleviews_overview_item_field($item);
  }

  return $form;
}


/**
 * Build the overview form fields for a single item.
 *
 * @see simpleviews_overview_form()
 */
function _simpleviews_overview_item_field($item) {
  $form['svid'] = array(
    '#type' => 'hidden',
    '#value' => $item['svid'],
  );

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => check_plain($item['title']),
  );

  $form['path'] = array(
    '#type' => 'markup',
    '#value' => l($item['path'], $item['path']),
  );

  $path = drupal_get_path('module', 'simpleviews') . '/images/';
  $links[] = array(
    'title' => theme('image', $path . 'text-editor.png', t('Edit')),
    'href' => 'admin/build/simpleviews/' . $item['svid'] . '/edit',
    'html' => TRUE,
  );
  $links[] = array(
    'title' => theme('image', $path . 'edit-delete.png', t('Delete')),
    'href' => 'admin/build/simpleviews/' . $item['svid'] . '/delete',
    'html' => TRUE,
  );

  $form['operations'] = array(
    '#type' => 'markup',
    '#value' => theme('links', $links),
  );

  return $form;
}

/**
 * Theme the overview form.
 *
 * Arranges items in a table.
 *
 * @ingroup themeable
 * @ingroup forms
 * @see simpleviews_overview_form()
 */
function theme_simpleviews_overview_form($form) {
  $rows = array();
  foreach (element_children($form['items']) as $key) {
    $row = array();

    // Render the hidden 'item id' field and the title of the item into the
    // same column of the row.
    $row[] = drupal_render($form['items'][$key]['svid']) . drupal_render($form['items'][$key]['title']);
    $row[] = drupal_render($form['items'][$key]['path']);
    $row[] = drupal_render($form['items'][$key]['operations']);
    $rows[] = $row;
  }

  // If there were no items found, note the fact so users don't get confused
  // by a completely empty table.
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No items have been added.'), 'colspan' => 3));
  }

  // Render a list of header titles, and our array of rows, into a table. Even
  // we've already rendered all of our items, we always call drupal_render()
  // on the form itself after we're done, so hidden security fields and other
  // elements (like buttons) will appear properly at the bottom of the form.
  $header = array(t('Title'), t('Path'), t('Operations'));
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form);

  return $output;
}


/* Individual item editing form */

/**
 * Build the record editing form.
 *
 * If a record is passed in, an edit form with both Save and Delete buttons will
 * be built. Otherwise, a blank 'add new record' form, without the Delete button,
 * will be built.
 *
 * @ingroup forms
 * @see simpleviews_form_submit()
 * @see simpleviews_form_delete()
 */
function simpleviews_form(&$form_state, $simpleview = array()) {
  $form['simpleview'] = _simpleviews_form($simpleview);

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  // Only show the delete button if we already have an ID. Set the delete
  // button's submit handler to a custom function that should only fire if
  // this button is clicked. In all other cases, the form will fall back to
  // the default $form_id_submit() function.
  if (!empty($simpleview['svid'])) {
    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('simpleviews_form_delete'),
    );
  }
  
  return $form;
}

/**
 * Build the record editing chunk, in a reusable fashion.
 */
function _simpleviews_form($simpleview = array()) {
  $simpleview += simpleviews_default_data();

  // We theme this explicitly, rather than overriding the entire form,
  // so that modules re-use this chunk of form to on their own pages and still
  // get the proper formatting.
  $form['#theme'] = 'simpleviews_edit_form';

  if (!empty($simpleview['svid'])) {
    $form['svid'] = array(
      '#type' => 'value',
      '#value' => $simpleview['svid'],
    );
  }

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => $simpleview['title'],
  );

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#required' => TRUE,
    '#default_value' => $simpleview['path'],
  );

  $form['filter'] = array(
    '#type' => 'select',
    '#title' => t('Display'),
    '#options' => array(
      'all' => t('All posts'),
    ),
    '#required' => TRUE,
    '#default_value' => $simpleview['filter'],
  );
  foreach (node_get_types('names') as $type => $name) {
    $form['filter']['#options']['node:'. $type] = t('!type posts', array('!type' => $name));
  }

  $form['sort'] = array(
    '#type' => 'select',
    '#title' => t('Sorted'),
    '#options' => array(
      'newest' => t('Newest first'),
      'oldest' => t('Oldest first'),
      'a-z' => t('By title'),
    ),
    '#required' => TRUE,
    '#default_value' => $simpleview['sort'],
  );
  if (module_exists('statistics')) {
    $form['sort']['#options']['popular'] = t('By number of hits');
  }
  if (module_exists('votingapi')) {
    $form['sort']['#options']['top-rated'] = t('By rating');
  }

  $form['style'] = array(
    '#type' => 'select',
    '#title' => t('As a'),
    '#options' => array(
      'full' => t('List of full posts'),
      'teasers' => t('List of teasers'),
      'titles' => t('List of titles'),
      //'grid' => t('Grid of thumbnails'),
    ),
    '#required' => TRUE,
    '#default_value' => $simpleview['style'],
  );

  $form['argument']['#element_validate'] = array('simpleviews_preprocess_arg_element');
  $form['argument']['arg_toggle'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let visitors filter the page'),
    '#default_value' => !empty($simpleview['argument']),
  );

  $form['argument']['argument'] = array(
    '#type' => 'select',
    '#title' => t('By'),
    '#required' => TRUE,
    '#options' => array(
      'date' => t('The date posts were written'),
      'author' => t('The author of the post'),
    ),
    '#default_value' => $simpleview['argument'],
  );
  foreach (taxonomy_get_vocabularies() as $key => $vocab) {
    $form['argument']['argument']['#options']['term:' . $vocab->vid] = "The post's $vocab->name";
  }

  $form['rss'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add an RSS feed'),
    '#default_value' => $simpleview['rss'],
  );
  
  $form['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create a sidebar widget'),
    '#default_value' => $simpleview['rss'],
  );

  drupal_alter('simpleview_reusable_form', $form, $simpleview);
  
  return $form;
}


/**
 * A slightly unholy hack to preserve the behavior of the 'filter' checkbox
 * even though we never store it explicitly.
 */
function simpleviews_preprocess_arg_element(&$element, &$form_state) {
  if (empty($element['arg_toggle']['#value'])) {
    $key = $element['argument']['#name'];
    $form_state['values'][$key] = '';
  }
}

/**
 * Simple theme wrapper for the simpleview edit form, adds CSS and JS.
 */
function theme_simpleviews_edit_form($form) {
  drupal_add_css(drupal_get_path('module', 'simpleviews') . '/simpleviews.css');
  drupal_add_js(drupal_get_path('module', 'simpleviews') . '/simpleviews.js');

  $extra = empty($form['argument']['argument']['#default_value']) ? ' class="js-hide"' : '';
  $form['argument']['argument']['#prefix'] = '<div id="simpleviews-arg-wrapper"' . $extra . '>';
  $form['argument']['argument']['#suffix'] = '</div>';

  $output .= '<div id="simpleviews-elements">' . drupal_render($form) . '</div>';
  return $output;
}


/**
 * General submit handler for the add/edit form.
 *
 * Simply passes incoming form values on to the module's CRUD save function,
 * then redirects to the overview form.
 *
 * @ingroup formapi
 * @see simpleviews_form()
 */
function simpleviews_form_submit($form, &$form_state) {
  $item = $form_state['values'];
  simpleviews_item_save($item);
  views_flush_caches();
  $form_state['redirect'] = 'admin/build/simpleviews';
}


/**
 * Delete button submit handler for the add/edit form.
 *
 * Redirects to the 'delete item' confirmation page without performing any
 * operations.
 *
 * @ingroup formapi
 * @see simpleviews_form()
 */
function simpleviews_form_delete($form, &$form_state) {
  $form_state['redirect'] = 'admin/build/simpleviews/' . $form_state['values']['svid'] . '/delete';
}


/**
 * Build the delete confirmation form.
 *
 * A simple wrapper around Drupal's core confirm_form() function. Adds a value
 * field to store the ID of the item being deleted.
 *
 * @ingroup forms
 * @see simpleviews_delete_confirm_submit()
 * @see confirm_form()
 */
function simpleviews_delete_confirm(&$form_state, $item) {
  $form['svid'] = array(
    '#type' => 'value',
    '#value' => $item['svid'],
  );

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $item['title'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/build/simpleviews',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * General submit handler for the delete confirmation form.
 *
 * Core's confirm_form() function adds the 'confirm' value element we check
 * against to ensure the form was properly submitted. If it's there, delete
 * the item and redirect to the overview form.
 *
 * @ingroup formapi
 * @see simpleviews_form()
 */

function simpleviews_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    simpleviews_item_delete($form_state['values']['svid']);
    drupal_set_message(t('Your item was deleted.'));
  }
  $form_state['redirect'] = 'admin/build/simpleviews';
}
