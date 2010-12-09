<?php
// $Id: event_database.pgsql.inc,v 1.17 2008/09/07 01:54:36 killes Exp $

/**
 * @param $nid node ID
 *
 * @return event object
 */
function event_get_event_load($nid) {
  global $user;
  $shift = variable_get('date_default_timezone', 0);

  return db_fetch_object(db_query("SELECT e.event_start, e.event_end, e.timezone, e.has_time, e.has_end_date, tz.\"offset\", tz.offset_dst, tz.dst_region, tz.is_dst, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_start_utc, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_end_utc, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_user, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_user, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_site, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_site, tz.name as timezone_name FROM {event} e INNER JOIN {event_timezones} tz ON tz.timezone = e.timezone WHERE nid = %d", (isset($user->timezone) ? $user->timezone : 0), (isset($user->timezone) ? $user->timezone : 0), $shift, $shift, $nid));
}

/**
 * @param $first start date
 * @param $last end date
 * @param Whether to order the result set on the starting date.
 *        Valid values: FALSE, 'DESC', 'ASC'
 * @param $rewrite_parameter optional array that is merged into the 
 *                      fourth argument of db_rewrite_sql.
 *
 * @return database resource
 */
function event_get_events_user($first, $last, $order, $rewrite_parameter = array()) {
  global $user;
  $shift = variable_get('date_default_timezone', 0);
  $before_first = event_implode_date(event_date_later(event_explode_date($first), -1));
  $after_last = event_implode_date(event_date_later(event_explode_date($last), 1));

  $query = "SELECT n.nid, n.title, n.type, e.event_start AS event_start_orig, e.event_end AS event_end_orig, e.timezone, e.has_time, e.has_end_date, tz.\"offset\", tz.offset_dst ELSEtz.dst_region, tz.is_dst, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_start_utc, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_end_utc, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_user, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_user, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_site, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_site, tz.name as timezone_name FROM {node} n INNER JOIN {event} e ON n.nid = e.nid INNER JOIN {event_timezones} tz ON tz.timezone = e.timezone WHERE n.status = 1 AND ((e.event_start >= '%s' AND e.event_start <= '%s') OR (e.event_end >= '%s' AND e.event_end <= '%s') OR (e.event_start <= '%s' AND e.event_end >= '%s')) GROUP BY n.nid HAVING (event_start >= '%s' AND event_start <= '%s') OR (event_end >= '%s' AND event_end <= '%s') OR (event_start <= '%s' AND event_end >= '%s')";

  switch ($order) {
    case 'DESC':
      $query .= " ORDER BY event_start DESC";
      break;
    case 'ASC':
      $query .= " ORDER BY event_start ASC";
      break;
  }

  $timezone = (isset($user->timezone) ? $user->timezone : 0);
  return db_query(db_rewrite_sql($query, 'n', 'nid', array_merge(array('event' => TRUE, 'event_get_events' => TRUE, 'event_get_events_user' => TRUE), $rewrite_parameter)), $timezone, $timezone, $timezone, $timezone, $shift, $shift, $before_first, $after_last, $before_first, $after_last, $before_first, $after_last, $first, $last, $first, $last, $first, $last);
}

/**
 * @param $first start date
 * @param $last end date
 * @param Whether to order the result set on the starting date.
 *        Valid values: FALSE, 'DESC', 'ASC'
 * @param $rewrite_parameter optional array that is merged into the 
 *                      fourth argument of db_rewrite_sql.
 *
 * @return database resource
 */
function event_get_events_site($first, $last, $order, $rewrite_parameter = array()) {
  $shift = variable_get('date_default_timezone', 0);
  $before_first = event_implode_date(event_date_later(event_explode_date($first), -1));
  $after_last = event_implode_date(event_date_later(event_explode_date($last), 1));

  $query = "SELECT n.nid, n.title, n.type, e.event_start AS event_start_orig, e.event_end AS event_end_orig, e.timezone, e.has_time, e.has_end_date, tz.\"offset\", tz.offset_dst, tz.dst_region, tz.is_dst, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_start_utc, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_end_utc, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_user, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_user, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_site, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_site, tz.name as timezone_name FROM {node} n INNER JOIN {event} e ON n.nid = e.nid INNER JOIN {event_timezones} tz ON tz.timezone = e.timezone WHERE n.status = 1 AND ((e.event_start >= '%s' AND e.event_start <= '%s') OR (e.event_end >= '%s' AND e.event_end <= '%s') OR (e.event_start <= '%s' AND e.event_end >= '%s')) GROUP BY n.nid HAVING (event_start >= '%s' AND event_start <= '%s') OR (event_end >= '%s' AND event_end <= '%s') OR (event_start <= '%s' AND event_end >= '%s')";

  switch ($order) {
    case 'DESC':
      $query .= " ORDER BY event_start DESC";
      break;
    case 'ASC':
      $query .= " ORDER BY event_start ASC";
      break;
  }

  $timezone = (isset($user->timezone) ? $user->timezone : 0);
  return db_query(db_rewrite_sql($query, 'n', 'nid', array_merge(array('event' => TRUE, 'event_get_events' => TRUE, 'event_get_events_site' => TRUE), $rewrite_parameter)), $timezone, $timezone, $shift, $shift, $shift, $shift, $before_first, $after_last, $before_first, $after_last, $before_first, $after_last, $first, $last, $first, $last, $first, $last);
}

