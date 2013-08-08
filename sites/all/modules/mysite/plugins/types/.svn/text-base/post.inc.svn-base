<?php
// $Id: post.inc,v 1.6 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Allows users to add individual nodes to a MySite collection.
 * Original code by heebiejeebieclub.  http://drupal.org/node/165324
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Node module must be enabled for this plugin to register.
 */
function mysite_type_post($get_options = TRUE) {
  if (module_exists('node')) {
    $type = array(
      'name' => t('Post'),
      'description' => t('<b>Posts</b>: A specific, published post.'),
      'include' => 'post',
      'prefix' => t(''),
      'suffix' => t(''),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Post'),
      'help' => t('You can add individual posts to your page. Search the list of active posts or browse from the list provided.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_post_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_post_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_post_active($type) {
  // some users must be allowed to view content, otherwise, give a configuration message
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  $br = '';
  $message = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (stristr($check, 'access content')) {
    $value = TRUE;
  }
  else {
    $value = FALSE;
    $message = l(t('No users have permission to access site content'), 'admin/user/access');
    $br = '<br />';
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_post_options() {
  $options = array();
  // construct the IN clause needed for node types
  $in = mysite_type_post_in();
  // we are dealing with nodes, so node_access requires db_rewrite_sql here. See http://drupal.org/node/135378.
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.status = 1 ORDER BY n.title");
  if (!empty($in)) {
    $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.status = 1 AND n.type IN $in ORDER BY n.title");
  }
  $result = db_query($sql);
  $nodes = array();
  while ($item = db_fetch_object($result)) {
    $nodes[] = $item;
  }
  foreach ($nodes as $key => $value) {
    $options['name'][] = mysite_type_post_title($value->nid, $value->title);
    $options['type_id'][] = $value->nid;
    $options['type'][] = 'post';
    $icon = mysite_get_icon('post', $value->nid);
    $options['icon'][] = $icon;
  }
  return $options;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_post_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    $data = array(
      'base' => 'node/'. $type_id,
      'xml' => ''
    );
    $items = array();
    $i = 0;
    $node = node_load($type_id);
    $type = mysite_type_post(FALSE);
    $items[$i]['type'] = $node->type;
    $items[$i]['link'] = l($node->title, 'node/'. $node->nid, array('target' => $type['link_target']));
    $items[$i]['title'] = check_plain($node->title);
    $items[$i]['subtitle'] = NULL;
    $items[$i]['date'] = $node->changed;
    $items[$i]['uid'] = $node->uid;
    $items[$i]['author'] = check_plain($node->name);
    $items[$i]['teaser'] = mysite_teaser($node);
    $items[$i]['nid'] = $node->nid;
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find post data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_post_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $node = node_load($type_id);
      $title = $node->title;
    }
    $type = mysite_type_post(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find post title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_post_block_node($nid = NULL, $type = NULL) {
  global $user;
  $types = variable_get('mysite_post_settings', array(1));
  if ($types[0] == 1 || in_array($type, $types)) {
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'post';
    $data['type_id'] = $nid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_post_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_post_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_post_search()
 */
function mysite_type_post_search_form($uid) {
  $form['add_post']['post_title'] = array('#type' => 'textfield',
    '#title' => t('Post title'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The title of the post you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/post'
  );
  $form['add_post']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_post']['type'] = array('#type' => 'hidden', '#value' => 'post');
  $form['add_post']['submit'] = array('#type' => 'submit', '#value' => t('Add Post'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_post_search_form_submit($form_id, $form_values) {
  // construct the IN clause needed for node types
  $in = mysite_type_post_in();
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE LOWER(n.title) LIKE LOWER('%s%%') AND n.status = 1");
  if (!empty($in)) {
    $sql = db_rewrite_sql("SELECT n.id, n.title FROM {node} WHERE LOWER(n.title) LIKE LOWER('%s%%') AND n.status = 1 AND n.type IN $in");
  }
  $result = db_query($sql, $form_values['post_title']);
  $count = 0;
  while ($node = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $node->nid;
    $data[$count]['title'] = mysite_type_post_title($node->nid, $node->title);
    $data[$count]['description'] = $node->title;
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'post');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_post_autocomplete($string) {
  $matches = array();
  // construct the IN clause needed for node types
  $in = mysite_type_post_in();
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE LOWER(n.title) LIKE LOWER('%s%%') AND n.status = 1");
  if (!empty($in)) {
    $sql = db_rewrite_sql("SELECT n.id, n.title FROM {node} WHERE LOWER(n.title) LIKE LOWER('%s%%') AND n.status = 1 AND n.type IN $in");
  }
  $result = db_query_range($sql, $string, 0, 10);
  while ($node = db_fetch_object($result)) {
    $matches[$node->title] = check_plain($node->title);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_post_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT nid FROM {node} WHERE nid = %d AND status = 1";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->nid)) {
      $data[$item['mid']] = $item;
    }
  }
  return $data;
}

 /**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_post_settings() {
  mysite_check_settings('post');
  $default = variable_get('mysite_post_settings', array());
  // if this variable is saved as blank, then a 0 => 1 value is present
  unset($default[0]);
  // get the content types
  $types = node_get_types();
  ksort($types);
  $options = array();
  foreach ($types as $key => $value) {
    $options[$key] = "<b>$value->name</b>: ". $value->description;
  }
  $output = drupal_get_form('mysite_type_post_settings_form', $default, $types, $options);
  return $output;
}

/**
 * FormsAPI for mysite_type_node_settings
 */
function mysite_type_post_settings_form($default, $types, $options) {
  $form['mysite_post_settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed content types'),
    '#prefix' => t('<h3>Content types</h3><p>Enabling the options below will allow users to add specific posts to their MySite page.</p><p><em>If no options are selected, then all node types will be available.</em></p>'),
    '#default_value' => $default,
    '#options' => $options,
    '#required' => FALSE,
    '#description' => t('Select the content types that users may select for their MySite page.')
  );
  return system_settings_form($form);
}

/**
 * Helper function for constructing proper IN clause for node types.
 *
 * @return
 * NULL or a formatted ('one', 'two', 'three') string
 */
function mysite_type_post_in() {
  // we have to get the allowed options and compare them to the stored options
  $default = variable_get('mysite_post_settings', array(1));
  // if all are selected, then array(0 => 1), so skip this routine
  if ($default[0] == 1) {
    return NULL;
  }
  // if this variable is saved as blank, then a 0 => 1 value is present
  $content = array_flip($default);
  unset($content[0]);
  return "('". implode("','", $content) ."')";
}
