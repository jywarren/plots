<?php
// $Id: to_do_views_handler_field_priority.inc,v 1.1.2.2 2010/08/04 09:17:36 alexiswilke Exp $

/**
 * @file
 * Handle the field priority by ID or name.
 */

class to_do_views_handler_field_priority extends to_do_views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['to_do_numeric_priority'] = array('default' => FALSE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['to_do_numeric_priority'] = array(
      '#title' => t('Output priority number'),
      '#description' => t('Display the priority as a number instead of a word.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['to_do_numeric_priority']),
    );
  }

  function render_name($data, $values) {
    if (!empty($this->options['to_do_numeric_priority'])) {
      return check_plain($data);
    }
    return check_plain(_to_do_priority_list($data));
  }

  function render($values) {
    $priority = $values->{$this->field_alias};
    return $this->render_link($this->render_name($priority, $values), $values);
  }
};

// vim: ts=2 sw=2 et syntax=php
