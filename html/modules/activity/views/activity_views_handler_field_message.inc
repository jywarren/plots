<?php
// $Id: activity_views_handler_field_message.inc,v 1.1.2.6 2009/08/02 15:11:53 sirkitree Exp $
/**
 * @file: renders the message field for the given user
 */
class activity_views_handler_field_message extends views_handler_field {
  
  /**
   * override the query to add in our stuff
   */
  function query() {
    $this->ensure_my_table();
    
    // join in the personal message table
    $this->personal_msg_table = $this->query->ensure_table('activity_personal_messages', $this->relationship);
    
    // use the personal message if there is one otherwise use the non-personal message
    $this->field_alias = $this->query->add_field(NULL, "COALESCE($this->personal_msg_table.message, $this->table_alias.$this->real_field)", $this->table_alias . '_' . $this->real_field);
  }

  function render($values) {
    // we do this so that an admin can enter html markup within the template
    return filter_xss_admin($values->{$this->field_alias});
  }
}