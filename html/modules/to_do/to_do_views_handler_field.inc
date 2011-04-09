<?php
// $Id: to_do_views_handler_field.inc,v 1.1.2.2 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * Views handler field.
 */

class to_do_views_handler_field extends views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
  }

  function render_link($data, $values) {
    return $data;
  }
}

// vim: ts=2 sw=2 et syntax=php
