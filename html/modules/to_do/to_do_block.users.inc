<?php
// $Id: to_do_block.users.inc,v 1.1.2.4 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * Management of the user information in regard to the block.
 */

/**
 * Actual implementation of hook_user().
 */
function _to_do_block_user($op, $edit, $account) {
  switch ($op) {
  case 'form':
    if (user_access('can be assigned to dos', $account)) {
      $user_preferences = _to_do_user_preferences($account->uid);

      $form['to_do_fieldset'] = array(
        '#type' => 'fieldset',
        '#title' => t('To do list'),
      );
      $form['to_do_fieldset']['to_do_items_to_display'] = array(
        '#type' => 'textfield',
        '#title' => t('Number of items to display'),
        '#maxlength' => 2,
        '#size' => 3,
        '#description' => t('One to thirty items can be displayed in your To do block.'),
        '#default_value' => $user_preferences['sidebar_items'],
      );
      $form['to_do_fieldset']['to_do_low_priority_items_display'] = array(
        '#type' => 'checkbox',
        '#title' => t('Include low priority items'),
        '#description' => t('Check this box to see To do items with the Low priority in your To do block.'),
        '#default_value' => $user_preferences['low_priority_items_display'],
      );
    }
    return $form;

  case 'validate':
    // IMPORTANT: New accounts do not have the account parameter set.
    //            Since a role is required to get the to_do_items_to_display
    //            we simply ignore such entries.
    if (is_object($account) && count($account->roles) > 0
        && user_access('can be assigned to dos', $account)
        && $edit['op'] == 'Save'
        && (empty($edit['to_do_items_to_display'])
            || $edit['to_do_items_to_display'] <= 0
            || $edit['to_do_items_to_display'] > 30)) {
      form_set_error(
        'to_do_items_to_display',
        t('Please choose a number of items to display between one and thirty.'));
    }
    break;

  case 'update':
    if (user_access('can be assigned to dos', $account)) {
      $sql = "UPDATE {to_do_block_user_preferences}"
          . " SET sidebar_items = %d, low_priority_items_display = %d"
        . " WHERE uid = %d";
      $updated = db_query($sql, $edit['to_do_items_to_display'],
        $edit['to_do_low_priority_items_display'], $account->uid);
      if (db_affected_rows($updated) == 0) {
        $sql = "INSERT INTO {to_do_block_user_preferences} (uid, sidebar_items, low_priority_items_display)"
          . " VALUES (%d, %d, %d)";
        db_query($sql, $account->uid, $edit['to_do_items_to_display'],
          $edit['to_do_low_priority_items_display']);
      }
    }
    break;

  case 'delete':
    db_query('DELETE FROM {to_do_block_user_preferences} WHERE uid = %d', $account->uid);
    break;

  }
}

// vim: ts=2 sw=2 et syntax=php
