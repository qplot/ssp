<?php
// $Id: term.inc,v 1.23 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles posts by category.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Taxonomy module must be enabled for this plugin to register.
 */
function mysite_type_term($get_options = TRUE) {
  if (module_exists('taxonomy')) {
    $type = array(
      'name' => t('Categories'),
      'description' => t('<b>Categories</b>: Content assigned to a specific taxonomy term.'),
      'include' => 'term',
      'prefix' => t(''),
      'suffix' => t('articles'),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Categories'),
      'help' => t('You can track content by category. Type a keyword in the search box to find the matching category, or choose from the list provided.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_term_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_term_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_term_active($type) {
  // there must be active and allowed taxonomies
  $value = TRUE;
  $message = '';
  $br = '';
  $tree = taxonomy_get_vocabularies();
  if (empty($tree)) {
    $value = FALSE;
    $message = l(t('There are no categories defined for the site.'), 'admin/content/taxonomy');
    $br = '<br />';
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_term_options() {
  $options = array();
  $default = variable_get('mysite_term_vocabularies', array(1));
  if ($default[0] == 1) {
    $temp = taxonomy_get_vocabularies();
    foreach ($temp as $vocab) {
      $vocabularies[] = $vocab->vid;
    }
  }
  else {
    $vocabularies = $default;
  }
  if (is_array($vocabularies) && !empty($vocabularies)) {
    foreach ($vocabularies as $vid) {
      if ($vid > 0) {
        $v = taxonomy_get_vocabulary($vid);
        $terms[$v->name] = taxonomy_get_tree($v->vid);
      }
    }
  }
  if (is_array($terms) && !empty($terms)) {
    foreach ($terms as $key => $value) {
      foreach ($value as $term) {
        $depth = str_repeat('-', $term->depth) .' ';
        $options['group'][] = $key;
        $options['name'][] = mysite_type_term_title($term->tid, $depth . $term->name);
        $options['type_id'][] = $term->tid;
        $options['type'][] = 'term';
        $options['icon'][] = mysite_get_icon('term', $term->tid);
      }
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_term_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $term = taxonomy_get_term($type_id);
      $title = $term->name;
    }
    $type = mysite_type_term(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find term title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_term_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {

    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n INNER JOIN {term_node} t ON n.nid = t.nid WHERE n.status = 1 AND t.tid = %d ORDER BY n.changed DESC");
    $result = db_query_range($sql, $type_id, 0, variable_get('mysite_elements', 5));

    $data = array(
      'base' => 'taxonomy/term/'. $type_id,
      'xml' => 'taxonomy/term/'. $type_id .'/0/feed'
      );

    $items = array();
    $i = 0;
    // we can't use node_view() here, because aggregator doesn't use nodes
    // and we need the same data structure for all content
    $type = mysite_type_term(FALSE);
    while ($nid = db_fetch_object($result)) {
      $node = node_load($nid->nid);
      $items[$i]['type'] = $node->type;
      $items[$i]['link'] = l($node->title, 'node/'. $node->nid, array('target' => $type['link_target']));
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
  drupal_set_message(t('Could not find term data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_term_block($arg, $op = 'view') {
  global $user;
  // filter against the settings
  $default = variable_get('mysite_term_vocabularies', array(1));
  $allow = FALSE;
  if ($default[0] == 1) {
    $allow = TRUE;
  }
  if (user_access('access content') &&  $arg[0] == 'taxonomy' && $arg[1] == 'term' && is_numeric($arg[2])) {
    $term = taxonomy_get_term($arg[2]);
    if ($allow || in_array($term->vid, $default)) {
      $data = array();
      $data['uid'] = $user->uid;
      $data['type'] = 'term';
      $data['type_id'] = $term->tid;
      $content = mysite_block_handler($data);
      return $content;
    }
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_term_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_term_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_term_search
 */
function mysite_type_term_search_form($uid) {
  $form['add_term']['term_title'] = array('#type' => 'textfield',
    '#title' => t('Category'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The content category that you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/term'
  );
  $form['add_term']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_term']['type'] = array('#type' => 'hidden', '#value' => 'term');
  $form['add_term']['submit'] = array('#type' => 'submit', '#value' => t('Add Category'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_term_search_form_submit($form_id, $form_values) {
  // exclude vocabularies not allowed by the site settings
  $in = _mysite_type_term_in();
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE LOWER(t.name) LIKE LOWER('%s%%')", 't', 'tid');
  if (!empty($in)) {
    $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE LOWER(t.name) LIKE LOWER('%s%%') AND t.vid IN ($in)", 't', 'tid');
  }
  $result = db_query($sql, strtolower($form_values['term_title']));
  $count = 0;
  while ($term = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $term->tid;
    $data[$count]['title'] = mysite_type_term_title($term->tid, $term->name);
    $data[$count]['description'] = $term->description;
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'term');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_term_autocomplete($string) {
  $matches = array();
  // exclude vocabularies not allowed by the site settings
  $in = _mysite_type_term_in();
  $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE LOWER(t.name) LIKE LOWER('%s%%')", 't', 'tid');
  if (!empty($in)) {
    $sql = db_rewrite_sql("SELECT t.tid, t.name, t.description FROM {term_data} t WHERE LOWER(t.name) LIKE LOWER('%s%%') AND t.vid IN ($in)", 't', 'tid');
  }
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
 * @return
 * NULL or An IN() modifier for SQL.
 */
function _mysite_type_term_in() {
  $in = NULL;
  $vocabularies = variable_get('mysite_term_vocabularies', array(1));
  if ($vocabularies[0] == 1) {
    return NULL;
  }
  if (is_array($vocabularies) && !empty($vocabularies)) {
    foreach ($vocabularies as $vid) {
      if ($vid > 0) {
        $in .= $vid .', ';
      }
    }
    $in = rtrim($in, ', ');
  }
  return $in;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_term_clear($type) {
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
function mysite_type_term_settings() {
  mysite_check_settings('term');
  $vocabularies = taxonomy_get_vocabularies();
  if (is_array($vocabularies) && !empty($vocabularies)) {
    return drupal_get_form('mysite_type_term_settings_form', $vocabularies);
  }
  else {
    return t('The taxonomy module is not active or there are no vocabularies.  These features cannot be used.');
  }
}

/**
 * FormsAPI for mysite_type_term_settings()
 */
function mysite_type_term_settings_form($vocabularies) {
  foreach ($vocabularies as $vocabulary) {
    $options[$vocabulary->vid] = $vocabulary->name;
  }
  $default = variable_get('mysite_term_vocabularies', array());
  unset($default[0]);
  $form['mysite_term']['mysite_term_vocabularies'] = array(
      '#prefix' => t('<p>Select the vocabularies to display to MySite users.</p><p><em>If no options are selected, then all vocabularies will be made available.</em></p>'),
      '#type' => 'checkboxes',
      '#title' => t('Allowed Vocabularies'),
      '#default_value' => $default,
      '#options' =>  $options,
      '#required' => FALSE,
      '#description' => t('What vocabularies should be displayed to MySite users?')
  );
  return system_settings_form($form);
}
