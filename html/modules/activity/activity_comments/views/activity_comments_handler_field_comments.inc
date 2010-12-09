<?php
// $Id: activity_comments_handler_field_comments.inc,v 1.1.2.1 2010/01/13 05:25:29 scottreynolds Exp $
 
/**
 * Activity comments comment form field handler.
 */
class activity_comments_handler_field_comments extends views_handler_field {
  function init(&$view, $options) {
    parent::init($view, $options);
    $this->additional_fields['aid'] = array('table' => 'activity', 'field' => 'aid');
  }
  
  function query() {
    $this->add_additional_fields();
  }
  
  function render($values) {
    return drupal_get_form('activity_comments_form', $values->{$this->aliases['aid']}, $this->options['limit'], $this->options['order']);
  }
  
  function option_definition() {
    $options = parent::option_definition();

    $options['order'] = array(
      'default' => 'ASC',
      'translatable' => FALSE,
    );
    $options['limit'] = array(
      'default' => '2',
      'translatable' => FALSE,
    );

    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['order'] = array(
      '#type' => 'select',
      '#title' => t('Order comments by'),
      '#default_value' => $this->options['order'],
      '#options' => array(
        'ASC' => t('Created time - ascending'),
        'DESC' => t('Created time - descending')
      ),
    );
    $form['limit'] = array(
      '#type' => 'textfield',
      '#title' => t('Comments number limit'),
      '#size' => 5,      
      '#default_value' => isset($this->options['limit']) ? $this->options['limit'] : 2,
    );
  }
}