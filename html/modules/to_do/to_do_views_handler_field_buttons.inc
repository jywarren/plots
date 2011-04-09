<?php
// $Id: to_do_views_handler_field_buttons.inc,v 1.1.2.4 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * Views handler for the buttons so one can show the different to_do buttons
 * in a view.
 */

class to_do_views_handler_field_buttons extends to_do_views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['nid'] = 'nid';
  }

  function render_name($data, $values) {
    $nid = $values->{$this->aliases['nid']};
    switch ($data) {
    case TO_DO_STATUS_NOT_STARTED:
      $action_buttons = drupal_get_form('to_do_start_button_form', $nid);
      break;

    case TO_DO_STATUS_FINISHED:
    case TO_DO_STATUS_CANCELED:
      $action_buttons = '';
      break;

    default:
      $action_buttons = drupal_get_form('to_do_finished_button_form', $nid);
      break;

    }
    return $action_buttons;
  }

  function render($values) {
    $status = $values->{$this->field_alias};
    return $this->render_link($this->render_name($status, $values), $values);
  }
};

// vim: ts=2 sw=2 et syntax=php
