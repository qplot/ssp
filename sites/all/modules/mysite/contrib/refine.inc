<?php
// $Id: refine.inc,v 1.7 2008/04/06 23:08:26 agentken Exp $

/**
 * @file
 * Addresses http://drupal.org/node/93060
 *
 * INSTALLATION
 *
 * Requires: refine_by_taxo.module
 * Directory: mysite/plugins/types
 * Settings: No
 *
 * Author: agentrickard
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Taxonomy module and Refine_by_taxo module must be enabled for this plugin to register.
 */
function mysite_type_refine($get_options = TRUE) {
  if (module_exists('refine_by_taxo') && module_exists('taxonomy')) {
    $type = array(
      'name' => t('Refined Categories'),
      'description' => t('<b>Refined Categories</b>: Posts that match multiple terms.'),
      'include' => 'refine',
      'prefix' => t(''),
      'suffix' => t('articles'),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Refined Categories'),
      'help' => t('Display posts tagged with multiple categories.'),
      'search' => FALSE
    );
    $basic_settings = variable_get('mysite_basic_refine_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_refine_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_refine_active($type) {
  // there must be an active option
  $check = db_result(db_query("SELECT COUNT(myid) FROM {mysite_content} WHERE type = 'refine'"));
  if ($check) {
    $value = TRUE;
    $message = '';
  }
  else {
    $value = FALSE;
    $message = l(t('You must create at least one term group to activate this plugin.'), 'admin/settings/mysite/type/refine');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_refine_options() {
  $options = array();
  $data = mysite_get_custom('refine', NULL, TRUE);
  // since $all = TRUE, this returns an array
  if (!empty($data)) {
    foreach ($data as $refine) {
      $options['name'][] = mysite_type_refine_title($refine->myid, $refine->title);
      $options['type_id'][] = $refine->myid;
      $options['type'][] = 'refine';
      $options['icon'][] = mysite_get_icon('refine', $refine->myid);
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_refine_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $data = mysite_get_custom('refine', $type_id, FALSE);
      $title = $data->title;
    }
    $type = mysite_type_refine(FALSE);
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
function mysite_type_refine_data($type_id = NULL) {
  if (!empty($type_id)) {
    $tids = '';
    $data = mysite_get_custom('refine', $type_id);
    $terms = explode(',', $data->type_key);
    $result = taxonomy_select_nodes($terms, $operator = 'and', $depth = 0, $pager = FALSE, $order = 'n.changed DESC');
    $nids = array();
    while ($row = db_fetch_object($result)) {
      $nids[] = $row->nid;
    }
    // slice the array to limit results
    $nodes = array_slice($nids, 0, variable_get('mysite_elements', 5));
    $data = array(
      'base' => 'taxonomy/term/'. $data->type_key,
      'xml' => 'taxonomy/term/'. $data->type_key .'/0/feed'
      );
    $items = array();
    $i = 0;
    $type = mysite_type_refine(FALSE)
    // we can't use node_view() here, because aggregator doesn't use nodes
    // and we need the same data structure for all content
    foreach ($nodes as $nid) {
      $node = node_load($nid);
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
  drupal_set_message(t('Could not find data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_refine_block($arg, $op = 'view') {
  global $user;
  // refine-by-taxo uses the syntax /taxonomy/term/1,2,4
  $terms = explode(',', $arg[2]);
  $name = '';
  if (user_access('access content') &&  $arg[0] == 'taxonomy' && $arg[1] == 'term' && count($terms) > 1) {
    foreach ($terms as $term) {
      $taxo[$term] = taxonomy_get_term($term);
      $name .= $taxo[$term]->name .', ';
    }
    $name = rtrim($name, ', ');
    $myid = mysite_get_myid('refine', $arg[2], $name);
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'refine';
    $data['type_id'] = $myid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_refine_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  // the join statement for items using strings for keys.
  $sql = "SELECT md.mid, md.uid, md.type_id, md.title, mc.type_key FROM {mysite_data} md INNER JOIN {mysite_content} mc ON mc.myid = md.type_id WHERE md.type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT tid FROM {term_data} WHERE tid IN (%s)";
    $check = db_fetch_object(db_query($sql, $item['type_key']));
    if (empty($check->tid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}

/**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_refine_settings() {
  mysite_check_settings('refine');
  $data = db_fetch_object(db_query("SELECT myid, title FROM {mysite_content} WHERE type = 'refine' AND myid = %d", intval(arg(5))));
  $output = '';
  if ($data->myid) {
    $output .= drupal_get_form('mysite_type_refine_title_form', $data);
    return $output;
  }
  else {
    $output .= drupal_get_form('mysite_type_refine_add_form');
    $output .= t('<h3>Rename Groups</h3>');
    $output .= t('<p>The following content groups have been created by the Refine-By-Taxo module.</p>');
    $header = array(
      array('data' => t('ID #'), 'field' => 'myid'),
      array('data' => t('Title'), 'field' => 'title'),
      array('data' => t('Terms'), 'field' => 'type_key'),
      array('data' => t('Rename'))
    );
    $limit = 25;
    $sql = "SELECT myid, type_key, title FROM {mysite_content} WHERE type = 'refine'" . tablesort_sql($header);
    $result = pager_query($sql, $limit);
    $rows = array();
    while ($group = db_fetch_object($result)) {
      $rows[] = array($group->myid, l($group->title, 'taxonomy/term/'. $group->type_key), $group->type_key, l(t('Edit name'), 'admin/settings/mysite/type/refine/'. $group->myid));
    }
    if (!empty($rows)) {
      $output .= theme_table($header, $rows);
      $output .= theme('pager', NULL, $limit, 0);
    }
    return $output;
  }
}

/**
 * FormsAPI for adding new content groups
 */
function mysite_type_refine_add_form() {
  // get the vocabularies
  $vocabularies = taxonomy_get_vocabularies();
  $form['refine'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Content Group'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('<p>Select as many terms as you like to create a new content view.  These items will show all content that matches term1 AND term2 AND so forth.</p>')
  );
  $form['refine']['title'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#title' => t('Group Name'),
    '#required' => TRUE
  );
  if (!empty($vocabularies)) {
    foreach ($vocabularies as $vid => $vocabulary) {
      $terms = taxonomy_get_tree($vid);
      if (!empty($terms)) {
        foreach ($terms as $term) {
          $options[$vid][$term->tid] = $term->name;
        }
        $form['refine'][$vid]['terms'] = array(
          '#type' => 'select',
          '#title' => check_plain($vocabulary->name),
          '#default_value' => NULL,
          '#options' =>  $options[$vid],
          '#multiple' => TRUE,
          '#description' => t('Select the vocabulary terms to combine.')
        );
      }
    }
  }
  $form['refine']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new group')
  );
  return $form;
}

/**
 * FormsAPI for changing a refine-by-taxo group name
 *
 * @param $data
 * The id and title of the element from {mysite_content}
 */
function mysite_type_refine_title_form($data) {
  $form = array();
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Group name'),
    '#size' => 40,
    '#required' => TRUE,
    '#default_value' => $data->title
  );
  $form['myid'] = array('#type' => 'value', '#value' => $data->myid);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Change title'),
  );
  return $form;
}


/**
 * FormsAPI for mysite_type_refine_title_form()
 */
function mysite_type_refine_title_form_validate($form_id, $form_values) {
  $sql = "SELECT myid FROM {mysite_content} WHERE lower(title) = '%s' AND type = 'refine' AND myid <> %d";
  $check = db_result(db_query($sql, trim(strtolower($form_values['title'])), $form_values['myid']));
  if ($check) {
    form_set_error($form_values['myid'], t('The title must be unique.'));
  }
}

/**
 * FormsAPI for mysite_type_refine_title_form()
 */
function mysite_type_refine_title_form_submit($form_id, $form_values) {
  $sql = "UPDATE {mysite_content} SET title = '%s' WHERE myid = %d AND type = 'refine'";
  db_query($sql, $form_values['title'], $form_values['myid']);
  drupal_set_message(t('Title changed successfully.'));
  drupal_goto('admin/settings/mysite/type/refine');
}

/**
 * FormsAPI for mysite_type_refine_add_form()
 */
function mysite_type_refine_add_form_validate($form_id, $form_values) {
  $sql = "SELECT myid FROM {mysite_content} WHERE lower(title) = '%s' AND type = 'refine'";
  $check = db_fetch_object(db_query($sql, trim(strtolower($form_values['title']))));
  if ($check->myid > 0) {
    form_set_error($form_values['title'], t('The title must be unique.'));
  }
  else {
    if (count($form_values['terms']) < 2) {
      form_set_error($form_values['terms'], t('You must select at least two terms.'));
    }
    else {
      ksort($form_values['terms']);
      $terms = implode(',', $form_values['terms']);
      $sql = "SELECT myid FROM {mysite_content} WHERE type_key = '%s' AND type = '%s'";
      $data = db_fetch_object(db_query($sql, $terms, 'refine'));
      if ($data->myid > 0) {
        form_set_error($form_values['terms'], t('This content group already exists.'));
      }
    }
  }
}

/**
 * FormsAPI for mysite_type_refine_add_form()
 */
function mysite_type_refine_add_form_submit($form_id, $form_values) {
  ksort($form_values['terms']);
  $terms = implode(',', $form_values['terms']);
  $myid = mysite_get_myid('refine', $terms, $form_values['title']);
  if ($myid > 0) {
    drupal_set_message(t('New content group added.'));
  }
  else {
    drupal_set_message(t('Insert failed'), 'error');
  }
}
