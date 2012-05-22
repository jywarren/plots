<?php

/**
 * @file
 * Additional functions used bu OpenID 2.0 Provider.
 */

/**
 * Create an association with an RP.
 *
 * Implementation of "8.2 Association Session Response"
 *
 * @param array $request
 */
function openid_provider_association_response($request) {
  module_load_include('inc', 'openid');

  $message = _openid_create_message(_openid_provider_association_save($request));

  drupal_set_header('HTTP/1.1 200 OK');
  drupal_set_header("Content-Type: text/plain");
  drupal_set_header('Content-Length: '. strlen($message));
  drupal_set_header('Connection: close');

  print $message;
}

/**
 * Helper function to create an association.
 *
 * This generates a valid assoc_handle and records it to the
 * openid_provider_association table.
 *
 * It used used in the 8.2 association response but can be used by
 * others, as it only returns the data and doesn't directly send it to
 * the client.
 *
 * This will generate a Diffie-Hellman session or a plain hmac session,
 * based on the session_type argument in the $request array.
 *
 * @param $request array
 *    a key-value array featuring the following parameters:
 *      openid.session_type: the session type, can be empty or
 *                           "no-encryption" (equivalent) or
 *                           "DH-SHA1", "DH-SHA256"
 *      openid.assoc_type: the association to generate, one of
 *                         "HMAC-SHA1" or "HMAC-SHA256".
 * @return array a key/value pair of the response elements
 * @see openid_provider_association_response()
 */
function _openid_provider_association_save($request) {
  $session_type = '';
  if (isset($request['openid.session_type'])) {
    $session_type = $request['openid.session_type'];
  }
  $assoc_type = '';
  if (isset($request['openid.assoc_type'])) {
    $assoc_type = $request['openid.assoc_type'];
  }

  $assoc_handle = _openid_provider_nonce();
  $expires_in = variable_get('openid_provider_assoc_expires_in', '3600');

  // CLEAR STALE ASSOCIATIONS
  db_query("DELETE FROM {openid_provider_association} WHERE created + expires_in < %d", time());

  $response = array(
    'ns' => OPENID_NS_2_0,
    'session_type' => $session_type,
    'assoc_handle' => $assoc_handle,
    'assoc_type' => $assoc_type,
    'expires_in' => $expires_in
  );

  if ($session_type == 'DH-SHA1'
        || (($session_type == '' || $session_type == 'no-encryption')
            && $assoc_type == 'HMAC-SHA1')) {
    $num_bytes = 20;
    $algo = 'sha1';
  }
  elseif ($session_type == 'DH-SHA256'
        || (($session_type == '' || $session_type == 'no-encryption')
            && $assoc_type == 'HMAC-SHA256')) {
    $num_bytes = 32;
    $algo = 'sha256';
  } else {
    // Implementation of "8.2.4 Unsuccessful Response Parameters"
    $reason = '';
    if ($session_type != '' && $session_type != 'no-encryption' && $session_type != 'DH-SHA1' && $session_type != 'DH-SHA256') {
      $reason .= 'unsupported session type; ';
    }
    if ($assoc_type != 'HMAC-SHA1' && $assoc_type != 'HMAC-SHA256') {
      $reason .= 'unsupported association type; ';
    }
    _openid_provider_debug('failed association because of %reason (response dump: <pre>%response</pre>)', array('%response' => $r, '%reason' => $reason));
    return array(
      'ns' => OPENID_NS_2_0,
      'error' => $reason, // optional
      'error_code' => 'unsupported-type',
      'session_type' => 'DH-SHA256', // optional
      'assoc_type' => 'HMAC-SHA256' // optional
    );
  }
  $secret = _openid_get_bytes($num_bytes);
  if ($session_type == '' || $session_type == 'no-encryption') {
    $mac_key = base64_encode(hash_hmac($algo, $response['assoc_handle'], $secret, TRUE));
    $response['mac_key'] = $mac_key;
  }
  else {
    $dh_assoc = openid_provider_dh_assoc($request, $secret, $algo);
    $mac_key = base64_encode($secret);
    $response['dh_server_public'] = $dh_assoc['dh_server_public'];
    $response['enc_mac_key'] = $dh_assoc['enc_mac_key'];
  }
  // Save the association for reference when dealing
  // with future requests from the same RP.
  db_query("INSERT INTO {openid_provider_association} (assoc_handle, assoc_type, session_type, mac_key, created, expires_in) VALUES ('%s', '%s', '%s', '%s', %d, %d)",
          $assoc_handle, $assoc_type, $session_type, $mac_key, time(), $expires_in);
  _openid_provider_debug('recorded association (response: <pre>%response</pre>)', array('%response' => var_export($response, TRUE)));

  return $response;
}

