<?php
// $Id: to_do_block.pages.inc,v 1.1.2.8 2010/11/24 20:46:22 alexiswilke Exp $

/**
 * @file
 * Generate the block and related entries.
 */

/**
 * Verify the page number for the To do block. The function verifies
 * that the number is withing range and saves it in the session so the
 * AJAX paging works as expected.
 */
function _to_do_block_page($page = NULL) {
  // if not set, select the default value
  if (!$page) {
    $page = empty($_SESSION['to_do_page']) ? 1 : $_SESSION['to_do_page'];
  }
  if ($page <= 0) {
    $page = 1;
  }
  // elseif ($page > $max) ... How do we efficiently calculate $max? save count in session?!
  $_SESSION['to_do_page'] = $page;

  return $page;
}

/**
 * Actual implementation of the hook_block() when operation is view.
 */
function _to_do_block_list() {
  global $user;

  // CSS for the block
  $path = drupal_get_path('module', 'to_do');
  drupal_add_css($path . '/css/to_do_block.css');

  $page = _to_do_block_page();

  $user_preferences = _to_do_user_preferences($user->uid);
  $items_per_page = $user_preferences['sidebar_items'];

  // get page counts... we do that in the _to_do_get_data() even when
  // the first parameter is FALSE so we may want to consider using
  // that result instead.
  $urgent_count = db_result(_to_do_get_data(TRUE, $page, TO_DO_SELECTION_URGENT));
  $urgent_page_count = ceil($urgent_count / $items_per_page);

  $all_count = db_result(_to_do_get_data(TRUE, $page, TO_DO_SELECTION_ALL));
  $all_page_count = ceil($all_count / $items_per_page);

  $selection = to_do_last_selection();
  $clean_urls = variable_get('clean_url', 0);
  $base_path = base_path() . ($clean_urls ? '' : '?q=');

  $module_settings = array(
    'urgentPageCount' => $urgent_page_count,
    'allPageCount' => $all_page_count,
    'selection' => $selection,
    'page' => $page,
    'basePath' => $base_path,
  );
  drupal_add_js(array('toDo' => $module_settings), "setting");
  drupal_add_js($path . '/scripts/to_do_block.js');

  $list = _to_do_block_generate_list();
  if (empty($list)) {
    return NULL;
  }

  $content = $list['table'];

  if (variable_get('to_do_block_list_selection', TO_DO_SELECTION_LAST_SELECTION)
                              == TO_DO_SELECTION_LAST_SELECTION) {
    $active = ' class="active"';
    $urgent = $selection == "urgent";
    $tab_url_base = $base_path . 'to_do/json/'; // AJAX links
    // XXX -- add support so users without JavaScript can still enjoy the tabs.
    // (i.e. href=...?to_do_selection=urgent|all...
    $content = '<ul class="primary">'
        . '<li' . ($urgent ? $active : '') . '><a jsref="' . $tab_url_base . 'urgent/1">' . t('Urgent') . '</a></li>'
        . '<li' . ($urgent ? '' : $active) . '><a jsref="' . $tab_url_base . 'all/1">'    . t('All')    . '</a></li>'
      . '</ul>'
      . $content;
  }

  $content .= '<div id="to_do_pagination" class="item-list"></div>';

  // only users who have enough permissions managing the to do items get to see action buttons
  // (although the Help button may be arguable it's not very necessary for people who cannot
  // edit the to do items...)
  if (_to_do_access_callback($user->uid) && variable_get('to_do_display_icon', 1)) {
    $content .= '<div class="to-do-links">';
    // only users who have the right to create to do content see the Add button
    if (user_access('create to do content')) {
      $content .= l(
        t('New To do'),
        'node/add/to-do',
        array(
          'attributes' => array(
            'id' => 'to_do_add_link',
            'title' => t('Add new To do item'),
          ),
        )
      );
    }
    $content .= l(
      t('To do list'),
      'user/' . $user->uid . '/to_do',
      array(
        'attributes' => array(
          'id' => 'to_do_list_link',
          'title' => t('To do list'),
        ),
      )
    );
    $help_link = variable_get('to_do_block_help_link', NULL);
    if (is_null($help_link)) {
      $help_link = TO_DO_HELP_LINK;
    }
    if ($help_link) {
      $content .= l(
        t('Help'),
        $help_link,
        array(
          'attributes' => array(
            'id' => 'to_do_help_link',
            'title' => t('To do help'),
            'target' => '_blank',
          ),
        )
      );
    }
    $content .= '</div>';
  }

  $block = array(
    'subject' => $list['title'],
    'content' => $content,
  );


  return $block;
}

