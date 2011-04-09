<?php
// $Id: dashboard-widget.tpl.php,v 1.2 2009/09/09 06:09:08 drumm Exp $

/**
 * @file
 * Widget template file for Dashboard module.
 */
?>

<div id="widget-<?php print $widget->widget_id ?>" class="widget grid-4 alpha omega">
  <div class="tools"><a href="#" class="remove-widget" title="<?php print t('Remove') ?>"></a></div>
  <h2><?php print $widget->subject; ?></h2>
  <div class="content">
    <?php print $widget->content; ?>
  </div>
</div>
