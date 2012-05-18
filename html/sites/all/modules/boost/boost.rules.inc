<?php
// $Id: boost.rules.inc,v 1.1.2.3 2009/10/28 04:07:03 mikeytown2 Exp $

/**
 * @file
 * Integration with the rules module
 */

/**
 * Implementation of hook_rules_action_info().
 *
 * @ingroup rules
 */
function boost_rules_action_info() {
  return array(
    'boost_rules_action_clear_page' => array(
      'label' => t('Clear a page from the boost cache.'),
      'arguments' => array(
        'page' => array('type' => 'string', 'label' => t('URL of page to clear')),
      ),
      'module' => 'Boost',
    ),
  );
}

/**
 * Clears a page from the Boost cache
 */
function boost_rules_action_clear_page($page, $settings) {
  if (boost_is_cached($path)) {
    boost_cache_expire_derivative(array($page));
    if (BOOST_VERBOSE >= 5) {
      watchdog('boost', 'Rules flushed !page from static page cache.', array('!page' => $page));
    }
  }
}

/**
 * Action clear page from cache configuration form.
 */
function boost_rules_action_clear_page_form($settings, &$form) {
  $settings += array('page' => '');

  $form['settings']['page'] = array(
    '#type' => 'textarea',
    '#title' => t('Page URL'),
    '#default_value' => $settings['page'],
    '#description' => t('The short URL of the page to clear from the Boost cache.'),
  );
}
