<?php
// $Id: page.tpl.php,v 1.26.2.3 2010/06/26 15:36:04 johnalbin Exp $

/**
 * @file
 * Theme implementation to display a single Drupal page.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $css: An array of CSS files for the current page.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/garland.
 * - $is_front: TRUE if the current page is the front page. Used to toggle the mission statement.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Page metadata:
 * - $language: (object) The language the site is being displayed in.
 *   $language->language contains its textual representation.
 *   $language->dir contains the language direction. It will either be 'ltr' or 'rtl'.
 * - $head_title: A modified version of the page title, for use in the TITLE tag.
 * - $head: Markup for the HEAD section (including meta tags, keyword tags, and
 *   so on).
 * - $styles: Style tags necessary to import all CSS files for the page.
 * - $scripts: Script tags necessary to load the JavaScript files and settings
 *   for the page.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It should be placed within the <body> tag. When selecting through CSS
 *   it's recommended that you use the body tag, e.g., "body.front". It can be
 *   manipulated through the variable $classes_array from preprocess functions.
 *   The default values can be one or more of the following:
 *   - front: Page is the home page.
 *   - not-front: Page is not the home page.
 *   - logged-in: The current viewer is logged in.
 *   - not-logged-in: The current viewer is not logged in.
 *   - node-type-[node type]: When viewing a single node, the type of that node.
 *     For example, if the node is a "Blog entry" it would result in "node-type-blog".
 *     Note that the machine name will often be in a short form of the human readable label.
 *   - page-views: Page content is generated from Views. Note: a Views block
 *     will not cause this class to appear.
 *   - page-panels: Page content is generated from Panels. Note: a Panels block
 *     will not cause this class to appear.
 *   The following only apply with the default 'sidebar_first' and 'sidebar_second' block regions:
 *     - two-sidebars: When both sidebars have content.
 *     - no-sidebars: When no sidebar content exists.
 *     - one-sidebar and sidebar-first or sidebar-second: A combination of the
 *       two classes when only one of the two sidebars have content.
 * - $node: Full node object. Contains data that may not be safe. This is only
 *   available if the current page is on the node's primary url.
 * - $menu_item: (array) A page's menu item. This is only available if the
 *   current page is in the menu.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 * - $mission: The text of the site mission, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $search_box: HTML to display the search box, empty if search has been disabled.
 * - $primary_links (array): An array containing the Primary menu links for the
 *   site, if they have been configured.
 * - $secondary_links (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title: The page title, for use in the actual HTML content.
 * - $messages: HTML for status and error messages. Should be displayed prominently.
 * - $tabs: Tabs linking to any sub-pages beneath the current page (e.g., the
 *   view and edit tabs when displaying a node).
 * - $help: Dynamic help text, mostly for admin pages.
 * - $content: The main content of the current page.
 * - $feed_icons: A string of all feed icons for the current page.
 *
 * Footer/closing data:
 * - $footer_message: The footer message as defined in the admin settings.
 * - $closure: Final closing markup from any modules that have altered the page.
 *   This variable should always be output last, after all other dynamic content.
 *
 * Helper variables:
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * Regions:
 * - $content_top: Items to appear above the main content of the current page.
 * - $content_bottom: Items to appear below the main content of the current page.
 * - $navigation: Items for the navigation bar.
 * - $sidebar_first: Items for the first sidebar.
 * - $sidebar_second: Items for the second sidebar.
 * - $header: Items for the header region.
 * - $footer: Items for the footer region.
 * - $page_closure: Items to appear below the footer.
 *
 * The following variables are deprecated and will be removed in Drupal 7:
 * - $body_classes: This variable has been renamed $classes in Drupal 7.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see zen_preprocess()
 * @see zen_process()
 */
?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>">

