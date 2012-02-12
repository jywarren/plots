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
<h1>The Public Laboratory website is temporarily unavailable</h1>
<p>Please wait a moment and try again. If the issue persists, contact <a href="mailto:team@publiclaboratory.org">team@publiclaboratory.org</a>.</p>

<br />

</body>
</html>
