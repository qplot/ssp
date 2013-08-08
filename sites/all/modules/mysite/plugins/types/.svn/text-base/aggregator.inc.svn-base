<?php
// $Id: aggregator.inc,v 1.17 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles categories of RSS/ATOM feeds defined by the Aggregator module.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Aggregator module must be enabled for this plugin to register.
 */
function mysite_type_aggregator($get_options = TRUE) {
  if (module_exists('aggregator')) {
    $type = array(
      'name' => t('Web Headlines'),
      'description' => t('<b>Web Headlines</b>: News items for an Aggregator category.'),
      'include' => 'aggregator',
      'prefix' => t(''),
      'suffix' => t('headlines'),
      'category' => t('Aggregation'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Web Headlines'),
      'help' => t('Some web feeds are combined into categories. You can add an entire category of feeds to your personal page.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_aggregator_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_aggregator_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_aggregator_active($type) {
  // there must be some aggregator categories
  $count = db_result(db_query("SELECT count(cid) FROM {aggregator_category}"));
  if ($count > 0) {
    return array($type => TRUE);
  }
  else {
    return array($type => FALSE, 'message' => l(t('At least one Aggregator category must be created.'), 'admin/content/aggregator/add/category'));
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_aggregator_options() {
  $options = array();
  $sql = "SELECT cid, title FROM {aggregator_category}";
  $result = db_query($sql);
  $aggregators = array();
  while ($item = db_fetch_object($result)) {
    $aggregators[] =$item;
  }
  foreach ($aggregators as $key => $value) {
    $options['name'][] = mysite_type_aggregator_title($value->cid, $value->title);
    $options['type_id'][] = $value->cid;
    $options['type'][] = 'aggregator';
    $options['icon'][] = mysite_get_icon('aggregator', $value->cid);
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_aggregator_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $sql = "SELECT title FROM {aggregator_category} WHERE cid = %d ORDER BY title";
      $aggregator = db_fetch_object(db_query($sql, $type_id));
      $title = $aggregator->title;
    }
    $type = mysite_type_aggregator(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find aggregator title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_aggregator_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {

    $sql = "SELECT cid, title FROM {aggregator_category} WHERE cid = %d";
    $feed = db_fetch_object(db_query($sql, $type_id));

    $data = array(
      'base' => 'aggregator/categories/'. $feed->cid,
      'xml' => 'aggregator/categories/'. $feed->cid .'/feed'
      );

    $sql = "SELECT i.iid, i.fid, i.title, i.link, i.author, i.description, i.timestamp FROM {aggregator_item} i INNER JOIN {aggregator_category_item} c ON c.iid = i.iid WHERE c.cid = %d ORDER BY i.timestamp DESC";
    $result = db_query_range($sql, $feed->cid, 0, variable_get('mysite_elements', 5));
    $items = array();
    $i = 0;
    $type = mysite_type_aggregator(FALSE);
    while ($obj = db_fetch_object($result)) {
      $sql = "SELECT fid, title AS feedtitle FROM {aggregator_feed} WHERE fid = %d";
      $source = db_fetch_object(db_query($sql, $obj->fid));
      $items[$i]['type'] = 'aggregator';
      $items[$i]['link'] = l($obj->title, $obj->link, array('target' => $type['link_target']));
      $items[$i]['title'] = check_plain($obj->title);
      $items[$i]['date'] = $obj->timestamp;
      $items[$i]['uid'] = NULL;
      if (!empty($obj->author)) {
        $items[$i]['author'] = check_plain($obj->author);
      }
      else {
        $items[$i]['author'] = check_plain($source->feedtitle);
      }
      $items[$i]['teaser'] = aggregator_filter_xss($obj->description);
      $items[$i]['nid'] = NULL;
      $i++;
    }
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find aggregator data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_aggregator_block($arg, $op = 'view') {
  global $user;
  if (user_access('access news feeds') && $arg[0] == 'aggregator' && $arg[1] == 'categories' && is_numeric($arg[2])) {
    $sql = "SELECT cid, title FROM {aggregator_category} WHERE cid = %d";
    $aggregator = db_fetch_object(db_query($sql, $arg[2]));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'aggregator';
    $data['type_id'] = $aggregator->cid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_aggregator_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_aggregator_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_aggregator_search()
 */
function mysite_type_aggregator_search_form($uid) {
  $form['add_aggregator']['aggregator_title'] = array('#type' => 'textfield',
    '#title' => t('Headline name'),
    '#default_value' => $edit['aggregator_title'],
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The name of the headlines you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/aggregator'
  );
  $form['add_aggregator']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_aggregator']['type'] = array('#type' => 'hidden', '#value' => 'aggregator');
  $form['add_aggregator']['submit'] = array('#type' => 'submit', '#value' => t('Add Web Headlines'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_aggregator_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT cid, title FROM {aggregator_category} WHERE title LIKE LOWER('%s%%')", $form_values['aggregator_title']);
  $count = 0;
  while ($aggregator = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $aggregator->cid;
    $data[$count]['title'] = mysite_type_aggregator_title($aggregator->cid, $aggregator->title);
    $data[$count]['description'] = t('The aggregator posts of @title', array('@title' => $aggregator->title));
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'aggregator');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_aggregator_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT cid, title FROM {aggregator_category} WHERE title LIKE LOWER('%s%%')", $string, 0, 10);
  while ($aggregator = db_fetch_object($result)) {
    $matches[$aggregator->title] = check_plain($aggregator->title);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_aggregator_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT cid FROM {aggregator_category} WHERE cid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->cid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}