<head>
  <title><?php print $head_title; ?></title>
  <META NAME="description" CONTENT="The Public Laboratory for Open Technology and Science (PLOTS) collaboratively develops and publicizes accessible technologies for investigating and reporting on local environmental health and justice issues. PLOTS provides an online research space for citizens, linking them to scientists, social scientists, and technologists. PLOTS is an expansion of Grassroots Mapping, where citizens use helium-filled balloons and digital cameras to generate high resolution “satellite” maps.">
  <meta name="viewport" content=""/>
  <?php print $head; ?>
  <link rel="stylesheet" href="/sites/all/themes/plots/plots.css" type="text/css" media="screen" />
  <?php print $styles; ?>
  <!--[if !IE]><!--><link rel="stylesheet" href="/sites/all/themes/plots/not-ie.css" type="text/css" media="screen" /><!--<![endif]-->
  <!--[if lt IE 7]>
  <link rel="stylesheet" href="/sites/all/themes/plots/ie.css" type="text/css" media="screen" />
  <![endif]-->
  <?php print $scripts; ?>
  <script src="/scripts/jquery-nivo-slider-pack.js" type="text/javascript"></script>
  <link rel="stylesheet" href="/scripts/nivo-slider.css" type="text/css" media="screen" />
  <link href="http://publiclaboratory.org/rss.xml" rel="alternate" type="application/rss+xml" title="The Public Laboratory" />

</head>
<body class="<?php print $classes; ?>">
<div id="fb-root"></div>
<script>(function(d, s, id) {
  var js, fjs = d.getElementsByTagName(s)[0];
  if (d.getElementById(id)) return;
  js = d.createElement(s); js.id = id;
  js.src = "//connect.facebook.net/en_US/all.js#xfbml=1";
  fjs.parentNode.insertBefore(js, fjs);
}(document, 'script', 'facebook-jssdk'));</script>

  <div style="border:2px solid red;background:#fcc;padding:10px;" id="iewarning"><p><h2>Incompatible browser</h2>
