<?php
// $Id: node.tpl.php,v 1.10 2009/11/02 17:42:27 johnalbin Exp $

/**
 * @file
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $user_picture: The node author's picture from user-picture.tpl.php.
 * - $date: Formatted creation date. Preprocess functions can reformat it by
 *   calling format_date() with the desired parameters on the $created variable.
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $display_submitted: whether submission information should be displayed.
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - node: The current template type, i.e., "theming hook".
 *   - node-[type]: The current node type. For example, if the node is a
 *     "Blog entry" it would result in "node-blog". Note that the machine
 *     name will often be in a short form of the human readable label.
 *   - node-teaser: Nodes in teaser form.
 *   - node-preview: Nodes in preview mode.
 *   The following are controlled through the node publishing options.
 *   - node-promoted: Nodes promoted to the front page.
 *   - node-sticky: Nodes ordered above other non-sticky nodes in teaser
 *     listings.
 *   - node-unpublished: Unpublished nodes visible only to administrators.
 *   The following applies only to viewers who are registered users:
 *   - node-by-viewer: Node is authored by the user currently viewing the page.
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $build_mode: Build mode, e.g. 'full', 'teaser'...
 * - $teaser: Flag for the teaser state (shortcut for $build_mode == 'teaser').
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * The following variables are deprecated and will be removed in Drupal 7:
 * - $picture: This variable has been renamed $user_picture in Drupal 7.
 * - $submitted: Themed submission information output from
 *   theme_node_submitted().
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 * @see zen_preprocess()
 * @see zen_preprocess_node()
 * @see zen_process()
 */
?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix">
  <?php print $user_picture; ?>

  <?php if (!$page): ?>
    <h2 class="title"><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>

  <?php if ($unpublished): ?>
    <div class="unpublished"><?php print t('Unpublished'); ?></div>
  <?php endif; ?>

  <?php if ($display_submitted || $terms): ?>
    <div class="meta">
      <?php if ($display_submitted): ?>
      <?php endif; ?>

      <?php if (false): //$terms): ?>
        <div class="terms terms-inline"><?php print $terms; ?></div>
      <?php endif; ?>
    </div>
  <?php endif; ?>

<script type="text/javascript" src="http://openlayers.org/api/2.11/OpenLayers.js"></script>
<script src='http://maps.google.com/maps?file=api&amp;v=2&amp;key=ABQIAAAANO6Yx8ihhesSqnPHx9a3RxQCA0kQZc0rHxnaN3mazoBpOqX1oBQwLut2gk7rd_T9sYyxcGJrxQK3gg' type='text/javascript'></script> 
<script src="http://api.maps.yahoo.com/ajaxymap?v=3.0&amp;appid=INSERT_YOUR_YAHOO_APP_ID_HERE"></script>

<script>
<?php 
	$sw = explode(' ',str_replace(')','',str_replace('POINT(','',$node->field_bbox[0]['view'])));
	$ne = explode(' ',str_replace(')','',str_replace('POINT(','',$node->field_bbox[1]['view'])));
?>

	var sw_lon = <?php echo $sw[0] ?>;
	var sw_lat = <?php echo $sw[1] ?>;
	var ne_lon = <?php echo $ne[0] ?>;
	var ne_lat = <?php echo $ne[1] ?>;

	var cen_lon = (sw_lon+ne_lon)/2
	var cen_lat = (sw_lat+ne_lat)/2

	var map;
	var mapBounds = new OpenLayers.Bounds(sw_lon,sw_lat,ne_lon,ne_lat)
	var mapMinZoom = <?php print ($node->field_zoom_min[0]['value']) ?>;
	<?php $zoom_max = $node->field_zoom_max[0]['value'];if (!$zoom_max) $zoom_max = 22; ?>

	var mapMaxZoom = <?php print ($zoom_max); ?>;

	var spher_merc = new OpenLayers.Projection("EPSG:4326")
	var latlon = new OpenLayers.Projection("EPSG:900913")
        
	// avoid pink tiles
        OpenLayers.IMAGE_RELOAD_ATTEMPTS = 3;
        OpenLayers.Util.onImageLoadErrorColor = "transparent";

	OpenLayers.theme = '/map-styles/dark/style.css';
	OpenLayers.ImgPath = '/map-styles/dark/';
	// http://mapbox.github.com/webintegration

	map = new OpenLayers.Map('map', { //controls: [], 
		tileOrigin: new OpenLayers.LonLat(0,0).transform(latlon,spher_merc),
		units: "m",
		//theme: "/theme/mapmill.css",
		projection: spher_merc,
		displayProjection: latlon, 
		maxExtent: new OpenLayers.Bounds(-20037508, -20037508, 20037508, 20037508.34),//mapBounds,
		maxResolution: 156543.0339,
		controls: [
		  new OpenLayers.Control.TouchNavigation({
                    dragPanOptions: {
                      enableKinetic: true
                    }
           	  })
		]
	});

        var grm_tms = new OpenLayers.Layer.TMS( "<?php print $title; ?> (<?php echo date("F j, Y",strtotime($node->field_capture_date[0][value])); ?>)",
	"<?php print $node->field_tms_url[0]['value'] ?>",
        { projection: latlon,
	  getURL: overlay_getTileURL,
//	  maxExtent: mapBounds,
//	  displayOutsideMaxExtent: false, 
	  attribution: '<a href="http://publiclaboratory.org/">Public Laboratory</a>',
          serviceVersion: '.', 
          layername: '.', 
          type: '<?php if ($node->field_tms_tile_type[0]['value']) { print $node->field_tms_tile_type[0]['value']; } else { print "png"; } ?>', 
          alpha: true, 
          isBaseLayer: false,
	 // transitionEffect: "resize",
	});
        map.addLayer(grm_tms);


	// create OSM/OAM layer
	var osm = new OpenLayers.Layer.TMS( "OpenStreetMap",
		"http://tile.openstreetmap.org/",
		{ type: 'png', getURL: osm_getTileURL, displayOutsideMaxExtent: true, attribution: '<a href="http://www.openstreetmap.org/">OpenStreetMap</a>'} );
        //map.addLayer(osm);
	var oam = new OpenLayers.Layer.TMS( "OpenAerialMap",
		"http://tile.openaerialmap.org/tiles/1.0.0/openaerialmap-900913/",
		{ type: 'png', getURL: osm_getTileURL } );
 


	var gsat = new OpenLayers.Layer.Google("Google Satellite", 
	{ type: G_SATELLITE_MAP, 
	  sphericalMercator: true, 
	  numZoomLevels: 25
	});
        var yahoosat = new OpenLayers.Layer.Yahoo("Yahoo Satellite",
        {'type': YAHOO_MAP_SAT, 'sphericalMercator': true});

	map.addLayer(gsat)

