<?php
// $Id: storylink.inc,v 1.6 2008/04/06 23:08:26 agentken Exp $

/**
 * @file
 * Allows users to add storylinks to MySite collections.
 *
 * INSTALLATION
 *
 * Requires: vote_storylink.module from the Vote_Up-Down module
 * Directory: mysite/plugins/types
 * Settings: No
 *
 * Author: patchak
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Storylink module must be enabled for this plugin to register.
 */
function mysite_type_storylink($get_options = TRUE) {
  if (module_exists('vote_storylink')) {
    $type = array(
      'name' => t('Storylinks'),
      'description' => t('<b>Storylinks</b>: All storylink posts from a specific user'),
      'include' => 'storylink',
      'prefix' => t('News from '),
      'suffix' => t(''),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add User Storylinks'),
      'help' => t('You can choose storylink postings by specific users. Type a user name in the search box, or choose from the list of link contributors.'),
      'search' => TRUE,
    );
    $basic_settings = variable_get('mysite_basic_storylink_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_storylink_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_storylink_options() {
  $options = array();
  $sql = "SELECT DISTINCT(u.uid), u.name, u.picture FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'storylink' ORDER BY u.name";
  $result = db_query($sql);
  $storylinks = array();
  while ($item = db_fetch_object($result)) {
    $storylinks[] =$item;
  }
  foreach ($storylinks as $key => $value) {
    $options['name'][] = mysite_type_storylink_title($value->uid, $value->name);
    $options['type_id'][] = $value->uid;
    $options['type'][] = 'storylink';
    $icon = mysite_get_icon('storylink', $value->uid);
    if (empty($icon)) {
      $icon = array('path' => file_directory_path() .'/'. variable_get('user_picture_path', 'pictures'), 'file' => $value->picture); // this is a special case
    }
    $options['icon'][] = $icon;
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_storylink_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $storylink = user_load(array('uid' => $type_id));
      $title = $storylink->name;
    }
    $type = mysite_type_storylink(FALSE);
    $title = $type['prefix'] .' '. $title . t("") .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_storylink_data($type_id = NULL) {
  if (!empty($type_id)) {
    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n WHERE n.type = 'storylink' AND n.status = 1 AND n.uid = %d ORDER BY n.changed DESC");
    $result = db_query_range($sql, $type_id, 0, variable_get('mysite_elements', 5));
    $data = array(
      'base' => 'storylink/'. $type_id,
      'xml' => 'storylink/'. $type_id .'/feed',
      );
    $items = array();
    $i = 0;
    $type = mysite_type_storylink(FALSE);
    while ($nid = db_fetch_object($result)) {
      $node = node_load(array('nid' => $nid->nid));
      $items[$i]['type'] = $node->type;
      $items[$i]['link'] = l($node->title, 'node/'. $nid->nid, array('target' => $type['link_target']));
      $items[$i]['title'] = check_plain($node->title);
      $items[$i]['subtitle'] = NULL;
      $items[$i]['date'] = $node->changed;
      $items[$i]['uid'] = $node->uid;
      $items[$i]['author'] = check_plain($node->name);
      $items[$i]['teaser'] = mysite_teaser($node);
      $items[$i]['nid'] = $node->nid;
      $i++;
    }
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_storylink_block($arg, $op = 'view') {
  global $user;
  if (user_access('edit mysite') && ($arg[0] == 'storylink' || $arg[0] == 'user') && is_numeric($arg[1])) {
    $storylink = user_load(array('uid' => $arg[1]));
    if (user_access('edit own storylink', $storylink)) {
      $data = array();
      $data['uid'] = $user->uid;
      $data['type'] = 'storylink';
      $data['type_id'] = $storylink->uid;
      $content = mysite_block_handler($data);
      return $content;
    }
  }
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_storylink_block_node($nid, $type) {
  if ($type == 'storylink') {
    global $user;
    $sql = "SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.nid = %d AND n.status = 1 AND n.type = '%s'";
    $storylink = db_fetch_object(db_query($sql, $nid, $type));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = $type;
    $data['type_id'] = $storylink->uid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_storylink_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_storylink_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_storylink_search()
 */
function mysite_type_storylink_search_form($uid) {
  $form['add_storylink']['storylink_title'] = array('#type' => 'textfield',
    '#title' => t('User Name'),
    '#default_value' => $edit['storylink_title'],
    '#maxlength' => 64,
    '#description' => t('The user name of the storylink you wish to add.'),
    '#required' => FALSE, // this must be false, since all the search for are the same form
    '#autocomplete_path' => 'autocomplete/mysite/storylink'
  );
  $form['add_storylink']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_storylink']['type'] = array('#type' => 'hidden', '#value' => 'storylink');
  $form['add_storylink']['submit'] = array('#type' => 'submit', '#value' => t('Add storylink'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_storylink_search_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'storylink' AND u.name LIKE LOWER('%s%%')", $form_values['storylink_title']);
  $count = 0;
  while ($storylink = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $storylink->uid;
    $data[$count]['title'] = mysite_type_storylink_title($storylink->uid, $storylink->name);
    $data[$count]['description'] = t('The storylink posts of %user', array('%user' => $storylink->name));
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'storylink');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_storylink_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'storylink' AND u.name LIKE LOWER('%s%%')", $string, 0, 10);
  while ($storylink = db_fetch_object($result)) {
    $matches[$storylink->name] = check_plain($storylink->name);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_storylink_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT DISTINCT(u.uid) FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'storylink' AND u.uid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->uid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}