function _openid_provider_association_load($assoc_handle) {
  return db_fetch_object(db_query("SELECT * FROM {openid_provider_association} WHERE assoc_handle = '%s'", $assoc_handle));
}

/**
 * Generate an authentication response
 *
 * Implementation of "10. Responding to Authentication Requests"
 *
 * This is the guts of this module. This function will either:
 *
 * 1. prompt the user with a login form if he's not logged in
 * 2. cancel the login if the wrong identity is provided
 * 3. display a selection form to allow the user to decide to login
 * 4. allow the login directly, without prompting the user
 *
 * This function is likely not to return unless in case #3, where it
 * will return the login form. All other cases will be redirections.
 *
 * @param array $request
 *     The request from the RP
 */
function openid_provider_authentication_response($request) {
  global $user;

  // If the user is not yet logged in, redirect to the login page before continuing.
  if (!$user->uid) {
    if ($request['openid.mode'] == 'checkid_immediate') {
      _openid_provider_debug('RP requested checkid_immediate mode, but we need to prompt the user, returning');
      return openid_redirect_http($request['openid.return_to'], openid_provider_authentication_error($request));
    }
    $_SESSION['openid_provider']['request'] = $request;
    drupal_goto('user/login', 'destination=openid/provider/continue');
  }

  // Determine the realm (openid.trust_root in 1.x)
  $realm = (empty($request['openid.realm'])) ? $request['openid.trust_root'] : $request['openid.realm'];

  // Check for a directed identity request.
  if ($request['openid.identity'] == 'http://specs.openid.net/auth/2.0/identifier_select') {
    $identity = openid_provider_url(openid_provider_user_path($user->uid));
  }
  else {
    $identity = $request['openid.identity'];
    if ($identity != openid_provider_url(openid_provider_user_path($user->uid))) {
      $response = openid_provider_authentication_error($request);
      _openid_provider_debug('claimed identity is invalid, redirecting back to %url (response dump: <pre>%response</pre>)', array('%url' => $request['openid.return_to'], '%response' => var_export($response, TRUE)));
      return openid_redirect_http($request['openid.return_to'], $response);
    }
  }

  /*
   * according to section 9.1, an empty, invalid or expired
   * assoc_handle should make the 'transaction take place in
   * "Stateless mode"', which is also detailed in section section
   * 10.1. The latter states that responses MUST have that value set
   * and that we must register it for futher use, so we generate a new
   * one if the client didn't provide any.
   */
  $assoc_handle = $request['openid.assoc_handle'];
  if (!$assoc_handle || !_openid_provider_association_load($assoc_handle)) {
    $request['openid.assoc_type'] = 'HMAC-SHA256';
    $r = _openid_provider_association_save($request);
    if ($r['error_code']) {
      return openid_redirect_http($request['openid.return_to'], $r);
    }
    // we just use the assoc_handle here, but at least it is kept and
    // can be checked later
    $assoc_handle = $r['assoc_handle'];
  }
  $response = array(
    'openid.ns' => OPENID_NS_2_0,
    'openid.mode' => 'id_res',
    'openid.op_endpoint' => openid_provider_url('openid/provider'),
    'openid.identity' => $identity,
    'openid.claimed_id' => $identity,
    'openid.return_to' => $request['openid.return_to'],
    'openid.response_nonce' => _openid_provider_nonce(),
    'openid.assoc_handle' => $assoc_handle,
    'openid.sreg.nickname' => $user->name,
    'openid.sreg.email' => $user->mail,
    'openid.ns.sreg' => 'http://openid.net/extensions/sreg/1.1'
  );

  // calling hook_openid so we can do response parsing and send any pertinent data back to the user
  $response = array_merge($response, module_invoke_all('openid_provider', 'response', $response, $request));

  $rp = _openid_provider_rp_load($user->uid, $realm);
  if ($rp->auto_release) {
    $response = _openid_provider_sign($response);
    _openid_provider_rp_save($user->uid, $realm, TRUE);
    _openid_provider_debug('automatic response authentication success using redirect to %url (request dump: <pre>%request</pre>, response dump: <pre>%response</pre>)', array('%url' => $request['openid.return_to'], '%response' => var_export($response, TRUE), '%request' => var_export($request, TRUE)));
    return openid_redirect_http($response['openid.return_to'], $response);
  }
  // Is the RP requesting Immediate or Indirect mode?
  elseif ($request['openid.mode'] == 'checkid_immediate') {
    _openid_provider_debug('RP requested checkid_immediate mode, but we need to prompt the user, returning');
    return openid_redirect_http($request['openid.return_to'], openid_provider_authentication_error($request));
  }
  else {
    // Unset global post variable, otherwise FAPI will assume it has been
    // submitted against openid_provider_form.
    unset($_POST);
    return drupal_get_form('openid_provider_form', $response, $realm);
  }
}

