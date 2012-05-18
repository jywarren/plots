<?php
// $Id: xrds_simple.api.php,v 1.1.2.2 2010/02/14 16:10:08 walkah Exp $

/**
 * @file
 * This is a preliminary module to support the XRDS-Simple spec.
 *
 * The draft spec can be found here:
 *   http://xrds-simple.net/core/1.0/
 */

/**
 * Provides all XRDS implementations.
 */
function hook_xrds($account = NULL) {
  $xrds['example'] = array(
    'services' => array(
      array('priority' => 10,
        'data' => array(
          'Type' => array('http://specs.openid.net/auth/2.0/signon'),
          'URI' => array('http://example.com/server'),
        ),
      ),
    ),
  );

  return $xrds;
}
