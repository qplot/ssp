<?php
// $Id: biblio.inc,v 1.6 2008/04/06 23:08:26 agentken Exp $

/**
 * @file
 * Handles bibliographies created by the Biblio module
 *
 * INSTALLATION
 *
 * Requires: biblio.module
 * Directory: mysite/plugins/types
 * Settings: No
 *
 * Author: csc4
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Biblio module must be enabled for this plugin to register.
 */
function mysite_type_biblio($get_options = TRUE) {
  if (module_exists('biblio')) {
    $type = array(
      'name' => t('Bibliographies'),
      'description' => t('<b>Bibliographies</b>: All bibliographies from a specific user'),
      'include' => 'biblio',
      'prefix' => t(''),
      'suffix' => t('bibliographies'),
      'category' => t('Content'),
      'weight' => -10,
      'form' => FALSE,
      'label' => t('Add Bibliography'),
      'help' => t('You can choose bibliography postings by specific users. Type a user name in the search box, or choose from a list of active contributers.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_bilbio_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_biblio_options();
    }
    return $type;
  }
}


/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_biblio_active($type) {
  // there must be active and allowed bibliographies
  $value = TRUE;
  $message = '';
  $br = '';
  // some users must be allowed to creation bibliographies, otherwise, give a configuration message
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (!stristr($check, 'create biblio')) {
    $value = FALSE;
    $message = l(t('There are no users with permission to create bibliographies.'), 'admin/user/access');
    $br = '<br />';
  }
  $count = db_result(db_query("SELECT COUNT(nid) FROM {node} WHERE type = 'biblio'"));
  if ($count < 1) {
    $value = FALSE;
    $message = $br . l(t('At least one bibliography must exist.'), 'node/add/biblio');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_biblio_options() {
  $options = array();
  // can't respect biblio permissions because authenticated users aren't in the users_roles table
  // but it only pulls users who have bibliographies.
  $sql = "SELECT DISTINCT(u.uid), u.name, n.nid FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'biblio' ORDER BY n.nid DESC";
  $result = db_query($sql);
  $biblios = array();
  while ($item = db_fetch_object($result)) {
    $biblios[] =$item;
  }
  foreach ($biblios as $key => $value) {
    $options['name'][] = mysite_type_biblio_title($value->uid, $value->name);
    $options['type_id'][] = $value->uid;
    $options['type'][] = 'biblio';
    $options['icon'][] = mysite_get_icon('biblio', $term->tid);
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_biblio_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $biblio = user_load(array('uid' => $type_id));
      $title = $biblio->name;
    }
    $type = mysite_type_biblio(FALSE);
    $title = $type['prefix'] .' '. $title . t("'s") .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_biblio_data($type_id = NULL) {
  if (!empty($type_id)) {
    $sql = "SELECT nid, created FROM {node} WHERE type = 'biblio' AND status = 1 AND uid = %d ORDER BY created DESC";
    $result = db_query($sql, $type_id);
    $data = array(
      'base' => 'biblio/'. $type_id,
      'xml' => 'biblio/'. $type_id .'/feed',
      );
    $items = array();
    $i = 0;
    $type = mysite_type_bilbio(FALSE);
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
function mysite_type_biblio_block($arg, $op = 'view') {
  global $user;
  if (user_access('edit mysite') && ($arg[0] == 'biblio' || $arg[0] == 'user') && is_numeric($arg[1])) {
    $biblio = user_load(array('uid' => $arg[1]));
    if (user_access('edit own bibliography', $biblio)) {
      $data = array();
      $data['uid'] = $user->uid;
      $data['type'] = 'biblio';
      $data['type_id'] = $biblio->uid;
      $content = mysite_block_handler($data);
      return $content;
    }
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_biblio_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_biblio_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_biblio_search()
 */
function mysite_type_biblio_search_form($uid) {
  $form['add_biblio']['biblio_title'] = array('#type' => 'textfield',
    '#title' => t('User Name'),
    '#maxlength' => 64,
    '#description' => t('The user name of the author of the bibliography you wish to add.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/biblio'
  );
  $form['add_biblio']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_biblio']['type'] = array('#type' => 'hidden', '#value' => 'biblio');
  $form['add_biblio']['submit'] = array('#type' => 'submit', '#value' => t('Add biblio'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_biblio_search_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'biblio' AND u.name LIKE LOWER('%s%%')", $form_values['biblio_title']);
  $count = 0;
  while ($biblio = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $biblio->uid;
    $data[$count]['title'] = mysite_type_biblio_title($biblio->uid, $biblio->name);
    $data[$count]['description'] = t('The bibliographies of %user', array('%user' => $biblio->name));
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'biblio');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_biblio_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'biblio' AND u.name LIKE LOWER('%s%%')", $string, 0, 10);
  while ($biblio = db_fetch_object($result)) {
    $matches[$biblio->name] = check_plain($biblio->name);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_biblio_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT DISTINCT(u.uid) FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND n.type = 'biblio' AND u.uid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id'], $implode));
    if (empty($check->uid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}
