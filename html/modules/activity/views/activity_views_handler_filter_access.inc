<?php
// $Id: activity_views_handler_filter_access.inc,v 1.1.2.8 2010/03/31 05:11:20 scottreynolds Exp $

/**
 * @file
 * generic handler that provides access control on where
 */
class activity_views_handler_filter_access extends views_handler_filter {
  
  /**
   * tell Views UI this filter can't be exposed
   */
  function can_expose() {
    return FALSE;
  }
  
  /**
   * Override the options_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    
    $info = activity_get_module_info($this->definition['module']);
    $realms = $info->realms;
    foreach ($realms as $realm => $name) {
      $default_realms[$realm] = $realm;
    }

    $options['realms'] = array('default' => $default_realms);
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
    $form['realms'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Specific Relationships'),
      '#options' => $options,
      '#default_value' => $this->options['realms'],
    );
  }
  
  /**
   * override query
   */
  function query() {
    $this->ensure_my_table();
    $group = activity_views_group();
    $placeholder = '%d';
    
    // ensure the file is included for this module
    activity_get_module_info($this->definition['module']);
    
    // go through each grant that this user has
    $realms = module_invoke($this->definition['module'], 'activity_access_grants', activity_views_access_user());

    // All the valid realms according to the options form.
    $valid_realms = array_intersect(array_keys($this->options['realms']), array_keys($realms));

    if (empty($valid_realms)) {
      // The user hasn't done anything that $this->definition['module'] realms
      // keep track of, use a dummy realm.
      $realms = array(
        'activity_none' => array(-1)
      );
      $valid_realms = array('activity_none');
    }

    foreach ($valid_realms as $realm) {
      if (is_null($group)) {
        // Static cache the where group for other access handlers to use.
        $group = activity_views_group($this->query->set_where_group('OR'));
      }
      $values = $realms[$realm];

      $replace = array_fill(0, sizeof($values), $placeholder);
      $in = ' (' . implode(", ", $replace) . ')';

      // Use the group to keep all access handlers in the same OR query.
      $this->query->add_where($group, "($this->table_alias.realm = '%s' AND $this->table_alias.$this->real_field IN $in)", array_merge(array($realm), $values));
    }
  }
}