The Public Laboratory website will not render correctly in Internet Explorer 6 or earlier. To view it properly, please use an up-to-date browser, such as <a href="http://firefox.com">Firefox</a>, <a href="http://google.com/chrome">Chrome</a>, or another standards-compliant browser. It's a good idea to upgrade anyways -- olderversions of IE <a href-"http://en.wikipedia.org/wiki/Comparison_of_web_browsers#Security_and_vulnerabilities">pose a security risk</a> for your computer.</p></div>

  <div id="page-wrapper"><div id="page">

    <ul id='links'> 
	<li><a href="/">PLOTS</a></li>
	<li onMouseOver="$('#aboutHeader').show()" onMouseOut="$('#aboutHeader').hide()">
		<ul class="sublinks" style="display:none;" id="aboutHeader">
			<li><a href="/getting-started">Getting started</a></li>
			<li><a href="/contribute">Contribute</a></li>
			<li><a href="/join">Join</a></li>
		</ul>
		<a href="/about">About</a>
	</li>
	<li style="margin-right:0;"><a href="/places">Places</a></li>
	<li onMouseOver="$('#placeHeader').show()" onMouseOut="$('#placeHeader').hide()">
		<ul class="sublinks" style="display:none;" id="placeHeader">
			<li><a href="/place/new-york-city">New York</a></li>
			<li><a href="/place/gulf-coast">Gulf Coast</a></li>
			<li><a href="/place/somerville-massachusetts">Somerville</a></li>
			<li><a href="/place/providence">Providence</a></li>
			<li><a href="/place/butte">Butte</a></li>
			<li><a href="/place/western-north-carolina">Western North Carolina</a></li>
			<li><a href="/place/davis">Davis</a></li>
			<li><a href="/place/texas">Texas</a></li>
			<li><a href="/place/sumava-czech-republic">Sumava, Czech Republic</a></li>
			<li><a href="/place/portland-oregon">Portland, OR</a></li>
			<li><a href="/place/santiago-chile">Santiago Chile</a></li>
			<li><a href="/place/vermont">Vermont</a></li>
		</ul>
		<a style="padding-right:8px;" href="javascript:void();">&or;</a>
	</li>
	<li><a href="/notes">Research notes</a></li>
	<li onMouseOver="$('#archiveHeader').show()" onMouseOut="$('#archiveHeader').hide()">
		<ul class="sublinks" style="display:none;" id="archiveHeader">
			<li><a href="/maps/gulf-coast">Gulf Coast</a></li>
			<li><a href="/maps/new-york-city">New York City</a></li>
		</ul>
		<a href="/archive">Archive</a>
	</li>
	<li style="margin-right:0;"><a href="/tools">Tools</a></li>
	<li onMouseOver="$('#toolHeader').show()" onMouseOut="$('#toolHeader').hide()">
		<ul class="sublinks" style="display:none;" id="toolHeader">
			<li><a href="/tool/balloon-mapping">Balloon mapping</a></li>
			<li><a href="/tool/near-infrared-camera">Near-infrared camera</a></li>
			<li><a href="/tool/thermal-photography">Thermal photography</a></li>
			<li><a href="/tool/indoor-air-quality-mapping">Indoor air quality mapping</a></li>
			<li><a href="/tool/spectrometer">Spectrometer</a></li>
			<li><a href="/tool/hydrogen-sulfide-sensing">Hydrogen sulfide sensing</a></li>
			<li><a href="/tool/environmental-estrogen-testing">Environmental estrogen testing</a></li>
		</ul>
		<a style="padding-right:8px;" href="javascript:void();">&or;</a>
	</li>
	<li><a href="/events">Events</a></li> 

        <?php if (in_array('alpha',$user->roles)) { ?>
	<li class="right"> 
		   <a style="border-left:8px solid white;background:#35d" href="javascript:(function alpha() { if ((window.location+'').split('publiclaboratory')[0] == 'http://' || (window.location+'').split('publiclaboratory')[0] == 'http://www.') { window.location = 'http://alpha.'+(window.location+'').split('://')[1] } else { window.location = 'http://'+(window.location+'').split('://alpha.')[1] }})()">&alpha;</a> 
	</li>
        <?php } ?>
	<?php if ( $user->uid ) { ?>
	<li class="right"> 
		<a href="/note/add">Post a note [+]</a> 
	</li>
	<?php } ?> 
      </ul>
    </div> 

    <div id="header"><div class="clearfix">

      <?php if ($logo): ?>
        <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /></a>
      <?php endif; ?>

      <?php print $header; ?>

    </div></div> <!-- /.section, /#header -->

    <?php if ($node->type == 'map') { ?>
    <div id="map-fullscreen-btn-div"><a id="map-fullscreen-btn" href="javascript:void();" onClick="fullscreen()">Fullscreen</a></div>
    <div id='map'>
      <a id="map-minimize-btn" href="javascript:void();" onClick="minimize()">Close</a>
    </div>
    <style> 
      #header { padding-bottom:0 !important;margin-bottom:0 !important; }
      #navigation .clearfix { margin:0 !important;background:none !important; }
      #content { margin-top:0 !important; }
      #content-area ul.links { background:none !important;padding-bottom:0 !important; }
    </style>
    <?php } ?>

    <div id="main-wrapper"><div id="main" class="clearfix<?php if ($primary_links || $navigation) { print ' with-navigation'; } ?>">

      <div id="content" class="column"><div class="section">

        <?php print $highlight; ?>

	<?php 
	global $user;
	$path = explode('/', request_uri() );

	$uid = $user->uid;
	$profilePage = false;
	if ($path[1] && $path[1] == "people" && $path[2]) {
		$uid = user_load(array('name' => $path[2]));
		$uid = $uid->uid;
		$profilePage = true;
	}

	if ($path[1] == "people") { ?>
	<a class="subscribeBtn" href="http://publiclaboratory.org/notifications/subscribe/<?php echo $user->uid; ?>/author/author/<?php echo $uid; ?>?destination=user%2F<?php echo $uid; ?>&confirm=0">Subscribe +</a>
	<?php } 

	if ( request_uri() == "/dashboard" || $profilePage) {
	
	$sql = "SELECT n.uid, n.title, n.created FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE n.status=1 AND n.type='note' AND n.uid ='".$uid."'";
	
	$result = db_query($sql);
	
	$weeks = array();
	
	while ($row = db_fetch_array($result) ) {
	//print_r($row["title"].":".$row["created"]);
	  $weeks_ago = time() - $row["created"];
	  $weeks_ago = $weeks_ago/(60*60*24*7);
	  $weeks[$weeks_ago]++;
	}
	
	for ($i = 0;$i<52;$i++) {
	  if (!isset($weeks[$i])) $weeks[$i] = 0;
	}
	$weeks = array_reverse(array_slice($weeks,0,52));
	
	?>
	<div id="dashboardGraph" <?php if (!$profilePage) { ?>style="margin-top:44px;"<?php } ?>>
	<span class="barSparkline"><?php echo implode(",",$weeks); ?></span>
	<a href="/wiki/github-graphs"><img class="githubGraph" src="/<?php echo path_to_theme(); ?>/images/52-week-graph.png" /></a>
	</div>
	<script type="text/javascript">
	$('.barSparkline').sparkline('html', {type: 'bar',barColor:'#555',barWidth:2});
	</script>
	<?php } ?>

        <?php print $breadcrumb; ?>

        <?php if ($title && strlen($title) < 100) { ?>
          <h1 class="title"><?php print $title; ?></h1>
        <?php } else if ($title) { ?>
          <h1 class="title"><small><?php print $title; ?></small></h1>
        <?php } ?>
        <?php print $messages; ?>
        <?php if ($tabs): ?>
          <div class="tabs"><?php print $tabs; ?></div>
        <?php endif; ?>
        <?php print $help; ?>

        <?php print $content_top; ?>


        <div id="content-area">
          <?php print $content; ?>
        </div>

	<br style="clear:both;" />

        <?php print $content_bottom; ?>

        <?php if ($feed_icons): ?>
          <div class="feed-icons"><?php print $feed_icons; ?></div>
        <?php endif; ?>

      </div></div> <!-- /.section, /#content -->

      <?php if ($primary_links || $navigation): ?>
        <div id="navigation"><div class="section clearfix">