/**
 * Generate the title & list of the block so we can send them via
 * the JSON order.
 */
function _to_do_block_generate_list() {
  global $user;

  $status_list = _to_do_status_list();
  $priority_list = _to_do_priority_list();

  $display_status_column   = variable_get('to_do_status_column_display',   1);
  $display_priority_column = variable_get('to_do_priority_column_display', 1);
  $display_deadline_column = variable_get('to_do_deadline_column_display', 1);

  $output_middle = '';
  $urgent_exists = FALSE;
  $columns = 1
    + ($display_status_column   ? 1 : 0)
    + ($display_priority_column ? 1 : 0)
    + ($display_deadline_column ? 1 : 0);

  // No sorting capabilities because this list is heavily used
  // with AJAX and the sorting would not work well (plus it requires
  // a reload of the page which we try to avoid with this block.)
  $header = array();
  $header[] = array(
    'data' => t('Title'),
    'scope' => 'col',
  );
  if ($display_status_column) {
    $header[] = array(
      'data' => t('Status'),
      'scope' => 'col',
    );
  }
  if ($display_priority_column) {
    $header[] = array(
      'data' => t('Priority'),
      'scope' => 'col',
    );
  }
  if ($display_deadline_column) {
    $header[] = array(
      'data' => t('Deadline'),
      'scope' => 'col',
    );
  }

  $teaser_to_title = variable_get('to_do_block_list_title_with_teaser', 0);

  $rows = array();
  $to_do_listing = _to_do_get_data(FALSE, _to_do_block_page(), to_do_last_selection());
  while ($list_item = db_fetch_array($to_do_listing)) {
    $row = array();
    $options = array();

    if ($teaser_to_title) {
      $node = node_load($list_item['nid']);
      $title = strip_tags(node_view($node, TRUE, FALSE, FALSE));
      $options['attributes'] = array('title' => $title);
    }

    // name of the assignment
    $row[] = array(
      'data' => l($list_item['title'], 'node/' . $list_item['nid'], $options),
      'scope' => 'row',
    );

    // assign the status, priority & dates
    if ($display_status_column) {
      $row[] = $status_list[$list_item['item_status']];
    }
    if ($display_priority_column) {
      $row[] = $priority_list[$list_item['priority']];
    }
    if ($display_deadline_column) {
      $row[] = array(
        'data' => to_do_format_date($list_item['deadline']),
        'class' => 'to-do-middle',
      );
    }

    $status_class = to_do_item_status_class($list_item);
    switch ($status_class) {
    case 'to-do-late':
    case 'to-do-now':
    case 'to-do-immediate':
      $urgent_exists = TRUE;
      break;

    }
    $rows[] = array(
      'data' => $row,
      'class' => $status_class,
    );
  }

  if (count($rows) == 0) {
    // if empty, the user may want to hide the block altogether!
    if (variable_get('to_do_block_list_hide_when_empty', 0)) {
      return NULL;
    }

    $row = array(
      array(
        'data' => t('Your To do list is empty!'),
        'colspan' => $columns,
        'style' => 'text-align: center;',
      ),
    );
    $rows[] = $row;
  }
  elseif ($urgent_exists) {
    $row = array(
      array(
        'data' => t('Items in red and purple have been marked "immediate" or reached their deadline.'),
        'colspan' => $columns,
      ),
    );
    $rows[] = array(
      'data' => $row,
      'class' => 'to-do-immediate'
    );
  }

  // DO NOT CALL to_do_last_selection() for this test
  if (variable_get('to_do_block_list_selection', TO_DO_SELECTION_LAST_SELECTION)
                              == TO_DO_SELECTION_LAST_SELECTION) {
    // the tabs are enough to let us know the current mode
    $title = t('To do list');
  }
  else {
    $title = t('To do list (Mode:&nbsp;@selection)',
                  array('@selection' => _to_do_single_column_list(to_do_last_selection())));
  }
  return array(
    'title' => $title,
    'table' => theme('table', $header, $rows),
  );
}

