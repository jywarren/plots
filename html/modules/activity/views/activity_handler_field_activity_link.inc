<?php
// $Id: activity_handler_field_activity_link.inc,v 1.1.2.1 2009/08/25 18:34:42 sirkitree Exp $
/**
 * Field handler to present a link to the activity.
 */
class activity_handler_field_activity_link extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['aid'] = 'aid';
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['text'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    $nid = $values->{$this->aliases['aid']};
    return l($text, "activity/$aid"); // @todo: where should this lead?
  }
}