/**
 * Remove non OpenID element from redirection form.
 *
 * This is to fix compatibility with Janrain's Ruby library, which is especially anal about extra arguments.
 *
 * @see https://github.com/openid/ruby-openid/blob/master/lib/openid/consumer/idres.rb#L174
 * @see http://www.redmine.org/issues/8399
 */
function openid_provider_form_openid_redirect_form_alter(&$form, &$form_state) {
  unset($form['form_id']);
  unset($form['form_build_id']);
  unset($form['form_token']);
}

/**
 * Generate a error response to authentication requests
 *
 * Implementation of "10.2.  Negative Assertions"
 *
 * @param $mode string
 *     The mode of this connexion, assumed to be either
 *     'checkid_immediate' or 'checkid_setup'
 * @return $array a proper response for the request
 */
function openid_provider_authentication_error($request) {
  if ($request['openid.mode'] == 'checkid_immediate') {
    if ($request['openid.ns']) { // 2.0
      return array('openid.ns' => OPENID_NS_2_0, 'openid.mode' => 'setup_needed');
    } else { // 1.1 compatibility
      return array(
        'openid.mode' => 'id_res',
        'openid.user_setup_url' => url('user/login', array('absolute' => TRUE))
      );
    }
  }
  else { // checkid_setup
    return array(
      'openid.ns' => OPENID_NS_2_0,
      'openid.mode' => 'cancel'
    );
  }
}

/**
 * Sends an unsolicited positive assertion to the relying party.
 *
 * It is unclear what this function is for. It is not used anywhere,
 * and looks like a "10.1.  Positive Assertions", but that's what
 * openid_provider_authentication_response() does...
 *
 * This is basically a copy of openid_redirect_http() but it does a
 * full HTTP request directly from the browser.
 *
 * @todo figure out what this does or get rid of it.
 * @param $url
 *   The URL where the assertion should be sent
 * @param $response
 *   Response data
 * @return bool whether the HTTP request succeeded
 * @see drupal_http_request()
 */