/**
 * callback function for to_do/json/%/%
 */
function to_do_block_json($selection, $page_number) {
  // current user has the right to do that?!
  if (!user_access('access content')
      || !(user_access('can be assigned to dos') || user_access('create to do content'))) {
    drupal_access_denied();
    return;
  }

  // save new selection/page number
  to_do_last_selection($selection);
  _to_do_block_page($page_number);

  $list = _to_do_block_generate_list();

  // IMPORTANT: $list may be NULL if the list is empty and the
  //            flag asking to hide the block when empty is set
  //            to 1; we transform the NULL list in something
  //            that can be displayed in the JavaScript.
  if (!is_array($list)) {
    $list = array(
      'title' => t('To do list'),
      'table' => t('<table><tbody><tr><td><p>This list is empty.</p></td></tr></tbody></table>'),
    );
  }

  drupal_set_header('Content-Type: text/plain; charset: utf-8');
  echo drupal_to_js($list);
  exit();
}

/**
 * Function querying the database for the To do list block.
 *
 * \param[in] $count Whether the SQL query is to count or retrieve the data.
 * \param[in] $page The page to be returned.
 * \param[in] $selection The current selection: TO_DO_SELECTION_URGENT or TO_DO_SELECTION_ALL.
 */
function _to_do_get_data($count, $page, $selection) {
  global $user;

  // TODO: should we allow users (admins) to select who's items to see?
  //       (i.e. do tdau.uid IN (...) instead of just uid = current user.)
  $sql = " FROM {to_do} td, {to_do_assigned_users} tdau, {node} n"
    . " WHERE n.vid = td.vid AND n.vid = tdau.vid"
      . " AND n.type = 'to_do' AND tdau.uid = " . (int)$user->uid
      . " AND n.status = 1 AND td.item_status NOT IN ("
          . TO_DO_STATUS_FINISHED . ", " . TO_DO_STATUS_CANCELED . ")";

  if ($selection == TO_DO_SELECTION_URGENT) {
    // +8 days = 691200 seconds
    // XXX -- we really need a variable that each user can set for this +8 days
    $sql .= " AND (td.deadline <= " . (time() + 691200)
              . " OR td.priority = " . TO_DO_PRIORITY_IMMEDIATE . ")";
  }

  $user_preferences = _to_do_user_preferences($user->uid);
  if (empty($user_preferences['low_priority_items_display'])) {
    $sql .= " AND td.priority <> " . TO_DO_PRIORITY_LOW;
  }

  $sql_count = "SELECT COUNT(n.nid)" . $sql;
  if ($count) {
    return db_query(db_rewrite_sql($sql_count, 'n', 'nid'));
  }

  $sql_listing = "SELECT n.nid, n.title, td.deadline, td.item_status, td.priority"
                  . $sql . " ORDER BY td.deadline, td.priority DESC";

  // How do we determine the maximum $start without having the result of the COUNT()'s...
  $items_per_page = $user_preferences['sidebar_items'];

  // This is necessary if you change the settings... (# of items, etc.)
  $total_items = db_result(db_query(db_rewrite_sql($sql_count, 'n', 'nid')));
  $page_count = ceil($total_items / $items_per_page);
  if ($page > $page_count) {
    $page = max(1, $page_count);
  }
  $start = max(0, ($page - 1) * $items_per_page);

  return db_query_range(db_rewrite_sql($sql_listing, 'n', 'nid'), $start, $items_per_page);
}

// vim: ts=2 sw=2 et syntax=php
