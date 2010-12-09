<?php
// $Id: activity-comment.tpl.php,v 1.1.2.1 2010/01/13 05:25:29 scottreynolds Exp $

/**
 * @file activity-comment.tpl.php
 *
 * Theme implementation to display an activity comment.
 *
 * Available variables:
 * - $username: themed up user name with a link to user's profile.
 * - $comment_value: santized comment text.
 * - $date: date of the comment submitted.
 * - $delete_link: the link to delete the indiviual comment.
 * - $comment: The comment object to be displayed may contained unsafe values.
 *   
 * @see template_preprocess_activity_comment()
 */
?>
<div class='container-inline'>
  <span class='activity-user'><?php print $username ?></span>
  <div class='activity-comment'><?php print $comment_value ?></div>
</div>
<div class='activity-comments-meta'>
  <div class='activity-comments-timestamp'>
    <?php print $date; ?>
  </div>
  <?php if (!empty($delete_link)) :?>
    <div class='activity-comments-delete'>
      <?php print $delete_link; ?>
    </div>
  <?php endif; ?>
</div>
