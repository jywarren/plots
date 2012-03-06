<?php

/**
 * @file
 * Page callbacks for OpenID Provider.
 */

/**
 * Primary endpoint callback - serves op_endpoint.
 *
 * @param array $request (Optional) request parameters.
 */
function openid_provider_endpoint($request = array()) {
  module_load_include('inc', 'openid');
  module_load_include('inc', 'openid_provider');

  if (count($request) == 0) {
    $request = _openid_response();
  }

  if (isset($request['openid.mode'])) {
    _openid_provider_debug('endpoint called in mode %mode (request dump: <pre>%request</per>)', array('%mode' => $request['openid.mode'], '%request' => var_export($request, TRUE)));
    switch ($request['openid.mode']) {
      case 'associate':
        openid_provider_association_response($request);
        return;
      case 'checkid_immediate':
      case 'checkid_setup':
        return openid_provider_authentication_response($request);
      case 'check_authentication':
        openid_provider_verification_response($request);
        break;
    }
  }
}

/**
 * Callback page for OpenID Identifier. This page is primarily used for
 * discovery when someone is logging in from a Relying Party.
 */
function openid_provider_page($account) {
  return t('This is the OpenID page for %user.', array('%user' => $account->name));
}

/**
 * Menu callback to continue authentication process after user login. This
 * callback is encountered when a user tries to login to an RP but does not yet
 * have a valid local session
 */
function openid_provider_continue() {
  module_load_include('inc', 'openid');

  if (isset($_SESSION['openid_provider']['request'])) {
    $request = $_SESSION['openid_provider']['request'];
    unset($_SESSION['openid_provider']['request']);
    return openid_provider_endpoint($request);
  }
  else {
    drupal_set_message(t('Session expired'));
    drupal_goto();
  }
}

/**
 * Page callback for processing openid_provider_form.
 */
function openid_provider_send() {
  return drupal_get_form('openid_provider_form');
}


/**
 * Page callback for OpenID Sites form where users can configure the
 * auto_release status of RP's they have logged into.
 *
 * @param object $account User account object for the user.
 */
function openid_provider_sites($account) {
  drupal_set_title(check_plain($account->name));
  return drupal_get_form('openid_provider_sites_form', $account);
}

/**
 * Form builder function for openid_provider_sites
 */
function openid_provider_sites_form($form_state, $user = NULL) {
  if (!$user) {
    global $user;
  }

  module_load_include('inc', 'openid_provider');

  $result = pager_query("SELECT * FROM {openid_provider_relying_party} WHERE uid=%d ORDER BY last_time DESC", 50, 0, NULL, $user->uid);

  $form['description'] = array(
      '#type' => 'item',
      '#description' => t('Those are the sites you have used your OpenID on. Access control determines if you will be asked for approval when login into those sites using your OpenID. You can also completely deny access to those sites if you think they are malicious.'),
  );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
  );

  $form['auto_release']['#tree'] = TRUE;
  while ($rp = db_fetch_object($result)) {
      $rps[$rp->rpid] = '';
      $form['site'][$rp->rpid] = array(
          '#value' => l($rp->realm, $rp->realm),
      );
      $form['last_access'][$rp->rpid] = array(
          '#value' => $rp->last_time,
      );
      $form['auto_release'][$rp->rpid] = array(
          '#type' => 'checkbox',
          '#default_value' => $rp->auto_release,
      );
  }

  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));
  $form['#theme'] = 'openid_provider_sites';

  return $form;
}

/**
 * Form submit callback for openid_provider_sites.
 */
function openid_provider_sites_form_submit($form, &$form_state) {
  foreach ($form_state['values']['auto_release'] as $key => $value) {
    db_query("UPDATE {openid_provider_relying_party} SET auto_release=%d WHERE rpid=%d", $value, $key);
  }
  drupal_set_message(t('Settings saved.'));
}

/**
 * Theme openid sites overview.
 *
 * @ingroup themeable
 */
function theme_openid_provider_sites($form) {
  // If there are rows in this form, then $form['title'] contains a list of
  // the title form elements.
  $header = array(t('Auto release'), t('Site'), t('Last access'));
  foreach (element_children($form['site']) as $key) {
    $row = array();
    $row[] = drupal_render($form['auto_release'][$key]);
    $row[] = drupal_render($form['site'][$key]);
    $row[] = format_date(drupal_render(($form['last_access'][$key])));
    $rows[] = $row;
  }

  unset($form['last_access']);
  $output .= theme('table', $header, $rows);
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  $output .= drupal_render($form);

  return $output;
}
