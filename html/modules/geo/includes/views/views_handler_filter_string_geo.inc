<?php // $Id: views_handler_filter_string_geo.inc,v 1.1 2010/01/26 23:19:09 vauxia Exp $

/**
 * This handler exists for one reason only: to provide a filter group for geo 
 * data, which can be 'OR'ed.
 */

class views_handler_filter_string_geo extends views_handler_filter_string {

  function option_definition() {
    $options = parent::option_definition();
    $options['group'] = array('default' => 'geo');
    return $options;
  }
}
