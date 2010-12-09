<?php
// $Id: activity_handler_field_activity_link_delete.inc,v 1.1.2.3 2009/12/09 17:18:17 scottreynolds Exp $
/**
 * Field handler to present a link to delete an activity.
 */
class activity_handler_field_activity_link_delete extends activity_handler_field_activity_link {
  function construct() {
    parent::construct();
    $this->additional_fields['aid'] = 'aid';
  }

  function render($values) {
    if (activity_delete_access($values->{$this->aliases['aid']})) {
      $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
      return l($text, "activity/" . $values->{$this->aliases['aid']} . "/delete", array('query' => drupal_get_destination()));
    }
    else {
      return;
    }
  }
}

