<?php
// $Id: dashboard.page.inc,v 1.13 2010/07/27 21:41:27 beretta627 Exp $

/**
 * @file
 * Dashboard module page file.
 * Builds and renders Dashboard pages.
 */

/**
 * Add the CSS and JS requirements for the page.
 */
function dashboard_add_ui($page = NULL) {
  dashboard_add_tools();
  drupal_add_css(drupal_get_path('module', 'dashboard') . '/dashboard.css');
  drupal_add_js(drupal_get_path('module', 'dashboard') . '/dashboard.js');
  jquery_ui_add(array('ui.sortable'));
  drupal_add_js(array(
    'dashboardPageAddForm' => drupal_get_form('dashboard_page_add_form'),
  ),'setting');
  if (!is_null($page)) {
    drupal_add_js(array(
      'dashboardPage' => $page->path,
      'dashboardToken' => drupal_get_token('dashboard ' . $page->page_id),
      'dashboardPageEditForm' => drupal_get_form('dashboard_page_edit_form', $page),
    ),'setting');
  }
}

/**
 * Generate a dashboard page.
 *
 * @param $page
 *   Meta-data about the page to be rendered.
 * @return
 *   A fully-themed HTML page.
 */
function dashboard_page($page = NULL) {
  if (is_null($page)) {
    $page = dashboard_user_page_load('');
  }
  dashboard_add_ui($page);

  $widgets = array_fill(0, 3, '');
  $result = db_query('SELECT widget_id, type, subtype, conf, col FROM {dashboard_widget} WHERE page_id = %d ORDER BY weight', $page->page_id);
  while ($widget = db_fetch_object($result)) {
    $content = ctools_content_render($widget->type, $widget->subtype, unserialize($widget->conf));
    $content->widget_id = $widget->widget_id;
    $widgets[$widget->col] .= theme('dashboard_widget', $content);
  }

  return theme('dashboard_page', dashboard_user_tabs(), $widgets);
}

/**
 * FormsAPI for adding a new page to a Dashboard.
 */
function dashboard_page_add_form($form_state) {
  $form = array(
    '#attributes' => array('class' => 'nav-tab'),
  );

  $form['title'] = array(
    '#prefix' => '<div class="container-inline">',
    '#type' => 'textfield',
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#attributes' => array('disabled' => 'disabled'),
    '#suffix' => '</div>',
  );

  return $form;
}

/**
 * FormsAPI submit handler for adding a Dashboard page.
 */
function dashboard_page_add_form_submit($form, &$form_state) {
  global $user;

  // Make a sanitized & unique path.
  $pages = dashboard_user_page_load();
  $used_paths = array_keys($pages);
  $used_paths[] = $pages['']->path;
  $used_paths[] = 'widgets';

  $path = $base_path = preg_replace('/[^a-z0-9-]/', '', drupal_strtolower(str_replace(' ', '-', $form_state['values']['title'])));
  $n = 0;
  while (in_array($path, $used_paths)) {
    $n += 1;
    $path = $base_path . '-' . $n;
  }

  $page = array(
    'uid' => $user->uid,
    'path' => $path,
    'weight' => db_result(db_query("SELECT max(weight) FROM {dashboard_page} WHERE uid = %d", $user->uid)) + 1,
    'title' => $form_state['values']['title'],
  );
  drupal_write_record('dashboard_page', $page);

  $form_state['redirect'] = 'dashboard/'. $path;
}

/**
 * FormsAPI for editing a Dashboard page.
 */
function dashboard_page_edit_form($form_state, $page) {
  $form = array();

  $form['edit_title'] = array(
    '#prefix' => '<div class="edit container-inline">',
    '#type' => 'textfield',
    '#default_value' => $page->title,
    '#size' => 15,
    '#required' => TRUE,
  );
  $form['page_id'] = array(
    '#type' => 'value',
    '#value' => $page->page_id,
  );
  $form['edit_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#ahah' => array(
      'event' => 'click',
      'path' => 'dashboard/'. $page->path .'/rename',
    ),
    '#suffix' => '</div>',
  );
  $form['delete'] = array(
    '#prefix' => '<div class="delete container-inline">',
    '#type' => 'submit',
    '#value' => t('Deleting…'),
    '#suffix' => '<a href="#" class="cancel">' . t('Cancel') . '</a></div>',
  );

  return $form;
}

/**
 * Page callback for changing the name of a dashboard page.
 *
 * @param $page
 *   The page beign edited.
 * @return
 *   A JSON-enabled form.
 */
function dashboard_page_rename($page) {
  drupal_get_form('dashboard_page_edit_form', $page);
  drupal_json(array('status' => TRUE, 'data' => ''));
}

/**
 * FormsAPI submit handler for changing a Dashboard page.
 */
