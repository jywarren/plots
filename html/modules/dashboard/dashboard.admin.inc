<?php
// $Id: dashboard.admin.inc,v 1.8 2010/07/27 21:41:27 beretta627 Exp $

/**
 * @file
 * Dashboard module admin file.
 * Stores configuration and administrative elements for Dashboard.
 */
/**
 * Page callback to list views in the system.
 */
function dashboard_overview($arg = NULL) {
  if ($arg != NULL) {
    return drupal_not_found();
  }

  drupal_add_css(drupal_get_path('module', 'dashboard') . '/dashboard.css');
  $output = theme('dashboard_list_widget');

  return $output;
}

/**
 * Page callback for Dashboard general settings.
 */
function dashboard_general($arg = NULL) {
  if ($arg != NULL) {
    return drupal_not_found();
  }

  $output = 'General settings page placeholder';

  return $output;
}

/**
 * FormsAPI for module settings page
 */
function dashboard_settings_form(&$form_state, $dashboard = 'default') {
	//dashboard_add_tools();
	$form = array('#attributes' => array('enctype' => 'multipart/form-data'));


	//TODO: enable personalization settings. These settings are purposefully left disabled for the time being, until
	// personalization features have been enabled for the module.

  $form['personalization'] = array(
    '#type' => 'fieldset',
    '#title' => t('Personalization'),
    '#description' => t('Personalization settings for users creating dashboards. <i>These features are purposefully disabled for now.</i>'),
  );

  $form['personalization']['dashboard_personal_dashboards'] = array(
    '#type' => 'radios',
    '#title' => t('Personal Dashboards'),
    '#default_value' => variable_get('dashboard_personal_dashboards', 0),
    '#options' => array(0 => t('Disabled'), 1 => t('Enabled (for users with the "Add Personal Dashboard" permission)')),
    '#description' => t('Controls whether or not users are allowed to create personalized dashboards.')
  );

  $form['personalization']['dashboard_personal_content'] = array(
    '#type' => 'radios',
    '#title' => t('Create Widgets'),
    '#default_value' => variable_get('dashboard_personal_content', 0),
    '#options' => array(0 => t('Disabled'), 1 => t('Enabled (for users with the "Add Personal Dashboard" permission)')),
    '#description' => t('Are users allowed to add content to their dashboards from anywhere in the system?')
  );

  $form['dashboard_widget_content'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget Types'),
  );

  $content = array(
    'nodes' => 'nodes',
    'blocks' => 'blocks',
    //'views' => 'views',
  );

  //TODO: change default value to reflect selected tags
  $form['dashboard_widget_content']['dashboard_add_content'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Specify the types of content users can add to their personalized dashboards. <i>These features are purposefully disabled for now.</i>'),
    '#options' => $content,
    '#default_value' => variable_get('dashboard_add_content', array('nodes' => 'nodes', 'blocks' => 'blocks')),
  );

  // create a directory for storing thumbnails
  if (variable_get('dashboard_thumbs', 0)) {
    $thumb_path = file_create_path(variable_get('dashboard', 'thumbnails'));
    file_check_directory($thumb_path, 1, 'dashboard_thumbnail_path');
  }
  $thumb_support = variable_get('dashboard_thumbs', 0);

  $form['dashboard_thumbnails'] = array(
    '#type' => 'fieldset',
    '#title' => t('Widget Thumbnails'),
  );
  $form['dashboard_thumbnails']['dashboard_thumbs'] = array(
    '#type' => 'radios',
    '#title' => t('Thumbnail support'),
    '#default_value' => $thumb_support,
    '#options' => array(t('Disabled'), t('Enabled')),
  );

  $default_image = drupal_get_path('module', 'dashboard') .'/images/widget-default.png';

  $form['dashboard_thumbnails']['dashboard_thumb_default'] = array(
    '#type' => 'value',
    '#value' => variable_get('dashboard_thumb_default', $default_image)
  );

  $form['dashboard_thumbnails']['dashboard_thumb_fid'] = array(
    '#type' => 'value',
    '#value' => variable_get('dashboard_thumb_fid', 0)
  );

  $file = db_fetch_object(db_query("SELECT * FROM {files} WHERE fid = %d", variable_get('dashboard_thumb_fid', 0)));
  $default_image = '';
  if ($file) {
    $default_image = theme('image', $file->filepath);
  }
  $form['dashboard_thumbnails']['dashboard_thumb_file'] = array(
    '#prefix' => $default_image,
    '#type' => 'file',
    '#title' => t('Default thumbnail'),
    '#description' => t('Upload the default file'),
  );

  // TODO: remove? system_settings_form is adding own submit button
  // $form['submit'] = array(
  //   '#type' => 'submit',
  //   '#value' => t('Submit'),
  // );

  return system_settings_form($form);

}

