<?php
// $Id: to_do_views_handler_field_title.inc,v 1.1.2.1 2010/10/01 21:14:42 alexiswilke Exp $
/**
 * @file
 * Contains the handling of the to_do title.
 */

/**
 * to_do title handling with a <del> tag when canceled or finished.
 */
class to_do_views_handler_field_title extends views_handler_field_node {

  function init(&$view, $options) {
    parent::init($view, $options);
    if (!empty($this->options['link_to_node'])) {
      $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
      if (module_exists('translation')) {
        $this->additional_fields['language'] = array('table' => 'node', 'field' => 'language');
      }
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_node'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Provide link to node option
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_node'] = array(
      '#title' => t('Link this field to its node'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_node']),
    );
  }

  /**
   * Render whatever the data is as a link to the node.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_node']) && $data !== NULL && $data !== '') {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "node/" . $values->{$this->aliases['nid']};
      if (isset($this->aliases['language'])) {
        $languages = language_list();
        if (isset($languages[$values->{$this->aliases['language']}])) {
          $this->options['alter']['language'] = $languages[$values->{$this->aliases['language']}];
        }
        else {
          unset($this->options['alter']['language']);
        }
      }
    }
    return $data;
  }

  function render($values) {
    $link = $this->render_link(check_plain($values->{$this->field_alias}), $values);
    $node = node_load(array('nid' => $values->nid /*{this->aliases['nid']}*/ ));
    $class = '';
    $ins = FALSE;
    $del = FALSE;
    switch ($node->item_status) {
    case TO_DO_STATUS_NOT_STARTED:
      $class .= 'to-do-not-started';
      $ins = TRUE;
      break;

    case TO_DO_STATUS_STARTED:
      $class .= 'to-do-started';
      break;

    case TO_DO_STATUS_FINISHED:
      $class .= 'to-do-finished';
      $del = TRUE;
      break;

    case TO_DO_STATUS_AWAITING_MORE_INFO:
      $class .= 'to-do-awaiting-more-info';
      break;

    case TO_DO_STATUS_DELAYED:
      $class .= 'to-do-delayed';
      break;

    case TO_DO_STATUS_CANCELED:
      $class .= 'to-do-canceled';
      $del = TRUE;
      break;

    case TO_DO_STATUS_IN_PROGRESS:
      $class .= 'to-do-in-progress';
      break;

    }
    $output = '<span class="' . trim($class) . '">';
    if ($del) {
      $output .= '<del>';
    }
    elseif ($ins) {
      $output .= '<ins>';
    }
    $output .= $link;
    if ($del) {
      $output .= '</del>';
    }
    elseif ($ins) {
      $output .= '</ins>';
    }
    $output .= '</span>';
    return $output;
  }
}

// vim: ts=2 sw=2 et syntax=php