function dashboard_page_edit_form_submit($form, &$form_state) {
  global $user;

  switch ($form_state['clicked_button']['#id']) {
    case 'edit-edit-submit':
      $page = array(
        'page_id' => $form_state['values']['page_id'],
        'title' => $form_state['values']['edit_title'],
      );
      drupal_write_record('dashboard_page', $page, array('page_id'));
      break;

    case 'edit-delete':
      db_query("DELETE FROM {dashboard_page} WHERE page_id = %d", $form_state['values']['page_id']);
      db_query("DELETE FROM {dashboard_widget} WHERE page_id = %d", $form_state['values']['page_id']);
      $form_state['redirect'] = 'dashboard';
      break;
  }
}

/**
 * Menu callback for changing the order of a dashboard page.
 */
function dashboard_page_reorder() {
  foreach (explode(',', $_POST['pages']) as $weight => $page_id) {
    $page = array(
      'page_id' => $page_id,
      'weight' => $weight,
    );
    drupal_write_record('dashboard_page', $page, array('page_id'));
  }
}

/**
 * Menu callback for changing the order of a widgets on a page.
 *
 * @param $page
 *   The page being edited.
 */
function dashboard_widget_reorder($page) {
  global $user;

  $page_id = db_result(db_query("SELECT page_id FROM dashboard_page WHERE uid = '%d'", $user->uid));
  
  foreach (range(0, 2) as $column) {
    foreach (explode(',', $_POST['column_'. $column]) as $weight => $widget_id) {
      $widget = array(
        'widget_id' => $widget_id,
        'page_id' => $page_id,
        'col' => $column,
        'weight' => $weight,
      );
      drupal_write_record('dashboard_widget', $widget, array('widget_id', 'page_id'));
    }
  }
}

/**
 * Menu callback for adding a widget to page.
 */
