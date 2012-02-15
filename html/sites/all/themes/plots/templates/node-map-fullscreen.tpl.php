<!-- Leaflet CSS -->
<link rel="stylesheet" href="/sites/all/themes/plots/leaflet/leaflet.css" />
<!--[if lte IE 8]><link rel="stylesheet" href="<path_to_dist>/leaflet.ie.css" /><![endif]-->

<!-- Leaflet JavaScript -->
<script src="/sites/all/themes/plots/leaflet/leaflet.js"></script>

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
//
//	var mapBounds = new OpenLayers.Bounds(sw_lon,sw_lat,ne_lon,ne_lat)
//	var mapMinZoom = <?php print ($node->field_zoom_min[0]['value']) ?>;
//	var mapMaxZoom = 22;
//
//	"<?php print $node->field_tms_url[0]['value'] ?>",
//	  attribution: '<a href="http://publiclaboratory.org/">Public Laboratory</a>',
 //         type: '<?php if ($node->field_tms_tile_type[0]['value']) { print $node->field_tms_tile_type[0]['value']; } else { print "png"; } ?>', 
// initialize the map on the "map" div
var map = new L.Map('map');

// create a CloudMade tile layer (or use other provider of your choice)
var cloudmade = new L.TileLayer(<?php print $node->field_tms_url[0]['value'] ?>'/{z}/{x}/{y}.png', {
    attribution: '<a href="http://publiclaboratory.org">Public Laboratory</a>',
    maxZoom: 22
});

// add the CloudMade layer to the map set the view to a given center and zoom
map.addLayer(cloudmade).setView(new L.LatLng(cen_lat,cen_lon), 19);


</script>

<div id="map"></div>

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
  <div class="content nodebody">
    <?php print $content; ?>
  </div>

<!--  <div id="share">
	<a name="fb_share" type="button_count" href="http://www.facebook.com/sharer.php">Share</a><script src="http://static.ak.fbcdn.net/connect.php/js/FB.Share" type="text/javascript"></script>  
	<a href="http://twitter.com/share?text=Grassroots map of <?php print $title; ?> in the Public Laboratory Archive: "><img src="/img/twitter.png" /></a>
  </div>
-->
