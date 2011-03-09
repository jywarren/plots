<?php

/**
 * @file
 * Administration pages and code for the Google Groups module.
 */

/**
 * Lists groups entered in into the site.
 *
 * @return
 *  Page HTML.
 */
function google_groups_list_page() {
  $header = array(t('Group name'), t('Account form'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array();
  $settings = variable_get('google_groups_settings', array());

  foreach ($settings['groups'] as $delta => $group) {
    if ($group['register']) {
      $reg = '<li>' . t('List on form: Yes') . '</li>';
      $reg .= '<li>' . t('Default: ') . ($group['register_default'] ? t('On') : t('Off')) . '</li>';
      $reg .= '<li>' . t('Forced: ') . ($group['register_default_forced'] ? t('Yes') : t('No')) . '</li>';
    } else {
      $reg = '<li>' . t('List on form: No') . '</li>';
    }
    $reg = '<ul>' . $reg . '</ul>';
    $row = array(check_plain($group['name']), $reg);

    // Set the edit column.
    $row[] = array('data' => l(t('edit'), 'admin/settings/googlegroups/edit/' . $delta));
    // Set the delete column.
    $row[] = array('data' => l(t('delete'), 'admin/settings/googlegroups/delete/' . $delta));

    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No Google Groups have been created.'), 'colspan' => '4', 'class' => 'message'));
  }

  return theme('table', $header, $rows);
}

/**
 * Administration form.
 *
 * @return
 *  FAPI array.
 */
function google_groups_add_form() {
  $delta = (arg(3) === 'edit' && arg(4)) ? arg(4) : NULL;
  if ($delta) {
    $settings = variable_get('google_groups_settings', array());
    $group    = $settings['groups'][$delta];
  }

  $form['group_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Group Name'),
    '#default_value' => $delta ? $group['name'] : '',
    '#required' => TRUE,
    '#size' => 30,
    '#maxlength' => 100,
    '#description' => t("Your Google Group's human readable name."),
  );
  $form['group_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Group Email'),
    '#default_value' => ($delta ? $group['id'] : ''),
    '#required' => TRUE,
    '#size' => 30,
    '#maxlength' => 64,
    '#description' => t("Your Google Group's email address."),
    '#field_suffix' => '<span class="google-groups-email-domain">@googlegroups.com</span>',
  );
  $form['register'] = array(
    '#type' => 'fieldset',
    '#title' => t('Registration Form'),
    '#description' => t('Options for subscribing via the registration form.'),
  );
  $form['register']['register'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow subscription during registration'),
    '#default_value' => ($delta ? $group['register'] : ''),
    '#description' => t('This will add a subscription checkbox to the registration form.'),
  );
  $form['register']['default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Checked by default'),
    '#default_value' => ($delta ? $group['register_default'] : ''),
    '#description' => t('If checked, the subscription checkbox on the registration form will be checked by default.'),
  );
  $form['register']['forced'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force default'),
    '#default_value' => ($delta ? $group['register_default_forced'] : ''),
    '#description' => t('If checked, the subscription checkbox on the registration form will be disabled. User will not be able to change it from its default setting. Useful to force user subscription.'),
  );
  $form['delta'] = array(
    '#type' => 'value',
    '#value' => $delta,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save group'),
  );

  $form['#submit'][] = 'google_groups_add_form_submit';

  return $form;
}

function google_groups_add_form_submit($form, &$form_state) {
  $settings = variable_get('google_groups_settings', array());
  $groups   = &$settings['groups'];
  $values   = &$form_state['values'];
  $delta    = check_plain($values['delta']);
  $group    = array();

  $group['name'] = check_plain($values['group_name']);
  $group['id'] = check_plain($values['group_id']);
  $group['register'] = (int)$values['register'];
  $group['register_default'] = (int)$values['default'];
  $group['register_default_forced'] = (int)$values['forced'];

  if ($delta) {
    $settings['groups'][$delta] = array_merge($settings['groups'][$delta], $group);
  }
  else {
    $delta = &$settings['next_delta'];
    $settings['groups']['gg' . $delta] = $group;
    $delta++;
  }

  variable_set('google_groups_settings', $settings);
  drupal_set_message(t('Google Group saved.'), 'status');
  $form_state['redirect'] = 'admin/settings/googlegroups';
  return;
}

/**
 * Menu callback; delete a single Google Group.
 */
function google_groups_delete_confirm(&$form_state) {
  $settings = variable_get('google_groups_settings', array());
  $delta = check_plain(arg(4));
  $form['delta'] = array(
    '#type' => 'value',
    '#value' => $delta,
  );

  $message = t('Are you sure you want to delete the Google Group @group?', array('@group' => $settings['groups'][$delta]['name']));
  $caption = '<p>'. t('This action cannot be undone.') .'</p>';

  return confirm_form($form, $message, 'admin/settings/googlegroups', $caption, t('Delete'));
}

/**
 * Process Google Group delete confirm submissions.
 */
function google_groups_delete_confirm_submit($form, &$form_state) {
  $settings = variable_get('google_groups_settings', array());

  drupal_set_message(t('The Google Group %name has been deleted.', array('%name' => $settings['groups'][$form_state['delta']]['name'])));
  unset($settings['groups'][$form_state['values']['delta']]);
  variable_set('google_groups_settings', $settings);

  $form_state['redirect'] = 'admin/settings/googlegroups';
  return;
}

/**
 * Implementation of hook_user() register op.
 */
function google_groups_user_register_form() {
  $form = array();

  $visable = 0;
  $settings = variable_get('google_groups_settings', array());
  foreach ($settings['groups'] as $delta => $group) {
    if ($group['register']) {
      $form['google_groups_subscribe_' . $delta] = array(
        '#type' => 'checkbox',
        '#title' => t('Subscribe to @group', array('@group' => $group['name'])),
        '#weight' => 10,
      );

      if ($group['register_default']) {
        $form['google_groups_subscribe_' . $delta]['#default_value'] = 1;
      }

      if ($group['register_default_forced']) {
        $form['google_groups_subscribe_' . $delta]['#type'] = 'hidden';
      }
      else {
        $visable++;
      }
    }
  }

  if ($visable) {
    $fieldset = array(
      '#type' => 'fieldset',
      '#title' => t('Email lists'),
      '#description' => t('Subscribe to our email list(s).'),
    );
    $form = array('google_groups' => array_merge($fieldset, $form));
  }

  return $form;
}

/**
 * Implementation of hook_user() insert op.
 */
function google_groups_user_register_submit(&$edit) {
  $settings = variable_get('google_groups_settings', array());

  foreach ($settings['groups'] as $delta => $group) {
    if ($group['register']) {
      if ($edit['google_groups_subscribe_' . $delta] == 1) {
        if (google_groups_mail($group['id'], $edit['mail'])) {
          drupal_set_message('You will be receiving a confirmation email for our email list.', 'status');
        }
        else {
          $message = 'An error sending a subscription email occurred and user was NOT subscribed to our email list.';
          watchdog('error', $message, array(), WATCHDOG_WARNING);
          drupal_set_message('An error occurred and you were NOT subscribed to our email list.', 'warning');
        }
      }
      $edit['google_groups_subscribe_' . $delta] = NULL;
    }
  }
}
