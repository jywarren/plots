<div class="dashboard-widget <?php print $widget->classes; ?>">
  <?php if ($thumbnail): ?>
    <div class="widget-thumbnail"><?php print $thumbnail; ?></div>
  <?php endif; ?>
  <div class="widget-content">
    <h3 class="widget-name"><?php print $widget->title; ?></h3>
    <div class="widget-desc"><?php print $widget->description; ?></div>
    <div class="widget-edit">
      <div class="widget-button"><?php print $add_to_dashboard; ?></div>
      <div class="widget-tags"><strong><?php print t('Categories:'); ?></strong> <?php print $widget->tag; ?></div>
    </div>
  </div>
  <div class="clear"></div>
</div>