/**
 * Handle upload of new file
 */
function dashboard_settings_form_validate($form, &$form_state) {
  $validators = array('file_validate_is_image' => array());
  if ($file = file_save_upload('dashboard_thumb_file', $validators, file_directory_path())) {
    // If there is old file delete it
    $old_file = db_fetch_object(db_query("SELECT * FROM {files} WHERE fid = %d", $form_state['values']['dashboard_thumb_fid']));
    if ($old_file) {
      file_delete($old_file->filepath);
      db_query("DELETE FROM {files} WHERE fid = %d", $old_file->fid);
    }

    $form_state['values']['dashboard_thumb_fid'] = $file->fid;
    $form_state['values']['dashboard_thumb_default'] = $file->filepath;
  }
}

/**
 * Preprocess the list widget theme
 */
function template_preprocess_dashboard_list_widget(&$vars) {
  drupal_add_js(drupal_get_path('module', 'dashboard') . '/dashboard.admin.js');
  $widgets = dashboard_get_all_defaults();

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $form_state = array(
    'widgets' => $widgets,
    'input' => $_POST,
    'method' => 'post',
    'rerender' => TRUE,
    'no_redirect' => TRUE,
  );

  //TODO move back to CORE FAPI
  $vars['widget_filter'] = drupal_build_form('dashboard_list_widget_form', $form_state);
  $sorts = array();

  foreach ($widgets as $widget) {
    if ($form_state['values']['tag'] != 'all') {
      if ($form_state['values']['tag'] == 'none') {
        if (!empty($widget->tags)) {
          continue;
        }
      }
      else if ($form_state['values']['tag'] != $widget->tags) {
        continue;
      }
    }
    //dsm($widget);
    if ($form_state['values']['type'] != 'all' && $form_state['values']['type'] != $widget->widget_type) {
      continue;
    }

    // TODO: get all the right fields in here
    $item = new stdclass();
    $item->title = $widget->title;
    $item->type = $widget->widget_type;
    $item->subtype = $widget->subtype;
    $item->thumbnail = '';
    $item->description = check_plain($widget->description);
    $item->default_enabled = $widget->default_enabled;

    $item->ops = array();
    //$item->ops[] = l(t('Edit'), "admin/settings/dashboard/widget/$widget->id/edit");
    $item->ops[] = ctools_modal_text_button('Edit', "admin/settings/dashboard/widget/$widget->id/edit", $alt, $class);
    //@TODO: set configurable defaults
    if($widget->id == 1) {
      $item->ops[] = t('Default');
    } else {
      $item->ops[] = l(t('Delete'), "admin/settings/dashboard/widget/$widget->id/delete");
    }
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

  if ($form_state['values']['sort'] == 'desc') {
    arsort($sorts);
  } else {
    asort($sorts);
  }

  $i = array();
  foreach ($sorts as $id => $title) {
    $i[] = $items[$id];
  }

  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  ctools_modal_add_js();

  $vars['widget_add'] = ctools_modal_text_button("Add new widget", 'admin/settings/dashboard/add/modal', $alt, $class);

  $vars['widgets'] = $i;
}

/**
 * generate a list of default widgets
 */
function dashboard_get_all_defaults() {

  // select distinct from views table
  // iterates through and displays

  $output = '';
  $query = db_query("SELECT * FROM {dashboard_default}");

  $widgets = array();

  while ($results = db_fetch_object($query)) {
  	$widgets[] = $results;
  }

  return $widgets;
}

/**
 * Provide a form for sorting and filtering the list of widgets within the administrator. Provides more options than the public version.
 */
function dashboard_list_widget_form(&$form_state) {
 if (!variable_get('clean_url', FALSE)) {
    $form['q'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['q'],
    );
  }

  $all = array('all' => t('<All>'));
  $none = array('none' => t('<None>'));

  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Storage'),
    '#options' => array(
      'all' => t('<All>'),
      'block' => t('Blocks'),
      'node' => t('Nodes'),
    ),
    '#default_value' => 'all',
  );

  $tags = array();

  $extras = array();
  foreach ($form_state['widgets'] as $name => $widget) {
  	if (!empty($widget->tags)) {
      $tags[$widget->tags] = $widget->tags;
    }
  }

  asort($tags);

  $form['tag'] = array(
    '#type' => 'select',
    '#title' => t('Tag'),
    '#options' => array_merge($all, $none, $tags),
    '#default_value' => 'all',
  );

  $form['order'] = array(
    '#type' => 'select',
    '#title' => t('Sort by'),
    '#options' => array(
      'title' => t('Title'),
      'tag' => t('Tag'),
      'widget_type' => t('Type'),
      'desc' => t('Description'),
    ),
    '#default_value' => 'name',
  );

  $form['sort'] = array(
    '#type' => 'select',
    '#title' => t('Order'),
    '#options' => array(
      'asc' => t('Up'),
      'desc' => t('Down'),
    ),
    '#default_value' => 'asc',
  );

  $form['submit'] = array(
    '#name' => '', // so it won't in the $_GET args
    '#type' => 'submit',
    '#id' => 'edit-widget-apply',
    '#value' => t('Apply'),
  );

  if (!empty($_SESSION['dashboard']['#admin'])) {
    $form['reset'] = array(
      '#type' => 'submit',
      '#id' => 'edit-widget-reset',
      '#value' => t('Reset'),
    );
  }

  $form['#theme'] = array('dashboard_list_widget_form_form');
  return $form;
}