<!--          <?php print theme(array('links__system_main_menu', 'links'), $primary_links,
            array(
              'id' => 'main-menu',
              'class' => 'links clearfix',
            ),
            array(
              'text' => t('Main menu'),
              'level' => 'h2',
              'class' => 'element-invisible',
            ));
          ?>

          <?php print $navigation; ?>

-->
        </div></div> <!-- /.section, /#navigation -->
      <?php endif; ?>
      <?php print $sidebar_first; ?>

      <?php print $sidebar_second; ?>

    </div></div> <!-- /#main, /#main-wrapper -->

    <p class="clearfix"></p>

    <?php if ($footer || $footer_message || $secondary_links): ?>
      <div id="footer" class="clearfix"><div class="section">

        <?php print theme(array('links__system_secondary_menu', 'links'), $secondary_links,
          array(
            'id' => 'secondary-menu',
            'class' => 'links clearfix',
          ),
          array(
            'text' => t('Secondary menu'),
            'level' => 'h2',
            'class' => 'element-invisible',
          ));
        ?>

        <?php if ($footer_message): ?>
          <div id="footer-message"><?php print $footer_message; ?></div>
        <?php endif; ?>

        <?php print $footer; ?>

      </div></div> <!-- /.section, /#footer -->
    <?php endif; ?>

  </div></div> <!-- /#page, /#page-wrapper -->

  <?php print $page_closure; ?>

  <?php print $closure; ?>

<script type="text/javascript">

  function urlParam(name) {
    return decodeURIComponent(urlParams()[name].replace(/\+/g,' ')) 
  }
  function urlParams() {
    params = {}
    $.each(location.toString().split('?')[1].split('&'),function(i,p) {
      params[p.split('=')[0]] = p.split('=')[1]
    })
    return params
  }
  (function(){
  
  if (location.toString().split('.org/')[1] == "note/add") {
    
  }

  if (location.toString().split('/')[location.toString().split('/').length-1]) {
    if (urlParam('title')!='null') $('input#edit-title').val(urlParam('title'))
    if (urlParam('body')!='null') $('textarea#edit-body').val(urlParam('body'))
    if (urlParam('tags')!='null') $('input#edit-taxonomy-tags-3').val(urlParam('tags')+"".replace('%2C',','))
  } 

  })()
</script>
<style>
  .page-note-add .taxonomy-super-select-checkboxes,
  .page-note-add #edit-taxonomy-tags-1-wrapper,
  .page-note-add legend.collapse-processed {
    display:none;
  }
  .page-note-add fieldset {
    border:0;
  }
  .page-note-add #edit-body {
    height:300px;
  }
</style>

<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-180781-33']);
  _gaq.push(['_setDomainName', 'publiclaboratory.org']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
</body>
</html>
