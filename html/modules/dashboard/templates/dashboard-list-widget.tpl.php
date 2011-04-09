<?php // $Id: dashboard-list-widget.tpl.php,v 1.2 2010/05/22 15:02:21 techsoldaten Exp $ ?>
<p><?php print $help; ?></p>
<?php print $widget_add; ?>
<?php print $widget_filter; ?>
<?php foreach ($widgets as $widget): ?>
  <table class="widget-entry <?php print $widget->classes; ?>">
    <tbody>
      <tr>
        <td class="widget-name">
          <?php print $help_type_icon; ?>
          <?php print t('<em>@type</em> @base widget: <strong>@view</strong>', array('@type' => $widget->type, '@view' => $widget->title, '@base' => $widget->subtype)); ?>
          <?php if (!empty($widget->tag)): ?>
            &nbsp;(<?php print $widget->tag; ?>)
          <?php endif; ?>

          <?php if ($widget->title): ?>            
            <div class="widget-title"><?php print t('Title: @title', array('@title' => $widget->title)); ?></div>
          <?php endif; ?>
        </td>
        <td class="widget-description">
          <?php if ($widget->description): ?>
            <?php print $widget->description; ?>
          <?php endif; ?>
        </td>
        <td class="widget-ops"><?php print $widget->ops ?></td>
      </tr>
    </tbody>
  </table>
<?php endforeach; ?>