/**
 * generate a widget column
 *
 * @return unknown
 */
function theme_dashboard_admin_widget() {
  return '';
}


function theme_dashboard_list_widget_form_form($form) {
  // Don't render these:
  unset($form['form_id']);
  unset($form['form_build_id']);
  unset($form['form_token']);
  return drupal_render($form);
}

function dashboard_wizard_modal() {
  $step = arg(5);
	ctools_include('wizard');
  ctools_include('ajax');
  ctools_include('modal');
  drupal_add_js('misc/autocomplete.js');
  $form_info = array(
    // think of this ID like a "sub" hook
    'id' => 'dashboard_generator',
    'js' => array('misc/autocomplete.js', 'misc/textarea.js', 'misc/collapse.js'),
    'path' => "admin/settings/dashboard/add/modal/%step",
    'show back' => TRUE,
    'show cancel' => TRUE,
    'next text' => 'Proceed to next step',
    'next callback' =>  'dashboard_generator_add_subtask_next',
    'finish callback' => 'dashboard_generator_add_subtask_finish',
    'cancel callback' => 'dashboard_generator_add_subtask_cancel',
    'order' => array(
      'information' => t('Step 1: Widget information'),
      'selection' => t('Step 2: Widget selection'),
    ),
    'forms' => array(
      'information' => array(
         // see docs in advanced help for more options here such as file path
        'form id' => 'dashboard_generator_form_information'
      ),
      'selection' => array(
        'form id' => 'dashboard_generator_form_selection'
      ),
    ),
  );

  // Load widget for editing
  if ($step == 'edit') {
    $dashboard = db_fetch_object(db_query("SELECT * FROM {dashboard_default} WHERE id = %d", arg(4)));
    $step = 'information';

    // Fix autocomplete field default value to look like autocomplete output
    switch ($dashboard->widget_type) {
      case 'block':
        list($module, $delta) = explode('-', $dashboard->subtype, 2);
        $module_block = module_invoke($module, 'block', 'list');
        if ($module_block && $module_block[$delta]) {
          $block = $module_block[$delta];
          $dashboard->subtype = $block['info'] ." [module:". $module ." delta:". $delta ."]";
        }
        break;
      case 'node':
        list($type, $nid) = explode('-', $dashboard->subtype);
        $node = node_load($nid);
        $dashboard->subtype = $node->title ." [nid:$nid]";
        break;
    }
    // Update cache
    dashboard_generator_set_page_cache($dashboard);
  }
  else {
    // **  always load the stored values
    $dashboard = dashboard_generator_get_page_cache('');
  }
  // if there is none set up the default storage object
  if (!$dashboard) {
    // set form to first step -- we have no data
    $step = current(array_keys($form_info['order']));
    // initialize your storage object
    $dashboard = new stdClass();
    // ** set the storage object so its ready for whatever comes next
    dashboard_generator_set_page_cache($dashboard);
  }

  $form_state = array(
    'cache name' => '',
    'cstorage' => $dashboard,
    'ajax' => TRUE,
    'modal' => TRUE,
    'commands' => array(),
  );

  ctools_wizard_multistep_form($form_info, $step, $form_state);
}

/*-------------------------- PART 2 JUST CLASSIC FORMAPI  ---------------------- */

/**
 * All forms within this wizard will take $form, and $form_state by reference
 * note that the form doesn't have a return value.
 */