function openid_provider_unsolicited_assertion($url, $response) {
  $response = _openid_provider_sign($response);
  $query = array();
  foreach ($response as $key => $value)  {
    $query[] = $key .'='. urlencode($value);
  }
  $sep = (strpos('?', $url) === FALSE) ? '&' : '?';
  $result = drupal_http_request($url . $sep . implode('&', $query), array(), 'GET');
  if ($result->code == 404) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Helper function to generate a Diffie-Hellman key exchange
 * association.
 *
 * Implementation of the Diffie-Hellman key exchange (RFC2631).
 *
 * @param $request
 *    The request as given by the RP or the UA.
 * @param $secret
 *    A random string that is secret.
 * @param $algo
 *    An algorithm valid for hash()
 * @return array
 *    An private/public keypair, public key is dh_server_public and
 *    private is enc_mac_key, as per "8.2.3.  Diffie-Hellman Response
 *    Parameters"
 */
function openid_provider_dh_assoc($request, $secret, $algo = 'sha1') {
  if (empty($request['openid.dh_consumer_public'])) {
    return FALSE;
  }

  if (isset($request['openid.dh_modulus'])) {
    $mod = _openid_dh_base64_to_long($request['openid.dh_modulus']);
  }
  else {
    $mod = OPENID_DH_DEFAULT_MOD;
  }

  if (isset($request['openid.dh_gen'])) {
    $gen = _openid_dh_base64_to_long($request['openid.dh_gen']);
  }
  else {
    $gen = OPENID_DH_DEFAULT_GEN;
  }

  $r = _openid_dh_rand($mod);
  $private = _openid_provider_add($r, 1);
  $public = _openid_provider_powmod($gen, $private, $mod);

  $cpub = _openid_dh_base64_to_long($request['openid.dh_consumer_public']);
  $shared = _openid_provider_powmod($cpub, $private, $mod);
  $mac_key = _openid_provider_dh_xorsecret($shared, $secret, $algo);
  $enc_mac_key = base64_encode($mac_key);
  $spub64 = _openid_dh_long_to_base64($public);
  return array(
    'dh_server_public' => $spub64,
    'enc_mac_key' => $enc_mac_key
    );
}

/**
 * This is copy of _opend_dh_xorsecret() but uses PHP5 hash()
 * function. Should be merged back into openid client for D8. See
 * https://drupal.org/node/1419778.
 *
 * @param long $shared
 * @param string $secret
 * @param string $algo
 * @return binary string
 */
function _openid_provider_dh_xorsecret($shared, $secret, $algo = 'sha1') {
  $dh_shared_str = _openid_dh_long_to_binary($shared);
  $sha1_dh_shared = hash($algo, $dh_shared_str, TRUE);
  $xsecret = "";
  for ($i = 0; $i < strlen($secret); $i++) {
    $xsecret .= chr(ord($secret[$i]) ^ ord($sha1_dh_shared[$i]));
  }
  return $xsecret;
}

/**
 * Respond to a verification request from the Relaying Party.
 *
 * Implementation of "11.4.2. Verifying Directly with the Identity
 * Provider"
 *  
 * @param array $request
 *     Exact copies of all fields from the authentication response
 * @return array
 *    A valid response as per "11.4.2.2. Response Parameters"
 */
function openid_provider_verification_response($request) {
  $is_valid = TRUE;

  // Use the request openid.assoc_handle to look up
  // how this message should be signed, based on
  // a previously-created association.
  $assoc = _openid_provider_association_load($request['openid.assoc_handle']);

  $signed_keys = explode(',', $request['openid.signed']);
  $signature = _openid_provider_signature($assoc, $request, $signed_keys);

  if ($signature != $request['openid.sig']) {
    $is_valid = FALSE;
  }

  if ($is_valid) {
    $response = array(
      'ns' => OPENID_NS_2_0,
      'is_valid' => 'true'
    );
  }
  else {
    $response = array(
      'ns' => OPENID_NS_2_0,
      'is_valid' => 'false',
      'invalidate_handle' => $request['openid.assoc_handle'] // optional, An association handle sent in the request
    );
  }

  $message = _openid_create_message($response);
  _openid_provider_debug('verification response, valid: %valid, handle: %handle', array('%valid' => $is_valid, '%handle' => $request['openid.assoc_handle']));
  header("Content-Type: text/plain");
  print $message;
}

/**
 * Handle situations where the user refuses to login.
 *
 * Implementation of "10.2.  Negative Assertions"
 */
function openid_provider_cancel_authentication_response($mode = 'checkid_immediate') {
  $response = array();
  if ($mode == 'checkid_immediate') {
    $response = array(
      'openid.ns' => OPENID_NS_2_0,
      'openid.mode' => 'id_res',
      'openid.user_setup_url' => url('user/login', array('absolute' => TRUE))
    );
  }
  else {
    $response = array('openid.ns' => OPENID_NS_2_0, 'openid.mode' => 'cancel');
  }
  return $response;
}

function _openid_provider_rp_load($uid, $realm = NULL) {
  if ($realm) {
    return db_fetch_object(db_query("SELECT * FROM {openid_provider_relying_party} WHERE uid=%d AND realm='%s'", $uid, $realm));
  }
  else {
    $rps = array();
    $result = db_query("SELECT * FROM {openid_provider_relying_party} WHERE uid=%d ORDER BY last_time DESC", $uid);
    while ($rp = db_fetch_object($result)) {
      $rps[] = $rp;
    }
    return $rps;
  }
}

function _openid_provider_rp_save($uid, $realm, $auto_release = FALSE) {
  $rpid = db_result(db_query("SELECT rpid FROM {openid_provider_relying_party} WHERE uid=%d AND realm='%s'", $uid, $realm));
  if ($rpid) {
    db_query("UPDATE {openid_provider_relying_party} SET auto_release=%d, last_time=%d WHERE rpid=%d", $auto_release, time(), $rpid);
  }
  else {
    db_query("INSERT INTO {openid_provider_relying_party} (uid, realm, first_time, last_time, auto_release) VALUES (%d, '%s', %d, %d, %d)", $uid, $realm, time(), time(), $auto_release);
  }
}

function _openid_provider_nonce() {
  // YYYY-MM-DDThh:mm:ssTZD UTC, plus some optional extra unique chars
  return gmstrftime('%Y-%m-%dT%H:%M:%SZ') .
    chr(mt_rand(0, 25) + 65) .
    chr(mt_rand(0, 25) + 65) .
    chr(mt_rand(0, 25) + 65) .
    chr(mt_rand(0, 25) + 65);
}

function _openid_provider_sign($response) {
  module_load_include('inc', 'openid');

  $signed_keys = array('op_endpoint', 'return_to', 'response_nonce', 'assoc_handle', 'identity', 'claimed_id', 'sreg.nickname', 'sreg.email');
  $signed_keys = array_merge($signed_keys, module_invoke_all('openid_provider', 'signed', $response));
  $response['openid.signed'] = implode(',', $signed_keys);

  // Use the request openid.assoc_handle to look up
  // how this message should be signed, based on
  // a previously-created association.
  $assoc = _openid_provider_association_load($response['openid.assoc_handle']);

  // Generate signature for this message
  $response['openid.sig'] = _openid_provider_signature($assoc, $response, $signed_keys);
  return $response;
}

/**
 * Create a signature for the provided association.
 *
 * This is copy from the openid client but uses PHP5 only hash_hmac()
 * function.
 *
 * @param object $association
 * @param array $message_array
 * @param array $keys_to_sign
 * @return string
 */
function _openid_provider_signature($association, $message_array, $keys_to_sign) {
  $signature = '';
  $sign_data = array();
  foreach ($keys_to_sign as $key) {
    if (isset($message_array['openid.'. $key])) {
      $sign_data[$key] = $message_array['openid.'. $key];
    }
  }
  $message = _openid_create_message($sign_data);
  $secret = base64_decode($association->mac_key);
  $signature = hash_hmac($association->assoc_type == 'HMAC-SHA256' ? 'sha256' : 'sha1', $message, $secret, TRUE);
  return base64_encode($signature);
}

function _openid_provider_add($a, $b) {
  return bcadd($a, $b);
}

function _openid_provider_powmod($base, $exp, $mod) {
  return bcpowmod($base, $exp, $mod);
}

/**
 * wrapper around the watchdog function
 *
 * this will log to the watchdog only if debugging is enabled
 */
function _openid_provider_debug($message, $variables = array(), $severity = WATCHDOG_DEBUG, $link = NULL) {
  if (variable_get('openid_provider_debugging', false)) {
    watchdog('openid_provider', $message, $variables, $severity, $link);
  }
}