// you can try
// http://hypercube.telascience.org/tilecache/tilecache.py/1.0.0/NAIP_ALL/

// but you might get better performance from newworld which switches
// between bmng/landsat/naip based on zoom level

// http://hypercube.telascience.org/tilecache/tilecache.py/1.0.0/NewWorld_google

	var tile_url = "http://tilestream.publiclaboratory.org/1.0.0/blue-marble-topo-bathy-jan/"
	//var tile_url = "http://localhost:8888/1.0.0/blue-marble-topo-bathy-jul/"
        var tms = new OpenLayers.Layer.TMS( "Tilestream Blue Marble", tile_url,
        { projection: latlon,
          serviceVersion: '.', 
          layername: '.', 
          type: 'png', 
          alpha: true, 
          isBaseLayer: true,
	  transitionEffect: "resize",
	});
        map.addLayer(tms);

	gsat.setVisibility(true);
	tms.setVisibility(false);

	//if (OpenLayers.Util.alphaHack() == false) { tmsoverlay.setOpacity(0.7); }
	if (OpenLayers.Util.alphaHack() == false) { gsat.setOpacity(0.7); }

	plotsPanZoom = OpenLayers.Class(OpenLayers.Control.PanZoom, {
        includeButtons: {
            "zoomin": {
                outImageSrc: "zoom-plus-mini.png",
                overImageSrc: "zoom-plus-mini-over.png"
            },
            "zoomout": {
                outImageSrc: "zoom-minus-mini.png",
                overImageSrc: "zoom-minus-mini-over.png"
            }
        },
	})
	
	map.addControl(new OpenLayers.Control.LayerSwitcher({'div':OpenLayers.Util.getElement('layerswitcher')}));
	map.addControl(new plotsPanZoom());
	//map.addControl(new OpenLayers.Control.PanZoom());
	map.addControl(new OpenLayers.Control.MouseDefaults());
	map.addControl(new OpenLayers.Control.KeyboardDefaults());

	map.zoomToExtent( mapBounds.transform(spher_merc,latlon ) );

	// switch resolutions based on zoomlevel:
	map.events.register("zoomend", map, function() {
		if (map.getResolution() < 611) {
			tms.setVisibility(false);
			gsat.setVisibility(true);
		} else if (map.getResolution() > 612) {
			tms.setVisibility(true);
			gsat.setVisibility(false);
		}
	})

        function overlay_getTileURL(bounds) {
	        var res = this.map.getResolution();
	        var x = Math.round((bounds.left - this.maxExtent.left) / (res * this.tileSize.w));
	        var y = Math.round((bounds.bottom - this.tileOrigin.lat) / (res * this.tileSize.h));
	        var z = this.map.getZoom();
		if (mapBounds.intersectsBounds( bounds ) && z >= mapMinZoom && z <= mapMaxZoom ) {
	               //console.log( this.url + z + "/" + x + "/" + y + "." + this.type);
	               return this.url + z + "/" + x + "/" + y + "." + this.type;
                } else {
                   return "http://publiclaboratory.org/map-styles/none.png";
                }
        }		

        function osm_getTileURL(bounds) {
            var res = this.map.getResolution();
            var x = Math.round((bounds.left - this.maxExtent.left) / (res * this.tileSize.w));
            var y = Math.round((this.maxExtent.top - bounds.top) / (res * this.tileSize.h));
            var z = this.map.getZoom();
            var limit = Math.pow(2, z);

            if (y < 0 || y >= limit) {
                return "http://publiclaboratory.org/map-styles/none.png";
            } else {
                x = ((x % limit) + limit) % limit;
                return this.url + z + "/" + x + "/" + y + "." + this.type;
            }
        }

	function fullscreen() {
		$('#map').addClass('map-fullscreen')
		$('#map-fullscreen-btn').addClass('map-fullscreen-btn-full')
		$('#map-minimize-btn').addClass('show')
		map.updateSize()
		$('#links').hide()
		$('#main').hide()
		$('#header').hide()
		$('#footer').hide()
		$('meta[name=viewport]').attr('content', 'width=320; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;');
	}
	function minimize() {
		$('#map').removeClass('map-fullscreen')
		$('#map-fullscreen-btn').removeClass('map-fullscreen-btn-full')
		$('#map-minimize-btn').removeClass('show')
		map.updateSize()
		$('#footer').show()
		$('#links').show()
		$('#main').show()
		$('#header').show()
		$('body').removeClass('body-fullscreen')
		$('meta[name=viewport]').attr('content', '');
	}