function dashboard_generator_form_information(&$form, &$form_state) {
  $widget = &$form_state['cstorage'];
  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => 'Widget title',
    '#default_value' => $widget->title,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => 'Widget description',
    '#default_value' => $widget->description,
  );
  $form['thumbnail'] = array(
    '#type' => 'file',
    '#required' => 0,
    '#title' => 'Widget thumbnail',
    '#default_value' => $widget->thumbnail,
  );
  $form['tags'] = array(
    '#type' => 'textfield',
    '#required' => 1,
    '#title' => 'Widget tags',
    '#default_value' => $widget->tags,
    '#autocomplete_path' => 'admin/settings/dashboard/ajax/autocomplete/tag',
  );
  $form['widget_type'] = array(
    '#title' => 'Widget type',
    '#type' => 'radios',
    '#required' => 1,
    '#default_value' => $widget->widget_type,
    '#options' => array(
      'block' => 'Block',
      'node' => 'Node',
      //'view' => 'View',
    )
  );
  $form_state['no buttons'] = TRUE;
}

/**
 * Page callback for views tag autocomplete
 */
function dashboard_autocomplete_tag($string = '') {
  $matches = array();
  // get matches from default views:
  $widgets = dashboard_get_all_defaults();
  foreach ($widgets as $widget) {
    if (!empty($widget->tags) && strpos($widget->tags, $string) === 0) {
      $matches[$widget->tags] = $widget->tags;
    }
  }

  if ($string) {
    $result = db_query_range("SELECT DISTINCT tags FROM {dashboard_default} WHERE LOWER(tags) LIKE LOWER('%s%%')", $string, 0, 10 - count($matches));
    while ($widget = db_fetch_object($result)) {
      $matches[$widget->tags] = check_plain($widget->tags);
    }
  }

  drupal_json($matches);
}

/**
 * Note that this validate callback operates exactly like it does in the regular form api
 */
function dashboard_generator_form_information_validate(&$from, &$form_state) {
}

/**
 * KEY CONCEPT: generally, you will never save data here -- you will simply add values to the
 * yet to be saved ctools_cache object.
 *
 * Saving happens at the end, within the $form_info['finish callback'];
 */
function dashboard_generator_form_information_submit(&$from, &$form_state) {
  $submitted = $form_state['values'];
  $save_values = array('title', 'description', 'thumbnail', 'widget_type', 'tags');
  foreach($save_values as $value) {
    // set it in cstorage, the voodoo in part 3 will take care of the rest magically
    $form_state['cstorage']->$value = $submitted[$value];
  }
}

/**
 * Menu callback; Retrieve a pipe delimited string of autocomplete suggestions for existing users
 */
function dashboard_autocomplete_node($string = '') {
	$sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.title LIKE '%%%s%%'");
  $result = db_query($sql, $string);
  $references = array();
  while ($nodes = db_fetch_object($result)) {
    $references[$nodes->nid] = array(
      'title' => $nodes->title,
      'rendered' => check_plain($nodes->title),
    );
  }
  $matches = array();
  if (!empty($references)) {
    foreach ($references as $id => $row) {
      // Add a class wrapper for a few required CSS overrides.
      $matches[$row['title'] ." [nid:$id]"] = '<div class="reference-autocomplete">'. $row['rendered'] . '</div>';
    }
  }
  drupal_json($matches);
}

function dashboard_autocomplete_block($string = '') {
  $matches = array();
  foreach (module_list() as $module) {
    $module_blocks = module_invoke($module, 'block', 'list');
    if ($module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        // If it's a new block, add identifiers.
        $block['module'] = $module;
        $block['delta']  = $delta;
        // Add to the list of blocks we return.
        // TODO make this a preg
        if (strpos(strtolower($block['info']), strtolower($string)) !== FALSE) {
          $matches[$block['info'] ." [module:". $block['module'] . ' delta:' . $block['delta'] ."]"] = '<div class="reference-autocomplete">'. $block['info'] . '</div>';
        }
      }
    }
  }
  drupal_json($matches);
}

function dashboard_generator_form_selection(&$form, &$form_state) {
  $widget = &$form_state['cstorage'];
  //TODO make autocomplete and add if logic for the path
  if ($widget->widget_type == 'node') {
  	$path = 'admin/settings/dashboard/ajax/autocomplete/node';
  }
  else if ($widget->widget_type == 'block') {
  	$path = 'admin/settings/dashboard/ajax/autocomplete/block';
  }
  $form['subtype'] = array(
    '#type' => 'textfield',
    '#title' => 'Widget',
    '#required' => 1,
    '#default_value' => $widget->subtype,
    '#autocomplete_path' => $path,
  );
  $form_state['no buttons'] = TRUE;
}


