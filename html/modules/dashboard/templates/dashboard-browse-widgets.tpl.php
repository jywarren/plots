<?php 
  print $widget_filter;
  foreach ($widgets as $widget):
    print theme('dashboard_display_widget', $widget); 
  endforeach; 
?>