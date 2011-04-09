<?php

class private_handler_filter_private extends views_handler_filter_boolean_operator {
  function construct() {
    parent::construct();
    $this->value_value = t('Private');
  }

  function query() {
    $this->ensure_my_table();
    $qualified_name = "$this->table_alias.$this->real_field"; 
    $this->query->add_where($this->options['group'], $qualified_name . (empty($this->value) ? " = 0 OR $qualified_name IS NULL" : ' = 1'));
  }
}
