<?php
// $Id: dashboard-page.tpl.php,v 1.3 2009/09/01 16:59:56 drumm Exp $

/**
 * @file
 * Page template file for Dashboard module.
 */
?>
<div id="dashboard" class="clear-block">
  <div class="messages warning noscript"><?php print t('Enable Javascript to customize your dashboard.') ?></div>
  <div class="nav-content-dashboard"><?php print theme('links', $tabs); ?></div>
  <div class="column grid-4 alpha"><?php print $widgets[0] ?></div>
  <div class="column grid-4"><?php print $widgets[1] ?></div>
  <div class="column grid-4 omega"><?php print $widgets[2] ?></div>
</div>
