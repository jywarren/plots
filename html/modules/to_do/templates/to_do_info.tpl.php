<?php
// $Id: to_do_info.tpl.php,v 1.3.2.5 2010/08/04 09:17:36 alexiswilke Exp $
?>
<?php if ($action_buttons && $side_actions) { ?>
<fieldset class="to-do-buttons" style="float: right;"><legend><?php echo t('To Do Actions') ?></legend>
  <?php echo $action_buttons ?>
</fieldset>
<?php } ?>
<dl class="to-do-info">
  <dt class="to-do-status"><?php echo t("Status"); ?></dt>
  <dd class="to-do-status"><?php echo $item_status ?></dd>

  <dt class="to-do-priority"><?php echo t("Priority"); ?></dt>
  <dd class="to-do-priority"><?php echo $priority ?></dd>

  <?php if ($start_date) { ?>
    <dt class="to-do-start-date"><?php echo t("Start Date"); ?></dt>
    <dd class="to-do-start-date"><?php echo $start_date ?></dd>
  <?php } ?>

  <?php if ($deadline) { ?>
    <dt class="to-do-deadline"><?php echo t("Deadline"); ?></dt>
    <dd class="to-do-deadline"><?php echo $deadline ?></dd>
  <?php } ?>

  <?php if ($users) { ?>
    <dt class="to-do-assigned-to"><?php echo t("Assigned To"); ?></dt>
    <dd class="to-do-assigned-to"><ul class="to-do-users">
      <?php
      foreach ($users as $u) {
        echo '<li>', $u, '</li>';
      }
      ?>
    </ul></dd>
  <?php } ?>

  <?php if ($navigation_buttons) { ?>
    <dt class="to-do-navigation"><?php echo t("Navigation"); ?></dt>
    <dd class="to-do-navigation"><ul><?php echo $navigation_buttons; ?></ul></dd>
    </dt>
  <?php } ?>
</dl>
<?php
if ($action_buttons && !$side_actions) {
  echo $action_buttons;
}
