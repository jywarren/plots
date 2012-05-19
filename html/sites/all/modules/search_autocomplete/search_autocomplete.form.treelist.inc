<?php

/**
 * @file
 * Search Autocomplete
 * Display the list of form to autocomplete and themis it as a draggable table.
 *
 * @authors
 * Miroslav Talenberg (Dominique CLAUSE) <http://www.axiomcafe.fr/contact>
 *
 * Sponsored by:
 * www.axiomcafe.fr
 */

//-------------------------------------------------------------------------------------
/**
 * Menu Callback: create the form to list the searchforms
 * @return     the form
 */
function search_autocomplete_treelist_form($form_state) {
  $base = "admin/settings/search_autocomplete";  // base URL for this module configurations

  $data = _search_autocomplete_get_items();  // get all forms ordered as a tree
  $form = array();

  $form['my_items'] = array();
  $form['my_items']['#tree'] = TRUE;

  // for each items to render in the form
  foreach ($data AS $values) {

    $id = $values['fid'];
    $title = $values['title'];
    $weight = $values['weight'];
    $enabled = $values['enabled'];

    $form['my_items'][$id] = array(       // element for this item
      'title' => array(                      // -> human readeable title
        '#type' => 'markup',
        '#value' => $title,
      ),
      'enabled' => array(                    // -> defines if the autocompletion is enabled for this item
        '#type' => 'checkbox',
        '#default_value' => $enabled,
      ),
      'operations' => array(
        'configure' => array(
          '#value' => l(t('configure'), "$base/$id/configure"),
        ),
        'delete' => array(
          '#value' => l(t('delete'), "$base/$id/delete"),
        ),
      ),
      'weight' => array(                     // -> weight of the item in hierarchy
        '#type' => 'weight',
        '#delta' => count($data),
        '#default_value' => $weight,
      ),
      'fid' => array(                        // -> the individual id if the item
        '#type' => 'hidden',
        '#value' => $id,
      ),
      'parent_fid' => array(                 // -> id of the parent item in hierarchy
        '#type' => 'textfield',
        '#default_value' => $values['parent_fid']
      ),
      '#depth' => $values['depth'],          // -> depth of the item
    );
  }

  // submit buton
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
} // function search_autocomplete_treelist_form()

//-------------------------------------------------------------------------------------
/**
 * Implementation of hook_submit().
 * Save the changes in the database
 */
function search_autocomplete_treelist_form_submit($form, &$form_state) {
  //Update the database with the new values
  foreach ($form_state['values']['my_items'] as $item) {
    db_query('UPDATE {search_autocomplete_forms} SET weight = %d, parent_fid = %d, enabled = %d WHERE fid = %d',
    array($item['weight'], $item['parent_fid'], $item['enabled'], $item['fid']));
  }
} //function search_autocomplete_treelist_form_submit()

//-------------------------------------------------------------------------------------
/**
 * CALLBACK:
 * Theme function for this treelist form
 */
function theme_search_autocomplete_treelist_form($form) {

  drupal_add_tabledrag('my-draggable-table', 'order', 'sibling', 'weight-group');
  drupal_add_tabledrag('my-draggable-table', 'match', 'parent', 'parent-group', 'parent-group', 'id-group');

  $header[] = t('Form name');
  $header[] = t('Enabled');
  $header[] = array(
    'data' => t('Operations'),
    'colspan' => '2',
  );
  $header[] = t('Weight');

  // for each elements to anchor in the form
  foreach (element_children($form['my_items']) as $key) {

    $element = &$form['my_items'][$key];
    $element['weight']['#attributes']['class'] = 'weight-group';
    $element['fid']['#attributes']['class'] = 'id-group';
    $element['parent_fid']['#attributes']['class'] = 'parent-group';

    $row = array();
    $row[] = theme('indentation', $element['#depth']) . drupal_render($element['title']);
    $row[] = drupal_render($element['enabled']);
    $row[] = drupal_render($element['operations']['configure']);
    $row[] = drupal_render($element['operations']['delete']);
    $row[] = drupal_render($element['weight']) .
              drupal_render($element['fid']) .
              drupal_render($element['parent_fid']);

    //Add a draggable class to every table row (<tr>)
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }

  // Themize the table and render the form
  $output = theme('table', $header, $rows, array('id' => 'my-draggable-table'));
  $output .= drupal_render($form);

  return $output;
} //function theme_search_autocomplete_treelist_form()



/////////////////////////////////////////////////////////////////////////////////////////
////                             HELPER FUNCTIONS                                    ////

//-------------------------------------------------------------------------------------
/**
 * Helper function: get the forms from database and render them hierarchically
 * return the items sorted
 */
function _search_autocomplete_get_items() {
  $items = array();
  // get data in database and fetch it
  $result = db_query('SELECT * FROM {search_autocomplete_forms} ORDER BY weight');
  while ($item = db_fetch_array($result)) {
    $items[] = $item;
  }
  // order the list
  return _search_autocomplete_get_ordered_list(0, $items);
} //function _search_autocomplete_get_items()

//-------------------------------------------------------------------------------------
/**
 * HELPER:
 * Returns a tree list of all items in the $items array that are children
 * of the supplied parent, ordered appropriately
 * @return the ordered tree
 */
function _search_autocomplete_get_ordered_list($parent, $items, $depth = 0) {

  $remnant = array(); $children = array();
  // Insert direct children in $children
  // and remaining in $remnant
  foreach ($items as $item) {
    if ($item['parent_fid'] == $parent) {
      $item['depth'] = $depth;
      $children[] = $item;
    }
    else
      $remnant[] = $item;
  }

  // Sort the direct children by weight
  usort($children, '_search_autocomplete_sort_by_weight');

  $ancestors = array();

  foreach ($children as $child) {
    // Order the child ancestors iteratively
    $child_children = _search_autocomplete_get_ordered_list($child['fid'], $remnant, $depth + 1);
    // Push the results into the main array below the child
    $ancestors[] = $child;
    // Merge it if needed
    if (count($child_children)) {
      $ancestors= array_merge($ancestors, $child_children);
    }
  }
  return $ancestors;
}

//-------------------------------------------------------------------------------------
/**
 * HELPER:
 * Usort function for sorting arrays by weight
 * @return   1: if ($a < $b),
 *      0 if equal,
 *      -1 otherwise
 */
function _search_autocomplete_sort_by_weight($a, $b) {
  if ($a['weight'] == $b['weight'])
    return 0;
  return ($a['weight'] < $b['weight']) ? -1 : 1;
}

