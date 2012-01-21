<?php
// $Id: maintenance-page.tpl.php,v 1.17 2009/11/04 20:49:23 johnalbin Exp $

/**
 * @file maintenance-page.tpl.php
 *
 * Theme implementation to display a single Drupal page while off-line.
 *
 * All the available variables are mirrored in page.tpl.php. Some may be left
 * blank but they are provided for consistency.
 *
 *
 * @see template_preprocess()
 * @see template_preprocess_maintenance_page()
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>
<body class="<?php print $classes; ?>">

<style>
html { 
	font-family: georgia, serif; 
	background:#222;
	color:#eee;
}
body { width: 600px; }
</style>
<h1>This is what the web could look like under the Stop Online Piracy Act</h1>
<p>Keep the web open. <a href="http://wfc2.wiredforchange.org/o/9043/p/dia/action/public/action_KEY=8173">Contact your representatives</a> or <a href="http://sopablackout.org/learnmore">find out more</a></p> 

<br />

<p>(The Public Laboratory website will be inaccessble until midnight Jan 18th.)</p>
</body>
</html>
