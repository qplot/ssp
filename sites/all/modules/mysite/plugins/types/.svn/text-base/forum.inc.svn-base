<?php
// $Id: forum.inc,v 1.17 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles forum threads.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Forum module must be enabled for this plugin to register.
 */
function mysite_type_forum($get_options = TRUE) {
  if (module_exists('forum')) {
    $type = array(
      'name' => t('Forums'),
      'description' => t('<b>Forum Topics</b>: Forum posts within a specific topic.'),
      'include' => 'forum',
      'prefix' => t(''),
      'suffix' => t('forum posts'),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Forum Topic'),
      'help' => t('You can track posts within a forum. Type a keyword to search for matching forum topics, or select from the list provided.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_forum_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_forum_options();
    }
  return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_forum_active($type) {
  // there must be forums
  $tree = taxonomy_get_tree(_forum_get_vid());
  $value = TRUE;
  $message = '';
  $br = '';
  if (empty($tree)) {
    $value = FALSE;
    $message = l('There are no no existing containers or forums.', 'admin/content/forum');
    $br = '<br />';
  }
  // some users must be allowed to user forums
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (!stristr($check, 'create forum topics') && !stristr($check, 'edit own forum topics')) {
    $value = FALSE;
    $message .= $br . l(t('There are no users with create or edit forum permissions.'), 'admin/user/access');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_forum_options() {
  $options = array();
  $forum = variable_get('forum_nav_vocabulary', 0);
  $forums = taxonomy_get_tree($forum);
  $key = '';
  foreach ($forums as $term) {
    // filter the containers
    if ($term->parents[0] == 0) {
      $key = $term->name;
    }
    $depth = str_repeat('-', $term->depth) .' ';
    $options['group'][] = $key;
    $options['name'][] = mysite_type_forum_title($term->tid, $depth . $term->name);
    $options['type_id'][] = $term->tid;
    $options['type'][] = 'forum';
    $options['icon'][] = mysite_get_icon('forum', $term->tid);
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_forum_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $forum = taxonomy_get_term($type_id);
      $title = $forum->name;
    }
    $type = mysite_type_forum(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find forum title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_forum_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    $forum = variable_get('forum_nav_vocabulary', 0);
    $terms = taxonomy_get_tree($forum, $type_id);
    $term_list = array($type_id);
    foreach ($terms as $term) {
      $term_list[] = $term->tid;
    }
    $implode = "(". implode(", ", $term_list) .")";
    $sql = db_rewrite_sql("SELECT n.nid, n.changed, t.tid FROM {node} n INNER JOIN {term_node} t ON n.nid = t.nid WHERE n.status = 1 AND n.type = 'forum' AND t.tid IN %s ORDER BY n.changed DESC");
    $result = db_query_range($sql, $implode, 0, variable_get('mysite_elements', 5));

    $data = array(
      'base' => 'forum/'. $type_id,
      'xml' => 'forum/'. $type_id .'/feed'
      );
    $items = array();
    $i = 0;
    $type = mysite_type_forum(FALSE);
    while ($nid = db_fetch_object($result)) {
      $node = node_load($nid->nid);
      $term = taxonomy_get_term($nid->tid);
      $items[$i]['type'] = $node->type;
      $items[$i]['link'] = l($node->title, 'node/'. $nid->nid, array('target' => $type['link_target']));
      $items[$i]['title'] = check_plain($node->title);
      if (count($term_list) > 1) {
        $items[$i]['subtitle'] = l(mysite_type_forum_title($term->tid, $term->name), 'forum/'. $term->tid);
      }
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
  drupal_set_message(t('Could not find forum data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_forum_block($arg, $op = 'view') {
  global $user;
  if (user_access('edit mysite') && $arg[0] == 'forum' && is_numeric($arg[1])) {
    $forum = taxonomy_get_term($arg[1]);
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'forum';
    $data['type_id'] = $forum->tid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_forum_block_node($nid, $type) {
  if ($type == 'forum') {
    global $user;
    $sql = db_rewrite_sql("SELECT t.tid, t.name FROM {term_data} t INNER JOIN {forum} f ON f.tid = t.tid WHERE f.nid = %d", 't', 'tid');
    $forum = db_fetch_object(db_query($sql, $nid));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = $type;
    $data['type_id'] = $forum->tid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_forum_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_forum_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_forum_search
 */
function mysite_type_forum_search_form($uid) {
  $form['add_forum']['forum_title'] = array('#type' => 'textfield',
    '#title' => t('Forum title'),
    '#default_value' => $edit['forum_title'],
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The name of the forum you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/forum'
  );
  $form['add_forum']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_forum']['type'] = array('#type' => 'hidden', '#value' => 'forum');
  $form['add_forum']['submit'] = array('#type' => 'submit', '#value' => t('Add Forum Topic'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_forum_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $forum = variable_get('forum_nav_vocabulary', 0);
  $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE LOWER(t.name) LIKE LOWER('%s%%') AND t.vid = %d", 't', 'tid');
  $result = db_query($sql, $form_values['forum_title'], $forum);
  $count = 0;
  while ($forum = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $forum->tid;
    $data[$count]['title'] = mysite_type_forum_title($forum->tid, $forum->name);
    $data[$count]['description'] = $forum->description;
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'forum');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_forum_autocomplete($string) {
  $matches = array();
  $forum = variable_get('forum_nav_vocabulary', 0);
  $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE LOWER(t.name) LIKE LOWER('%s%%') AND t.vid = %d", 't', 'tid');
  $result = db_query_range($sql, $string, $forum, 0, 10);
  while ($forum = db_fetch_object($result)) {
    $matches[$forum->name] = check_plain($forum->name);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_forum_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT tid FROM {term_data} WHERE tid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->tid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}
