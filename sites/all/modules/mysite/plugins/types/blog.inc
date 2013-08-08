<?php
// $Id: blog.inc,v 1.23 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles blog posts by individual users.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Blog module must be enabled for this plugin to register.
 */
function mysite_type_blog($get_options = TRUE) {
  if (module_exists('blog')) {
    $type = array(
      'name' => t('Blogs'),
      'description' => t('<b>Blog Posts</b>: Blog posts from a specific user.'),
      'include' => 'blog',
      'prefix' => t(''),
      'suffix' => t('blog posts'),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add User Blog'),
      'help' => t('You can choose blog postings by specific users. Type a user name in the search box, or choose from the list of active bloggers.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_blog_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_blog_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_blog_active($type) {
  // some users must be allowed to blog, otherwise, give a configuration message
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (stristr($check, 'edit own blog')) {
    return array($type => TRUE);
  }
  else {
    return array($type => FALSE, 'message' => l(t('There are no users with blog permissions.'), 'admin/user/access'));
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_blog_options() {
  $options = array();
  // can't respect blog permissions because authenticated users aren't in the users_roles table
  // but it only pulls users who have blogged.
  $sql = "SELECT DISTINCT(u.uid), u.name, u.picture FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'blog' ORDER BY u.name";
  $result = db_query($sql);
  $blogs = array();
  while ($item = db_fetch_object($result)) {
    $blogs[] = $item;
  }
  foreach ($blogs as $key => $value) {
    $options['name'][] = mysite_type_blog_title($value->uid, $value->name);
    $options['type_id'][] = $value->uid;
    $options['type'][] = 'blog';
    $icon = mysite_get_icon('blog', $value->uid);
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
function mysite_type_blog_title($type_id = NULL, $title = NULL) {
  if (is_numeric($type_id)) { // user id 0 is allowed
    if ($type_id == 0) {
      $title = variable_get('anonymous', 'anonymous user');
    }
    if (is_null($title)) {
      $blog = user_load(array('uid' => $type_id));
      $title = $blog->name;
    }
    $type = mysite_type_blog(FALSE);
    $title = $type['prefix'] .' '. $title . t("'s") .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find blog title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_blog_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {

    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n WHERE n.type = 'blog' AND n.status = 1 AND n.uid = %d ORDER BY n.changed DESC");
    $result = db_query_range($sql, $type_id, 0, variable_get('mysite_elements', 5));

    $data = array(
      'base' => 'blog/'. $type_id,
      'xml' => 'blog/'. $type_id .'/feed',
      );
    $items = array();
    $i = 0;
    $type = mysite_type_blog(FALSE);
    while ($nid = db_fetch_object($result)) {
      $node = node_load($nid->nid);
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
  drupal_set_message(t('Could not find blog data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_blog_block($arg, $op = 'view') {
  global $user;
  if (user_access('access content') && ($arg[0] == 'blog' || $arg[0] == 'user') && is_numeric($arg[1]) && user_access('edit own blog', user_load(array('uid' => $arg[1])))) {
    $blog = user_load(array('uid' => $arg[1]));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'blog';
    $data['type_id'] = $blog->uid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_blog_block_node($nid, $type) {
  if ($type == 'blog') {
    global $user;
    $sql = "SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.nid = %d AND n.status = 1 AND n.type = '%s'";
    $blog = db_fetch_object(db_query($sql, $nid, $type));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = $type;
    $data['type_id'] = $blog->uid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_blog_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_blog_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_blog_search()
 */
function mysite_type_blog_search_form($uid) {
  $form['add_blog']['blog_title'] = array('#type' => 'textfield',
    '#title' => t('Search by user name'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The user name of the blog you wish to add.'),
    '#required' => FALSE, // this must be false, since all the search for are the same form
    '#autocomplete_path' => 'autocomplete/mysite/blog'
  );
  $form['add_blog']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_blog']['type'] = array('#type' => 'hidden', '#value' => 'blog');
  $form['add_blog']['submit'] = array('#type' => 'submit', '#value' => t('Add User Blog'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_blog_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query(db_rewrite_sql("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'blog' AND LOWER(u.name) LIKE LOWER('%s%%')"), $form_values['blog_title']);
  $count = 0;
  while ($blog = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $blog->uid;
    $data[$count]['title'] = mysite_type_blog_title($blog->uid, $blog->name);
    $data[$count]['description'] = t('The blog posts of @user', array('@user' => $blog->name));
    $count++;
  }
  mysite_search_handler($data, 'blog');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_blog_autocomplete($string) {
  $matches = array();
  $result = db_query_range(db_rewrite_sql("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'blog' AND LOWER(u.name) LIKE LOWER('%s%%')"), $string, 0, 10);
  while ($blog = db_fetch_object($result)) {
    $matches[$blog->name] = check_plain($blog->name);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_blog_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT DISTINCT(u.uid) FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'blog' AND u.uid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->uid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}
