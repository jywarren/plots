<?php
// $Id: to_do_block.admin.inc,v 1.1.2.4 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * Function handling the block form of the to_do module.
 */

/**
 * Configuration for the To do block.
 */
function _to_do_block_list_settings_form() {
  $form['to_do_block_list_hide_when_empty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide block when empty'),
    '#description' => t('Check this box to hide the block completely when the currently selected list is empty.'),
    '#default_value' => variable_get('to_do_block_list_hide_when_empty', 0),
  );
  $form['to_do_block_list_title_with_teaser'] = array(
    '#type' => 'checkbox',
    '#title' => t('Teaser in title'),
    '#description' => t('Get the node teaser and insert as the title of the link to the To do item.'),
    '#default_value' => variable_get('to_do_block_list_title_with_teaser', 0),
  );
  $form['to_do_status_column_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display status column'),
    '#description' => t('Check this box to display the To do list status column.'),
    '#default_value' => variable_get('to_do_status_column_display', 1),
  );
  $form['to_do_priority_column_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display priority column'),
    '#description' => t('Check this box to display the To do list priority column.'),
    '#default_value' => variable_get('to_do_priority_column_display', 1),
  );
  $form['to_do_deadline_column_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display deadline column'),
    '#description' => t('Check this box to display the To do list deadline column.'),
    '#default_value' => variable_get('to_do_deadline_column_display', 1),
  );
  $form['to_do_display_icon'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display links'),
    '#description' => t('Check this box to get a icons: link to to complete list, link to enter a new To do item...'),
    '#default_value' => variable_get('to_do_display_icon', 1),
  );

  $last_selection = to_do_last_selection();
  $current_selection = _to_do_single_column_list();
  $current_selection[-1] = 'Last block selection (' . _to_do_single_column_list($last_selection) . ')';
  ksort($current_selection);
  $form['to_do_block_list_selection'] = array(
    '#type' => 'radios',
    '#title' => t('List to display'),
    '#description' => t('Choose what is to be displayed in the block. Users can override this selection when editing their account.'),
    '#options' => $current_selection,
    '#required' => TRUE,
    '#default_value' => variable_get('to_do_block_list_selection', TO_DO_SELECTION_LAST_SELECTION),
  );

  $help_link = variable_get('to_do_block_help_link', NULL);
  if (is_null($help_link)) {
    $help_link = TO_DO_HELP_LINK;
  }
  $form['to_do_block_help_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Help link'),
    '#description' => t('When displaying links, you can enter a help icon as well. Enter the URL here. The default is !help_link. Clear this field to avoid the help icon.',
      array('!help_link', l(TO_DO_HELP_LINK, TO_DO_HELP_LINK))),
    '#default_value' => $help_link,
  );

  return $form;
}

/**
 * Save the To do list block selection.
 */
function _to_do_block_list_settings_form_submit($edit) {
  // as we're at it, reset the Block page to 1
  //$_SESSION['to_do_page'] = 1; -- this would be helpful only to the admin...
  // instead we added a fix in the generation of the block, slow but works for everyone
  // and even when just an item is marked finished!

  // XXX -- rename all as 'to_do_block_list_<blah>'
  variable_set('to_do_block_list_hide_when_empty',   $edit['to_do_block_list_hide_when_empty']);
  variable_set('to_do_block_list_title_with_teaser', $edit['to_do_block_list_title_with_teaser']);
  variable_set('to_do_status_column_display',        $edit['to_do_status_column_display']);
  variable_set('to_do_priority_column_display',      $edit['to_do_priority_column_display']);
  variable_set('to_do_deadline_column_display',      $edit['to_do_deadline_column_display']);
  variable_set('to_do_display_icon',                 $edit['to_do_display_icon']);
  variable_set('to_do_block_list_selection',         $edit['to_do_block_list_selection']);
  variable_set('to_do_block_help_link',              $edit['to_do_block_help_link']);
}

// vim: ts=2 sw=2 et syntax=php
