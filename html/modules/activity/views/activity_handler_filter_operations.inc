<?php
// $Id: activity_handler_filter_operations.inc,v 1.1.2.1 2009/09/20 00:12:34 scottreynolds Exp $
/**
 * Filter by Activity Operation
 */
class activity_handler_filter_operations extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = ('Activity Operation');
      foreach (activity_get_module_info() as $info) {
        foreach ($info->hooks as $hook => $ops) {
	  foreach ($ops as $op) {
	    $options[$op][] = $hook;
	  }
	}
      }
      foreach ($options as $op => $hooks) {
	$this->value_options[$op] = t('@op used in: @hooks', array('@op' => $op, '@hooks' => implode(', ', $hooks)));
      }
    }
  }
}