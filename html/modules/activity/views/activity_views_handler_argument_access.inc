<?php
// $Id: activity_views_handler_argument_access.inc,v 1.1.2.1 2010/06/01 04:41:17 scottreynolds Exp $

/**
 * @file
 * Argument handler for access control.
 */
class activity_views_handler_argument_access extends views_handler_argument {
  function option_definition() {
    $options = parent::option_definition();

    $options['realm'] = array('default' => '');
    return $options;
  }

  /**
   * Override the options_form().
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $info = activity_get_module_info($this->definition['module']);
    $realms = $info->realms;
    $options = array();
    foreach ($realms as $realm => $title) {
      $options[$realm] = t($title);
    }
    $form['realm'] = array(
      '#type' => 'radios',
      '#title' => t('Specific Relationship'),
      '#options' => $options,
      '#default_value' => $this->options['realm'],
      '#required' => TRUE,
    );
  }

  function query() {
    $this->ensure_my_table();
    $realm = $this->options['realm'];
    $this->query->add_where(NULL, "($this->table_alias.realm='%s' AND $this->table_alias.$this->real_field = %d)", $this->options['realm'], $this->argument); 
  }
}