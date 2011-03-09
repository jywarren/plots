<form action="http://groups.google.com/group/<?php echo check_plain($group_id); ?>/boxsubscribe">
  <div class="gg-subscribe-form">
    <p>
      <label for="google-groups-email" ><?php print t('Email:'); ?></label>
      <input  id="google-groups-email" class="input-text" type="text" name="email" />
    </p>
    <p><input class="input-submit" type="submit" name="sub" value="<?php print t('Subscribe'); ?>" /></p>
    <p><img src="http://groups.google.com/groups/img/3nb/groups_bar.gif" height="26" width="132" alt="<?php print t('Google Groups'); ?>" /></p>
    <p><?php print l(t('Visit this group'), 'http://groups.google.com/group/' . check_plain($group_id)); ?></p>
  </div>
</form>