</script>

<div id="hide-on-fullscreen">

<div class="formats">
	<h3>Formats:</h3>
<!--	<?php if ($node->field_openlayers_url[0]['value']) { ?><a class="openlayers-field" href="<?php print $node->field_openlayers_url[0]['value'] ?>">OpenLayers</a><?php } ?>-->
	<?php if ($node->field_tms_url[0]['value']) { ?><a class="tms-field" href="<?php print $node->field_tms_url[0]['value'] ?>">TMS</a><?php } ?>
	<?php if ($node->field_geotiff_url[0]['value']) { ?><a class="geotiff-field" href="<?php print $node->field_geotiff_url[0]['value'] ?>">GeoTiff<?php if ($node->field_geotiff_filesize[0]['value']) { ?> (<?php print formatbytes($node->field_geotiff_filesize[0]['value']) ?>)<?php } ?></a><?php } ?>
	<?php if ($node->field_mbtiles_url[0]['value']) { ?><a class="mbtiles-field" href="<?php print $node->field_mbtiles_url[0]['value'] ?>">MBTiles</a><?php } ?>
	<?php if ($node->field_google_maps_url[0]['value']) { ?><a class="googlemaps-field" href="<?php print $node->field_google_maps_url[0]['value'] ?>">Google Maps</a><?php } ?>
 	<?php if ($node->field_jpg_url[0]['value']) { ?><a class="jpg-field" href="<?php print $node->field_jpg_url[0]['value'] ?>">JPG<?php if ($node->field_jpg_filesize[0]['value']) { ?> (<?php print formatbytes($node->field_jpg_filesize[0]['value']) ?>)<?php } ?></a><?php } ?>
	<?php if ($node->field_raw_images[0]['value']) { ?><a class="raw-field" href="<?php print $node->field_raw_images[0]['value'] ?>">Raw images<?php if ($node->field_raw_images_filesize[0]['value']) { ?> (<?php print formatbytes($node->field_raw_images_filesize[0]['value']) ?>)<?php } ?></a><?php } ?>
  </div>
  <div id="embed">
	<?php 
		$cen_lon = ($sw[0]+$ne[0])/2;
		$cen_lat = ($sw[1]+$ne[1])/2;
	?>
	<p><b>Embed code:</b> <input type="text" id="embedcode" value='<iframe style="border:none;" width="500" height="375" src="http://archive.publiclaboratory.org/leaflet/?tms=<?php if ($node->field_tms_url[0]['value']) { print $node->field_tms_url[0]['value']; } ?>&lon=<?php print $cen_lon; ?>&lat=<?php print $cen_lat; ?>&zoom=17"></iframe>' /></p>

  </div>
  <div class="content nodebody">
    <div class="field field-type-text field-field-license">
      <div class="field-label">Tags:&nbsp;</div>
        <div class="field-items">
        <?php
		foreach ($node->taxonomy as $term) {
			if ($term->vid == 6) {
				echo("<a href='/maps/".$term->name."'>".$term->name."</a>"); 
			}
		}
	 ?>
        </div>
    </div>
    <?php print $content; ?>
  </div>

  <div id="share">
	<a name="fb_share" type="button_count" href="http://www.facebook.com/sharer.php">Share</a><script src="http://static.ak.fbcdn.net/connect.php/js/FB.Share" type="text/javascript"></script>  
	<a href="http://twitter.com/share?text=Grassroots map of <?php print $title; ?> in the Public Laboratory Archive: "><img src="/img/twitter.png" /></a>
  </div>

  <?php print $links; ?>

  </div> <!-- #hide-on-fullscreen -->
</div> <!-- /.node -->
