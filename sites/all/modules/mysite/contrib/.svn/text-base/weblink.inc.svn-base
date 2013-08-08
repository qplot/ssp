<?php
// $Id: weblink.inc,v 1.4 2008/04/06 23:08:26 agentken Exp $

/**
 * @file
 * Handles weblink nodes created by the Links module
 *
 * INSTALLATION
 *
 * Requires: links.module AND links_weblink.module
 * Directory: mysite/plugins/types
 * Settings: No
 *
 * Author: suchold-it
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Taxonomy module and Links Weblink module must be enabled for this plugin to register.
 */
function mysite_type_weblink($get_options = TRUE) {
  if (module_exists('taxonomy') && module_exists('links_weblink')) {
    $type = array(
      'name' => t('Web Links'),
      'description' => t('<b>Weblinks</b>: All weblinks from a specific category.'),
      'include' => 'weblink',
      'prefix' => t(''),
      'suffix' => t('weblinks'),
      'weight' => 0,
      'category' => t('Content'),
      'form' => FALSE,
      'label' => t('Add Web Link'),
      'help' => t('Add all web page links in a given category.  You may search for links or select from the list provided.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_weblink_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_weblink_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_weblink_active($type) {
  // there must be active and allowed taxonomies
  $value = TRUE;
  $message = '';
  $br = '';
  $tree = taxonomy_get_vocabularies('weblink');
  if (empty($tree)) {
    $value = FALSE;
    $message = l(t('There are no categories defined for Weblink content.'), 'admin/content/taxonomy');
    $br = '<br />';
  }
  // if categories have been configured, there will be a '1' value in the variable array
  $allowed = variable_get('mysite_weblink_vocabularies', array());
  if (!in_array(1, $allowed)) {
    $value = FALSE;
    $message .= $br . l(t('The MySite Weblink options must be configured.'), 'admin/settings/mysite/type/weblink');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_weblink_options() {
  $options = array();
  $vocabularies = variable_get('mysite_weblink_vocabularies', array());
  if (is_array($vocabularies) && !empty($vocabularies)) {
    foreach ($vocabularies as $vid) {
      if ($vid > 0) {
        $v = taxonomy_get_vocabulary($vid);
        $terms[$v->name] = taxonomy_get_tree($v->vid);
      }
    }
  }
  if (is_array($terms) && !empty($terms)) {
    $i =0;
    foreach ($terms as $key => $value) {
      foreach ($value as $term) {
        $depth = str_repeat('-', $term->depth) .' ';
        $options['group'][] = $key;
        $options['name'][] = $depth . $term->name;
        $options['type_id'][] = $term->tid;
        $options['type'][] = 'weblink';
        $options['icon'][] = mysite_get_icon('weblink', $term->tid);
        $i++;
      }
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_weblink_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $term = taxonomy_get_term($type_id);
      $title = $term->name;
    }
    $type = mysite_type_weblink(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_weblink_data($type_id = NULL) {
  if (!empty($type_id)) {
    $order =  variable_get('mysite_weblink_sort', array());
    if ($order == "click") {
      $order_sql = "ln.clicks";
    }
    else {
      $order_sql = "n.changed";
    }
    $sql = db_rewrite_sql("SELECT n.nid, n.changed, ln.clicks FROM ({node} n, {links_node} ln) INNER JOIN {term_node} t ON n.nid = t.nid AND n.nid = ln.nid WHERE n.status = 1 AND n.type = 'weblink' AND t.tid = %d ORDER BY {$order_sql} DESC");
    $result = db_query($sql, $type_id);

    $data = array(
      'base' => 'links/weblink/'. $type_id,
      'xml' => 'taxonomy/term/'. $type_id .'/0/feed'
      );
    $items = array();
    $i = 0;
    // we don't use the target attribute here, since weblinks provides its own.
    // we can't use node_view() here, because aggregator doesn't use nodes
    // and we need the same data structure for all content
    while ($nid = db_fetch_object($result)) {
      $node = node_load($nid->nid);
      $link = links_get_list('links_weblink', $node);
      $items[$i]['type'] = $node->type;
      $items[$i]['link'] = l($node->title, $link[0]['href'], $link[0]['attributes']);
      $items[$i]['title'] = check_plain($node->title);
      $items[$i]['subtitle'] = $nid->clicks .' '. format_plural($nid->clicks, 'click', 'clicks');
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
function mysite_type_weblink_block($arg, $op = 'view') {
  global $user;
  if (user_access('access content') &&  $arg[0] == 'taxonomy' && $arg[1] == 'weblink' && is_numeric($arg[2])) {
    $term = taxonomy_get_term($arg[2]);
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'weblink';
    $data['type_id'] = $term->tid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_weblink_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_weblink_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_weblink_search()
 */
function mysite_type_weblink_search_form($uid) {
  $form['add_weblink']['weblink_title'] = array('#type' => 'textfield',
    '#title' => t('Category'),
    '#maxlength' => 64,
    '#description' => t('The weblink category you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/weblink'
  );
  $form['add_weblink']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_weblink']['type'] = array('#type' => 'hidden', '#value' => 'weblink');
  $form['add_weblink']['submit'] = array('#type' => 'submit', '#value' => t('Add category'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_weblink_search_form_submit($form_id, $form_values) {
  // exclude vocabularies not allowed by the site settings
  $in = _mysite_type_weblink_in();
  $placeholders = array_fill(0, count($in), "'%s'");
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE t.name LIKE LOWER('%s%%') AND t.vid IN (". implode(',', $placeholders) .")", 't', 'tid', $in);
  $result = db_query($sql, strtolower($form_values['weblink_title']));
  $count = 0;
  while ($term = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $term->tid;
    $data[$count]['title'] = mysite_type_weblink_title($term->tid, $term->name);
    $data[$count]['description'] = $term->description;
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'weblink');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_weblink_autocomplete($string) {
  $matches = array();
  // exclude vocabularies not allowed by the site settings
  $in = _mysite_type_weblink_in();
  $placeholders = array_fill(0, count($in), "'%s'");
  $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE t.name LIKE LOWER('%s%%') AND t.vid IN (". implode(',', $placeholders) .")", 't', 'tid', $in);
  $result = db_query_range($sql, $string, 0, 10);
  while ($term = db_fetch_object($result)) {
    $matches[$term->name] = check_plain($term->name);
  }
  return $matches;
}

/**
 * Helper function for searches
 *
 * Creates an IN() SQL statement based on allowed vocabularies.
 *
 * @return $in
 * An IN() modifier for SQL.
 */
function _mysite_type_weblink_in() {
  $in = array();
  $vocabularies = variable_get('mysite_weblink_vocabularies', array());
  if (is_array($vocabularies) && !empty($vocabularies)) {
    foreach ($vocabularies as $vid) {
      if ($vid > 0) {
        $in[] = $vid;
      }
    }
  }
  return $in;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_weblink_clear($type) {
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

/**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_weblink_settings() {
  $vocabularies = taxonomy_get_vocabularies('weblink');
  unset($vocabularies[$forum]);
  if (is_array($vocabularies) && !empty($vocabularies)) {
    return drupal_get_form('mysite_type_weblink_settings_form', $vocabularies);
  }
  else {
    return t('The taxonomy module is not active or there are no vocabularies.  These features cannot be used.');
  }
}

/**
 * FormsAPI for mysite_type_weblink_settings
 */
function mysite_type_weblink_settings_form($vocabularies) {
  foreach ($vocabularies as $vocabulary) {
    $options[$vocabulary->vid] = $vocabulary->name;
  }
  $form['mysite_weblink']['mysite_weblink_vocabularies'] = array(
      '#type' => 'checkboxes', '#title' => t('Allowed vocabularies'), '#default_value' => variable_get('mysite_weblink_vocabularies', array()),
      '#options' =>  $options,
      '#required' => FALSE,
      '#description' => t('What weblinks vocabularies should be displayed to MySite users?')
  );
  $options2 = array('change' => t('Last change'), 'click' => t('Number of clicks'));
  $form['mysite_weblink']['mysite_weblink_sort'] = array(
      '#type' => 'radios', '#title' => t('Sorting'), '#default_value' => variable_get('mysite_weblink_sort', array()),
      '#options' =>  $options2,
      '#required' => TRUE,
      '#description' => t('How should weblinks be sorted for MySite users?')
  );
  return system_settings_form($form);
}
