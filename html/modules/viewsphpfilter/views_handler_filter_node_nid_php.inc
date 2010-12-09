<?php
//$Id: views_handler_filter_node_nid_php.inc,v 1.1 2009/05/27 17:53:23 gnassar Exp $

class views_handler_filter_node_nid_php extends views_handler_filter {

  function admin_summary() {return '';}

  function can_expose() {return FALSE;}

  function operator_options() {
    return array(
      'OR' => t('OR'),
      'NOR' => t('NOR'),
    );
  }
  
  function option_definition() { 
    $options = parent::option_definition();
    $options['operator']['default'] = 'OR';
    $options['value']['default'] = '';
    $options['handler']['default'] = 'php';
    return ($options);
  }

  function value_form(&$form, &$form_state) {
    $form['value'] = array(
      '#title' => t('PHP (or IDs)'),
      '#type' => 'textarea',
      '#default_value' => $this->value,
    );
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['handler'] = array(
      '#type' => 'radios',
      '#title' => t('Handler'),
      '#options' => array('php' => t('PHP code'), 'nid' => t('Node ID list')),
      '#default_value' => $this->options['handler'],
    );
  }

  function query() {
    //to do: validation should be factored out
    $rawcode = $this->value;
    //error_log('php: ' . print_r($rawcode,TRUE));
    if ($this->options['handler'] == 'php') {
      $result = eval ($rawcode);
      //error_log('eval: ' . print_r($result,TRUE));
    } else $result = $rawcode;
    if (!is_array($result)) {
      $result = explode(',',$result);
    }
    foreach ($result as $i => $val) $result[$i] = trim($val, " \t\n\r\0\x0B,");
    //error_log('result: ' . print_r($result, TRUE));
    $allints = true;
    foreach ($result as $retval) {
      if ( ((string)((int)$retval)) != $retval ) {
        $allints = false;
        //error_log('Views PHP Filter check failure: ' . print_r($retval, TRUE) . " | " . (int)$retval . " | " . (string)((int)$retval) );
      }
    }
    if (!empty($result) and $allints) {
      //error_log('everything passed!');
      //error_log('result again: ' . print_r($result, TRUE));
      $this->ensure_my_table();
      if ($this->operator == 'OR') {
        $this->query->add_where($this->options['group'], "node.nid IN (%s)", implode(",", $result));
      }
      else if ($this->operator == 'NOR') {
        $this->query->add_where($this->options['group'], "node.nid NOT IN (%s)", implode(",", $result));
      }
    }
    else {
      //should print out a user warning on form validate if fails
    }
  }
}

// from old version
/*      if ($result === FALSE) {
        // PHP parse error; return error to user
        trigger_error('error in Node: ID PHP filter; PHP invalid, filter will be ignored', E_USER_WARNING);
        break;
      }
      else if ($result === NULL) {
        // Nothing was returned; return error to user
        trigger_error('error in Node: ID PHP filter; nothing returned, filter will be ignored', E_USER_WARNING);
        break;
      } */
