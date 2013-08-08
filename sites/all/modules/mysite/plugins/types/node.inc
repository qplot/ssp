<?php
// $Id: node.inc,v 1.12 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles content posted by type.  Provides CCK support.
 *
 * @ingroup mysite_plugins
 */
 
/**
 * Implements mysite_type_hook().
 *
 * Node module must be enabled for this plugin to register.
 */
function mysite_type_node($get_options = TRUE) {
  if (module_exists('node')) {
    $type = array(
      'name' => t('Posts'),
      'description' => t('<b>Post Types</b>: Content posts of a specific type.'),
      'include' => 'node',
      'prefix' => t(''),
      'suffix' => t('posts'),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Posts by Type'),
      'help' => t('You can add recent posts by type to your personal page. Choose from the list of available content types.'),
      'search' => FALSE
    );
    $basic_settings = variable_get('mysite_basic_node_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_node_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_node_active($type) {
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
function mysite_type_node_options() {
  $options = array();
  // we have to get the allowed options and compare them to the stored options
  $default = variable_get('mysite_node_settings', array(1));
  // if this variable is saved as blank, then a 0 => 1 value is present
  if ($default[0] == 1) {
    $content = node_get_types('names');
    $content = array_flip($content);
  }
  else {
    $content = array_flip($default);
    unset($content[0]);
  }
  $types = mysite_get_custom('node', NULL, TRUE);
  if (!empty($content) && !empty($types)) {
    foreach ($types as $type) {
      if (in_array($type->type_key, $content)) {
        $options['name'][] = mysite_type_node_title($type->myid);
        $options['type_id'][] = $type->myid;
        $options['type'][] = 'node';
        $options['icon'][] = mysite_get_icon('node', $type->myid);
      }
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_node_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $data = mysite_get_custom('node', $type_id, FALSE);
      $title = node_get_types('name', $data->type_key);
    }
    $type = mysite_type_node(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find node title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_node_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    $data = mysite_get_custom('node', $type_id, FALSE);
    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n WHERE n.type = '%s' AND n.status = 1 ORDER BY n.changed DESC");
    $result = db_query_range($sql, $data->type_key, 0, variable_get('mysite_elements', 5));

    $data = array(
      'base' => '',
      'xml' => ''
      );
    $items = array();
    $i = 0;
    $type = mysite_type_node(FALSE);
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
  drupal_set_message(t('Could not find node data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block_node().
 *
 * @param $type
 * Node type is an additional parameter here, since this is a special case
 */
function mysite_type_node_block_node($nid, $type) {
  global $user;
  $types = variable_get('mysite_node_settings', array(1));
  if ($types[0] == 1 || in_array($type, $types)) {
    $info = mysite_get_myid('node', $type);
    if (!empty($info)) {
      $data = array();
      $data['uid'] = $user->uid;
      $data['type'] = 'node';
      $data['type_id'] = $info;
      $content = mysite_block_handler($data);
      return $content;
    }
  }
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_node_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT myid FROM {mysite_content} WHERE type = 'node' AND myid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->myid)) {
      $data[$item['mid']] = $item;
    }
  }
  return $data;
}

 /**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_node_settings() {
  mysite_check_settings('node');
  // when this page is configured, we need to establish the data in the {mysite_content} table.
  // note that we do not delete existing entries in case they are ever enabled again.
  $default = variable_get('mysite_node_settings', array());
  // if this variable is saved as blank, then a 0 => 1 value is present
  unset($default[0]);
  // get the content types
  $types = node_get_types();
  ksort($types);
  $options = array();
  foreach ($types as $key => $value) {
    $options[$key] = "<b>$value->name</b>: ". $value->description;
  }
  // check to see which types have been created and create new as needed
  if (!empty($default)) {
    foreach ($default as $key => $value) {
      if (!empty($value)) {
        mysite_get_myid('node', $key);
      }
    }
  }
  $output = drupal_get_form('mysite_type_node_settings_form', $default, $types, $options);
  return $output;
}

/**
 * FormsAPI for mysite_type_node_settings
 */
function mysite_type_node_settings_form($default, $types, $options) {
  $form['mysite_node_settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed content types'),
    '#prefix' => t('<h3>Content types</h3><p>Enabling the options below will allow users to add recent posts of a specific content type to their MySite page.</p><p><em>If no options are selected, then all node types will be available.</em></p>'),
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
function mysite_type_node_in() {
  // we have to get the allowed options and compare them to the stored options
  $default = variable_get('mysite_node_settings', array(1));
  // if all are selected, then array(0 => 1), so skip this routine
  if ($default[0] == 1) {
    return NULL;
  }
  // if this variable is saved as blank, then a 0 => 1 value is present
  $content = array_flip($default);
  unset($content[0]);
  return "('". implode("','", $content) ."')";
}
