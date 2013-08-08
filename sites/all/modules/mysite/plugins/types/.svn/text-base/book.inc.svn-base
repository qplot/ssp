<?php
// $Id: book.inc,v 1.19 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles pages posted to a book collection.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Book module must be enabled for this plugin to register.
 */
function mysite_type_book($get_options = TRUE) {
  if (module_exists('book')) {
    $type = array(
      'name' => t('Books'),
      'description' => t('<b>Books</b>: Pages from a book collection.'),
      'include' => 'book',
      'prefix' => t(''),
      'suffix' => t('book pages'),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Book Updates'),
      'help' => t('You can track new pages for individual books. Type a book name in the search box, or choose from the list provided.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_book_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_book_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_book_active($type) {
  // some users must be able to create or edit books
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (stristr($check, 'create new books') || stristr($check, 'edit book pages') || stristr($check, 'create book pages')) {
    return array($type => TRUE);
  }
  else {
    return array($type => FALSE, 'message' => l(t('There are no users with create or edit book permissions.'), 'admin/user/access'));
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_book_options() {
  $options = array();
  // we are dealing with nodes, so node_access requires db_rewrite_sql here.  See http://drupal.org/node/135378.
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n INNER JOIN {book} b ON b.nid = n.nid WHERE b.parent =0 AND n.status = 1 AND n.type = 'book' ORDER BY n.title");
  $result = db_query($sql);
  $books = array();
  while ($item = db_fetch_object($result)) {
    $books[] = $item;
  }
  foreach ($books as $key => $value) {
    $options['name'][] = mysite_type_book_title($value->nid, $value->title);
    $options['type_id'][] = $value->nid;
    $options['type'][] = 'book';
    $options['icon'][] = mysite_get_icon('book', $value->nid);
  }
  return $options;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_book_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n INNER JOIN {book} b ON n.nid = b.nid WHERE b.parent = %d OR n.nid = %d AND n.type = 'book' AND n.status = 1 ORDER BY n.changed DESC, b.weight");
    $result = db_query_range($sql, $type_id, $type_id, 0, variable_get('mysite_elements', 5));
    $data = array(
      'base' => 'book/'. $type_id,
      'xml' => 'book/'. $type_id .'/feed',
      );
    $items = array();
    $i = 0;
    $type = mysite_type_book(FALSE);
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
  drupal_set_message(t('Could not find book data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_book_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $book = node_load($type_id);
      $title = $book->title;
    }
    $type = mysite_type_book(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find book title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_book_block_node($nid, $type) {
  if ($type == 'book') {
    global $user;
    $result = db_query(db_rewrite_sql('SELECT n.nid, n.title, b.parent, n.type FROM {node} n INNER JOIN {book} b ON n.vid = b.vid WHERE n.nid = %d'), $nid);
    $book = db_fetch_object($result);
    // if no parent, this is the parent
    if ($book->parent == 0) {
      $book->parent = $book->nid;
    }
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = $type;
    $data['type_id'] = $book->parent;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_book_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_book_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_book_search
 */
function mysite_type_book_search_form($uid) {
  $form['add_book']['book_title'] = array('#type' => 'textfield',
    '#title' => t('Book title'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The name of the book you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/book'
  );
  $form['add_book']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_book']['type'] = array('#type' => 'hidden', '#value' => 'book');
  $form['add_book']['submit'] = array('#type' => 'submit', '#value' => t('Add Book'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_book_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n INNER JOIN {book} b ON b.nid = n.nid WHERE LOWER(n.title) LIKE LOWER('%s%%') AND b.parent = 0");
  $result = db_query($sql, $form_values['book_title'], $book);
  $count = 0;
  while ($book = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $book->nid;
    $data[$count]['title'] = mysite_type_book_title($book->nid, $book->title);
    $data[$count]['description'] = $book->title;
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'book');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_book_autocomplete($string) {
  $matches = array();
  $sql = db_rewrite_sql("SELECT n.nid, n.title FROM {node} n INNER JOIN {book} b ON b.nid = n.nid WHERE LOWER(n.title) LIKE LOWER('%s%%') AND b.parent = 0");
  $result = db_query_range($sql, $string, 0, 10);
  while ($book = db_fetch_object($result)) {
    $matches[$book->title] = check_plain($book->title);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_book_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT b.nid FROM {book} b WHERE b.parent = 0 AND b.nid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->nid)) {
      $data[$item['mid']] = $item;
    }
  }
  return $data;
}