/**
 * @param $first start date
 * @param $last end date
 * @param Whether to order the result set on the starting date.
 *        Valid values: FALSE, 'DESC', 'ASC'
 * @param $rewrite_parameter optional array that is merged into the 
 *                      fourth argument of db_rewrite_sql.
 *
 * @return database resource
 */
function event_get_events_event($first, $last, $order, $rewrite_parameter = array()) {
  global $user;
  $shift = variable_get('date_default_timezone', 0);
  $before_first = event_implode_date(event_date_later(event_explode_date($first), -1));
  $after_last = event_implode_date(event_date_later(event_explode_date($last), 1));

  $query = "SELECT n.nid, n.title, n.type, e.event_start, e.event_end, e.timezone, e.has_time, e.has_end_date, tz.\"offset\", tz.offset_dst, tz.dst_region, tz.is_dst, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_start_utc, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END AS event_end_utc, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_user, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_user, e.event_start - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_start_site, e.event_end - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds' AS event_end_site, tz.name as timezone_name FROM {node} n INNER JOIN {event} e ON n.nid = e.nid INNER JOIN {event_timezones} tz ON tz.timezone = e.timezone WHERE n.status = 1 AND ((e.event_start >= '%s' AND e.event_start <= '%s') OR (e.event_end >= '%s' AND e.event_end <= '%s') OR (e.event_start <= '%s' AND e.event_end >= '%s')) GROUP BY n.nid,n.title, n.type, e.event_start, e.event_end,e.timezone, e.has_time, e.has_end_date, tz.\"offset\",tz.offset_dst, tz.dst_region, tz.is_dst, e.event_start,tz.name HAVING (event_start >= '%s' AND event_start <= '%s') OR (event_end >= '%s' AND event_end <= '%s') OR (event_start <= '%s' AND event_end >= '%s')";

  switch ($order) {
    case 'DESC':
      $query .= " ORDER BY event_start DESC";
      break;
    case 'ASC':
      $query .= " ORDER BY event_start ASC";
      break;
  }

  $timezone = (isset($user->timezone) ? $user->timezone : 0);
  return db_query(db_rewrite_sql($query, 'n', 'nid', array_merge(array('event' => TRUE, 'event_get_events' => TRUE, 'event_get_events_event' => TRUE), $rewrite_parameter)), $timezone, $timezone, $shift, $shift, $before_first, $after_last, $before_first, $after_last, $before_first, $after_last, $first, $last, $first, $last, $first, $last);
}

/**
 * @param $time current date
 * @param $limit number of returned events
 * @param $types The node types to get. If none given, all
 *          event-enabled types will be retrieved that are configured to show
 *          up in all calendars.
 * @param $rewrite_parameter optional array that is merged into the 
 *                      fourth argument of db_rewrite_sql.
 *
 * @return database resource
 */
function event_get_events_upcoming($time, $types, $limit, $rewrite_parameter = array()) {
  $placeholders = implode(',', array_fill(0, count($types), "'%s'"));
  $query = "SELECT n.nid, n.title, n.type, n.status, n.changed, e.event_start, e.event_end, e.timezone, e.has_time, e.has_end_date, extract('day' from event_start - '%s')*24 + cast(substring(cast(event_start - '%s' as varchar) from ' (..):') as integer) || substring(cast(event_start - '%s' as varchar) from ' ..(:..:..)') as time_left, 0 AS days_left FROM {node} n INNER JOIN {event} e ON n.nid = e.nid WHERE n.status = 1 AND (((e.event_start <> e.event_end) AND ('%s' >= e.event_start AND '%s' < e.event_end)) OR ((e.event_start = e.event_end) AND ('%s' <= e.event_start + '%d seconds')) OR e.event_start >= '%s') AND n.type IN ($placeholders) ORDER BY event_start";
  return db_query_range(db_rewrite_sql($query, 'n', 'nid', array_merge(array('event' => TRUE, 'event_get_events' => TRUE, 'event_get_events_upcoming' => TRUE, 'event_types' => $types), $rewrite_parameter)), array_merge(array($time, $time, $time, $time, $time, $time, $time, $time, (60 * 60 * 2), $time), $types), 0, $limit);
}

/**
 * @see event_where
 * @ingroup event_support
 * @param $type either 'start' or 'end'
 */
function event_where_user($type = 'start') {
  global $user;

  switch ($type) {
    case 'end':
    case 'start':
      $timezone = (isset($user->timezone) ? $user->timezone : 0);
      return str_replace('%d', (int) $timezone, "(e.event_$type - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds')");
  }
}

/**
 * @see event_where
 * @ingroup event_support
 * @param $type either 'start' or 'end'
 */
function event_where_utc($type = 'start') {
  global $user;

  switch ($type) {
    case 'end':
    case 'start':
      return "(e.event_$type - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END)";
  }
}

/**
 * @see event_where
 * @ingroup event_support
 * @param $type either 'start' or 'end'
 */
function event_where_site($type = 'start') {
  switch ($type) {
    case 'end':
    case 'start':
      return str_replace('%d', (int) variable_get('date_default_timezone', 0), "(e.event_$type - CASE WHEN tz.is_dst = 1 THEN tz.offset_dst ELSE tz.\"offset\" END + INTERVAL '%d seconds')");
  }
}

/**
 * @see event_where
 * @ingroup event_support
 * @param $type either 'start' or 'end'
 */
function event_where_event($type = 'start') {
  global $user;

  switch ($type) {
    case 'end':
    case 'start':
      return "e.event_$type";
  }
}