/**
 * Same idea as previous steps submit
 */
function dashboard_generator_form_selection_submit(&$form, &$form_state) {
  $submitted = $form_state['values'];
  $form_state['cstorage']->subtype = $submitted['subtype'];
  //TODO build conf if necessary
  //TODO what are we going to do with defaults here
}

/*----PART 3 CTOOLS CALLBACKS -- these usually don't have to be very unique  ---------------------- */

/**
 * Callback generated when the add page process is finished.
 * this is where you'd normally save.
 */
function dashboard_generator_add_subtask_finish(&$form_state) {
  $widget = &$form_state['cstorage'];
  // Clear the cache
  if ($widget->widget_type == 'block') {
    $widget->conf = "";
    preg_match('/^(.*)\[module\:(.*)\s*delta\:(.*)\]$/', $widget->subtype, $matches);
    if (!empty($matches)) {
      list(, $title, $module, $delta) = $matches;
      $widget->subtype = trim($module) . '-' . $delta;
    }
  }
  else {
    preg_match('/^(?:\s*|(.*) )?\[\s*nid\s*:\s*(\d+)\s*\]$/', $widget->subtype, $matches);
    if (!empty($matches)) {
      list(, $title, $nid) = $matches;
      $widget->subtype = 'node-'. $nid;
      $widget->conf = serialize(array('nid' => $nid));
    }
  }
  // Not current used so set to true
  $widget->default_enabled = TRUE;

  // Set update when appropriate (id exists)
  $update = array();
  if ($widget->id) {
    $update[] = 'id';
  }

  drupal_write_record("dashboard_default", $widget, $update);
  dashboard_generator_clear_page_cache($form_state['cache name']);

  //update outdated widgets on user pages

  if(!empty($widget->id) && !empty($widget->subtype)) {
    db_query("UPDATE {dashboard_widget} SET subtype = '%s' WHERE widget_id = '%d'", $widget->subtype, $widget->id);
  }
}

/**
 * Callback for the proceed step
 *
 */
function dashboard_generator_add_subtask_next(&$form_state) {
  $dashboard = &$form_state['cstorage'];
  ctools_include('object-cache');
  $cache = ctools_object_cache_set('dashboard_generator', $form_state['cache name'], $dashboard);
}

/**
 * Callback generated when the 'cancel' button is clicked.
 *
 * All we do here is clear the cache.
 */
function dashboard_generator_add_subtask_cancel(&$form_state) {
  dashboard_generator_clear_page_cache($form_state['cache name']);
}

/*----PART 4 CTOOLS FORM STORAGE HANDLERS -- these usually don't have to be very unique  ---------------------- */

/**
 * Store changes to a task handler in the object cache.
 */
function dashboard_generator_set_page_cache($dashboard) {
  ctools_include('object-cache');
  ctools_object_cache_set('dashboard_generator', $form_state['cache name'], $dashboard);
}

/**
 * Remove an item from the object cache.
 */
function dashboard_generator_clear_page_cache($name) {
  ctools_include('object-cache');
  ctools_object_cache_clear('dashboard_generator', $name);
}

/**
 * Get the cached changes to a given task handler.
 */
function dashboard_generator_get_page_cache($name) {
  ctools_include('object-cache');
  $cache = ctools_object_cache_get('dashboard_generator', $name);
  return $cache;
}

/**
 * Menu callback - ask for confirmation of default widget deletion
 */
function dashboard_default_widget_delete_confirm(&$form_state, $widget_id) {
  $form['widget_id'] = array(
    '#type' => 'value',
    '#value' => $widget_id,
  );

  return confirm_form($form,
    // @TODO: get the widget title here instead of $widget_id
    t('Are you sure you want to delete default widget %title?', array('%title' => $widget_id)),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/settings/dashboard/widgets',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute default widget deletion
 */
function dashboard_default_widget_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $widget_id = $form_state['values']['widget_id'];
    if (is_numeric($widget_id)) {
      // Delete all widget instances
      // @TODO: widget_id will be replaced with widget_default_id
      db_query("DELETE FROM {dashboard_widget} WHERE widget_id = %d", $widget_id);
      // Delete default widget
      db_query("DELETE FROM {dashboard_default} WHERE id = %d", $widget_id);
      drupal_set_message(t('Default widget %title deleted.', array('%title' => $widget_id)));
    }
  }
  $form_state['redirect'] = 'admin/settings/dashboard/widgets';
}