function dashboard_widget_add() {

  // Get the widget_id sent by ajax post request
  $widget_id = $_POST['widget_id'];
  $result = array();
  if ($widget_id) {

    // Get default dashboard page id for logged user
    $page_id = dashboard_get_default_page_id();

    // Get default widget data
    $res = db_query('SELECT * FROM {dashboard_default} WHERE id = %d', $widget_id);
    while ($default_widget = db_fetch_object($res)) {
      $result['widget_type'] = $default_widget->widget_type;
      $result['subtype'] = $default_widget->subtype;
      $result['conf'] = $default_widget->conf;
    }

    // Get remaining data for insert
    $type = $result['widget_type'];
    $subtype = $result['subtype'];
    $conf = $result['conf'];
    $col = 0;
    $weight = 0;

    // Replace existing row with the new one
    db_query("DELETE FROM {dashboard_widget} WHERE widget_id = %d AND page_id = %d", $widget_id, $page_id);
    db_query("INSERT INTO {dashboard_widget} (widget_id, page_id, type, subtype, conf, col, weight)
    VALUES (%d, %d, '%s', '%s', '%s', %d, %d)", $widget_id, $page_id, $type, $subtype, $conf, $col, $weight);

    $result['success'] = TRUE;
    $result['label'] = t('Added to dashboard.');
  }
  else {
    $result['success'] = FALSE;
  }
  return drupal_json($result);
}


/**
 * Menu callback for deleting a widget.
 */
function dashboard_widget_remove() {
  global $user;
  $page_id = db_result(db_query("SELECT {dashboard_page}.page_id FROM {dashboard_page} LEFT JOIN {dashboard_widget} ON {dashboard_page}.page_id = {dashboard_widget}.page_id WHERE uid = %d AND widget_id = %d", $user->uid, $_POST['widget_id']));
  if ($page_id) {
    db_query("DELETE FROM {dashboard_widget} WHERE widget_id = %d AND page_id = %d", $_POST['widget_id'], $page_id);
  }
}

/**
 * Generate a list of widgets for people to add to their personal dashboards.
 * @param unknown_type $page
 * @return unknown_type
 */
function dashboard_widgets_page($page) {

  dashboard_add_ui($page);
  $output = theme('dashboard_widget_browser');

  return $output;
}

/**
 * Get default dashboard page id for logged user
 */
function dashboard_get_default_page_id() {
  global $user;
  if (!empty($user->uid)) {
    $result = db_query('SELECT page_id FROM {dashboard_page} WHERE uid = %d ORDER BY page_id ASC', $user->uid);
    $row = db_fetch_object($result);
    if ($row->page_id) {
      return $row->page_id;
    }
  }
  return FALSE;
}

/**
 * Generate a list of public widgets
 */
function dashboard_get_public_defaults() {

  // selects a list of all widgets
  $output = '';
  $query = db_query("SELECT * FROM {dashboard_default}");
  $widgets = array();

  while ($results = db_fetch_object($query)) {
    $widgets[] = $results;
  }
  return $widgets;
}

/**
 * Preprocessor for the browse widgets page.
 * @param array $vars Variables
 */
function template_preprocess_dashboard_display_widget(&$vars) {

  $widget = $vars['widget'];

  // controls whether or not we see thumbnails
  if (variable_get('dashboard_thumbs', 0)) {
    if ($widget->thumbnail !== '') {
      $img = base_path() . $widget->thumbnail;
    } else {
      $img = base_path() . variable_get('dashboard_thumb_default', '');
    }
    $vars['thumbnail'] = "<img src='" . $img . "' >";
  }
  $widget_list = dashboard_load_widgets();

  //is_object prevents mistaking a page_id with a widget_id

  if (isset($widget_list[$widget->id]) && is_object($widget_list[$widget->id])) {
    // Add the "Add to Dashboard" link to template
    $vars['add_to_dashboard'] = t('Already on your dashboard.');
  }
  else {
    // Add the "Add to Dashboard" link to template
    $vars['add_to_dashboard'] = '<a href="#" class="add-widget" id="add-widget-'. $widget->id .'">'. t('Add to Dashboard') .'</a>';
  }
}

/**
 * Preprocessor for the browse widgets page.
 * @param array $vars Variables
 */
function template_preprocess_dashboard_widget_browser(&$vars) {

  $widgets = dashboard_get_public_defaults();

  $form_state = array(
    'widgets' => $widgets,
    'input' => $_POST,
    'method' => 'post',
    'rerender' => TRUE,
    'no_redirect' => TRUE,
  );

  //TODO move back to CORE FAPI
  //TODO remove all filters except tags, display tags as checkboxes
  // $vars['widget_filter'] = drupal_build_form('dashboard_list_widget_form', $form_state);
  $vars['widget_filter'] = drupal_build_form('dashboard_select_widget_form', $form_state);

  $sorts = array();

  foreach ($widgets as $widget) {
    //TODO: change this logic to accomodate the checkboxes
    $check = false;

    foreach ($form_state['values']['tags'] as $tag) {
      if ($widget->tags === $tag || $form_state['values']['default'] === 'true') {
        $check = true;
      }
    }
    if ($check !== true) {
      continue;
    }

    // TODO: get all the right fields in here
    $item = new stdclass();
    $item->id = $widget->id;
    $item->title = check_plain($widget->title);
    $item->type = $widget->widget_type;
    $item->subtype = $widget->subtype;
    $item->thumbnail = '';
    $item->description = check_plain($widget->description);
    $item->default_enabled = $widget->default_enabled;

    $item->ops = array();
    $item->ops[] = l(t('Edit'), "admin/settings/dashboard/widget/$widget->id/edit");
    $item->ops[] = l(t('Delete'), "admin/settings/dashboard/widget/$widget->id/delete");
    $item->ops = implode(' | ', $item->ops);

    if (!empty($widget->tags)) {
      $item->tag = check_plain($widget->tags);
    }

    $item->classes = empty($widget->default_enabled) ? 'widget-enabled' : 'widget-disabled';
    $items[] = $item;

    $sort = '';
    switch ($form_state['values']['order']) {
      case 'title':
      default:
        $sort = strtolower($widget->title);
        break;
      case 'type':
        $sort .= strtolower($widget->type);
        break;
      case 'tag':
        $sort .= strtolower($widget->tags);
        break;
      case 'desc':
        $sort .= strtolower($widget->description);
        break;
    }
    $sorts[] = $sort;
  }

  $i = array();
  foreach ($sorts as $id => $title) {
    $i[] = $items[$id];
  }

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $vars['widgets'] = $i;

}

/**
 * Provide a form for filtering the public list of widgets.
 */
function dashboard_select_widget_form(&$form_state) {

  // check for clean urls
  if (!variable_get('clean_url', FALSE)) {
    $form['q'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['q'],
    );
  }

  if (sizeof($form_state['input']) == 0) {
      $form['default'] = array(
        '#type' => 'hidden',
        '#value' => 'true',
      );
    }

  $tags = array();
  foreach ($form_state['widgets'] as $name => $widget) {
    if (!empty($widget->tags)) {
      $tags[$widget->tags] = check_plain($widget->tags);
    }
  }
  asort($tags);

  if ($form_state['input']['tags']) {
    $defaults = $form_state['input']['tags'];
  } else {
    $defaults = array_merge($tags);
  }

  $form_state['values']['tags'] = $defaults;

  //TODO: change default value to reflect selected tags
  $form['tags'] = array(
    '#type' => 'checkboxes',
    '#prefix' => '<h3 class="form-title">'. t('Categories') .'</h3>',
    '#options' => array_merge($tags),
    '#default_value' => $defaults,
  );

  $form['submit'] = array(
    '#name' => '', // so it won't in the $_GET args
    '#type' => 'submit',
    '#id' => 'edit-widget-apply',
    '#value' => t('Apply'),
  );
  //TODO change the form theme to something more appropriate
  // $form['#theme'] = array('dashboard_list_widget_form_form');
  return $form;
}
