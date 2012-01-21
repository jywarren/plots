<div class="node<?php if ($sticky) { print " sticky"; } ?>">
    <?php if ($picture) {
      print $picture;
    }?>
    <?php if ($page == 0) { ?><h2 class="title"><a href="<?php print $node_url?>"><?php print $title?></a></h2><?php }; ?>
    <span class="submitted"><?php print $submitted?></span>
    <span class="taxonomy"><?php print $terms?></span>

  <div class="content">
    <table class="node-content-table"><tr>
    <td class="node-body">
	  <?php print $content ?>
	</td>

    <?php if ($node->node_images) { ?>
  	  <!-- Node images -->
	  <td class="node-images" rowspan="2">
	    <?php print $node->node_images ?><?php ?>
		<div style="text-align:center;">
		  <a href="<?php print url('node/'.$node->nid.'/image_gallery'); ?>">Gallery</a>
		</div>
	  </td>
	<?php }?>

	</tr><tr>
    </tr></table>
  </div>

  <?php if ($links) { ?>
    <div class="links">&raquo; <?php print $links?></div>
  <?php }?>
</div>