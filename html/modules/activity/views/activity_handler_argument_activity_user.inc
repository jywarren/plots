<?php
// $Id: activity_handler_argument_activity_user.inc,v 1.1.2.2 2009/08/27 01:05:41 scottreynolds Exp $
/**
 * @file
 * Argument handler to set the 'activity_user' used for all the filters
 */
class activity_handler_argument_activity_user extends views_handler_argument_user_uid {
  /**
   * override pre_query and set context
   */
  function pre_query() {
    $arg = NULL;
    $position = 0;
    // Iterate through each argument and find me
    foreach ($this->view->argument as $id => $arg) {
      $arg = isset($this->view->args[$position]) ? $this->view->args[$position] : NULL;
      $position++;
      if (isset($arg) || $this->has_default_argument()) {
        if (!isset($arg)) {
          $arg = $this->get_default_argument();
        }
      }
    }
    if (isset($arg)) {
      activity_views_access_user(user_load($arg));
    }
  }
  
  /**
   * Override query to prevent anything from happening
   */
  function query() {
    
  }
}