<?php
// $Id:

/**
 * @file
 * Handles comments posted on individual nodes
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Comment module must be enabled for this plugin to register.
 */
function mysite_type_comment($get_options = TRUE) {
  if (module_exists('comment')) {
    $type = array(
      'name' => t('Comments'),
      'description' => t('<b>Post Comments</b>: Comments on a specific post.'),
      'include' => 'comment',
      'prefix' => t('Comments on'),
      'suffix' => t(''),
      'category' => t('Content'),
      'weight' => 1,
      'form' => FALSE,
      'label' => t('Add Post Comments'),
      'help' => t('You can choose comments attached to specific posts. Search the list of active posts or browse from the list below.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_comment_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_comment_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_comment_active($type) {
  // some users must be allowed to comment, otherwise, give a configuration message
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (stristr($check, 'post comments')) {
    return array($type => TRUE);
  }
  else {
    return array($type => FALSE, 'message' => l(t('There are no users with commenting permissions.'), 'admin/user/access'));
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_comment_options() {
  $options = array();
  // we are dealing with nodes, so node_access requires db_rewrite_sql here.  See http://drupal.org/node/135378.
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE n.status = 1 ORDER BY n.title");
  $result = db_query($sql);
  $nodes = array();
  while ($item = db_fetch_object($result)) {
    $nodes[] = $item;
  }
  foreach ($nodes as $key => $value) {
    $options['name'][] = mysite_type_comment_title($value->nid, $value->title);
    $options['type_id'][] = $value->nid;
    $options['type'][] = 'comment';
    $icon = mysite_get_icon('comment', $value->uid);
    $options['icon'][] = $icon;
  }
  return $options;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_comment_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    // wrap this query in db_rewrite_sql to preserve acccess rules -- untested
    $sql = db_rewrite_sql("SELECT c.nid, c.subject, c.cid, c.timestamp, c.comment, c.format, c.name, c.uid, n.title FROM {comments} c INNER JOIN {node} n ON n.nid = c.nid WHERE c.nid = %d AND n.status = 1 AND c.status = %d ORDER BY c.timestamp DESC", 'c', 'cid');
    $result = db_query_range($sql, $type_id, COMMENT_PUBLISHED, 0, variable_get('mysite_elements', 5));
    $data = array(
      'base' => 'node/'. $type_id,
      'xml' => 'node/'. $type_id .'/feed',
      );
    $items = array();
    $i = 0;
    $type = mysite_type_comment(FALSE);
    while ($comment= db_fetch_object($result)) {
      $items[$i]['type'] = 'comment';
      $items[$i]['link'] = l($comment->subject, 'node/'. $comment->nid, array('target' => $type['link_target']), array(), NULL, "comment-$comment->cid");
      $items[$i]['title'] = check_plain($comment->title);
      $items[$i]['subtitle'] = NULL;
      $items[$i]['date'] = $comment->timestamp;
      $items[$i]['uid'] = $comment->uid;
      $items[$i]['author'] = check_plain($comment->name);
      $items[$i]['teaser'] = check_markup($comment->comment, $comment->format);
      $items[$i]['nid'] = $comment->nid;
      $i++;
    }
    if (empty($items)) {
      $items[0]['type'] = 'comment';
      $items[0]['title'] = t('No comments found.');
      $items[0]['link'] = t('No comments found.');
    }
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find comment data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_comment_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $node = node_load($type_id);
      $title = $node->title;
    }
    $type = mysite_type_comment(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find comment title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_comment_block_node($nid, $type) {
  // are comments enabled for this node?
  $comment = db_result(db_query("SELECT n.comment FROM {node} n WHERE n.nid = %d", $nid));
  if ($comment > 0) {
    global $user;
    if (user_access('access comments')) {
      $result = db_query(db_rewrite_sql('SELECT n.nid FROM {node} n WHERE n.nid = %d'), $nid);
      $node = db_fetch_object($result);
    }
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'comment';
    $data['type_id'] = $node->nid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_comment_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_comment_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_comment_search
 */
function mysite_type_comment_search_form($uid) {
  $form['add_comment']['comment_title'] = array('#type' => 'textfield',
    '#title' => t('Post title'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The title of the post you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/comment'
  );
  $form['add_comment']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_comment']['type'] = array('#type' => 'hidden', '#value' => 'comment');
  $form['add_comment']['submit'] = array('#type' => 'submit', '#value' => t('Add Post Comments'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_comment_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE LOWER(n.title) LIKE LOWER('%s%%') AND n.status = 1");
  $result = db_query($sql, $form_values['comment_title']);
  $count = 0;
  while ($node = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $node->nid;
    $data[$count]['title'] = mysite_type_comment_title($node->nid, $node->title);
    $data[$count]['description'] = $node->title;
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'comment');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_comment_autocomplete($string) {
  $matches = array();
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n WHERE LOWER(n.title) LIKE LOWER('%s%%') AND n.status = 1");
  $result = db_query_range($sql, $string, 0, 10);
  while ($node = db_fetch_object($result)) {
    $matches[$node->title] = check_plain($node->title);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_comment_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT nid FROM {node} WHERE nid = %d AND comment > 0 AND status = 1";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->nid)) {
      $data[$item['mid']] = $item;
    }
  }
  return $data;
}
