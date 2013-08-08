<?php
// $Id: user.inc,v 1.14 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Tracks all posts by a specific user.
 *
 * @ingroup mysite_plugins
 */
 
/**
 * Implements mysite_type_hook().
 *
 * Native to MySite, since User module is required.
 */
function mysite_type_user($get_options = TRUE) {
  $type = array(
    'name' => t('Users'),
    'description' => t('<b>User Posts</b>: Content posted by a specific user.'),
    'include' => 'user',
    'prefix' => t(''),
    'suffix' => t('posts'),
    'category' => t('Content'),
    'weight' => 2,
    'form' => FALSE,
    'label' => t('Add User Posts'),
    'help' => t('You can choose to track postings by specific users. Type a user name in the search box, or choose from the list of active contributors.'),
    'search' => TRUE
  );
  $basic_settings = variable_get('mysite_basic_user_settings', array());
  $type = array_merge($type, $basic_settings);
  if ($get_options) {
    $type['options'] = mysite_type_user_options();
  }
  return $type;
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_user_active($type) {
  $default = variable_get('mysite_user_settings', array('2' => 2));
  $br = '';
  // some users must be allowed to access content, otherwise, give a configuration message
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (stristr($check, 'access content')) {
    $value = TRUE;
  }
  else {
    $value = FALSE;
    $message = l(t('There are no users with permission to access content.'), 'admin/user/access');
    $br = '<br />';
  }
  if (array_sum($default) == 0) {
    $value = FALSE;
    $message .= $br . l(t('No user roles are configured for MySite.'), 'admin/settings/mysite/type/user');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_user_options() {
  $options = array();
  $sql = "SELECT DISTINCT(u.uid), u.name, u.picture FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 ORDER BY u.name";
  $result = db_query($sql);
  // filter the list of allowed users by role
  $users = mysite_type_user_allowed($result);
  if (!empty($users)) {
    foreach ($users as $key => $value) {
      $options['name'][] = mysite_type_user_title($value->uid, $value->name);
      $options['type_id'][] = $value->uid;
      $options['type'][] = 'user';
      $icon = mysite_get_icon('user', $value->uid);
      if (empty($icon)) {
        $icon = array('path' => file_directory_path() .'/'. variable_get('user_picture_path', 'pictures'), 'file' => $value->picture); // this is a special case
      }
      $options['icon'][] = $icon;
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_user_title($type_id = NULL, $title = NULL) {
  if (is_numeric($type_id)) { // user id 0 is allowed
    if ($type_id == 0) {
      $title = variable_get('anonymous', 'anonymous user');
    }
    if (is_null($title)) {
      $user = user_load(array('uid' => $type_id));
      $title = $user->name;
    }
    $type = mysite_type_user(FALSE);
    $title = $type['prefix'] .' '. $title . t("'s") .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find user title.'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_user_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {

    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n WHERE n.status = 1 AND n.uid = %d ORDER BY n.changed DESC");
    $result = db_query_range($sql, $type_id, 0, variable_get('mysite_elements', 5));

    $data = array(
      'base' => 'user/'. $type_id,
      'xml' => NULL,
      );
    $items = array();
    $i = 0;
    $type = mysite_type_user(FALSE);
    while ($nid = db_fetch_object($result)) {
      $node = node_load($nid->nid);
      $items[$i]['type'] = $node->type;
      $items[$i]['link'] = l($node->title, 'node/'. $nid->nid, array('target' => $type['link_target']));
      $items[$i]['title'] = check_plain($node->title);
      $items[$i]['subtitle'] = node_get_types('name', $node->type);
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
  drupal_set_message(t('Could not find user data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_user_block($arg, $op = 'view') {
  global $user;
  if (user_access('access user profiles') && ($arg[0] == 'user' && is_numeric($arg[1]))) {
    $owner = user_load(array('uid' => $arg[1]));
    $keys = array_keys($owner->roles);
    $roles = mysite_type_user_rids();
    $new = array_intersect($keys, $roles);
    if (!empty($new)) {
      $data = array();
      $data['uid'] = $user->uid;
      $data['type'] = 'user';
      $data['type_id'] = $owner->uid;
      $content = mysite_block_handler($data);
      return $content;
    }
  }
}

/**
 * Implements mysite_type_hook_block_node().
 */
function mysite_type_user_block_node($nid, $type) {
  global $user;
  $sql = "SELECT DISTINCT(u.uid) FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.nid = %d AND n.status = 1 AND n.uid > 0";
  $data = db_fetch_object(db_query($sql, $nid));
  $owner = user_load(array('uid' => $data->uid));
  $keys = array_keys($owner->roles);
  $roles = mysite_type_user_rids();
  $new = array_intersect($keys, $roles);
  if (!empty($new)) {
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'user';
    $data['type_id'] = $owner->uid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_user_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_user_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_user_search
 */
function mysite_type_user_search_form($uid) {
  $form['add_user']['user_title'] = array('#type' => 'textfield',
    '#title' => t('Search by user name'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The user name of the user you wish to add.'),
    '#required' => FALSE, // this must be false, since all the search for are the same form
    '#autocomplete_path' => 'autocomplete/mysite/user'
  );
  $form['add_user']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_user']['type'] = array('#type' => 'hidden', '#value' => 'user');
  $form['add_user']['submit'] = array('#type' => 'submit', '#value' => t('Add User Posts'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_user_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND LOWER(u.name) LIKE LOWER('%s%%')", $form_values['user_title']);
  // filter the list of allowed users by role
  $users = mysite_type_user_allowed($result);
  $count = 0;
  if (!empty($users)) {
    foreach ($users as $user) {
      $data[$count]['uid'] = $form_values['uid'];
      $data[$count]['type'] = $form_values['type'];
      $data[$count]['type_id'] = $user->uid;
      $data[$count]['title'] = mysite_type_user_title($user->uid, $user->name);
      $data[$count]['description'] = t('The user posts of @user', array('@user' => $user->name));
      $count++;
    }
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'user');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_user_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT DISTINCT(u.uid), u.name FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND LOWER(u.name) LIKE LOWER('%s%%')", $string, 0, 10);
  // filter the list of allowed users by role -- this may cause no returns
  $users = mysite_type_user_allowed($result);
  if (!empty($users)) {
    foreach ($users as $user) {
      $matches[$user->name] = check_plain($user->name);
    }
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_user_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT DISTINCT(u.uid) FROM {users} u INNER JOIN {node} n ON u.uid = n.uid WHERE n.status = 1 AND u.uid = %d";
    $result = db_query($sql, $item['type_id']);
    $users = mysite_type_user_allowed($result);
    $list = array();
    if (!empty($users)) {
      foreach ($users as $user) {
        $list[] = $user->uid;
      }
    }
    if (!in_array($item['type_id'], $list)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}

 /**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_user_settings() {
  mysite_check_settings('user');
  $default = variable_get('mysite_user_settings', array('2' => 2));
  // it is possible to get a 0 element in the array
  unset($default[0]);
  $roles = user_roles();
  return drupal_get_form('mysite_type_user_settings_form', $default, $roles);
}

/**
 * FormsAPI for mysite_type_user_settings
 */
function mysite_type_user_settings_form($default, $roles) {
  $form['mysite_user_settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed user roles'),
    '#prefix' => t('<h3>User posts</h3><p>Enabling the options below will allow users to add recent posts from a specific user to their personal page.  The users available may be restricted to only the roles selected below.</p>'),
    '#default_value' => $default,
    '#options' => $roles,
    '#required' => FALSE,
    '#description' => t('Select the user roles that may be added to a MySite page. <br /><em>NOTE: Adding <b>authenticated user</b> will enable all roles except <b>anonymous user</b>.</em>')
  );
  return system_settings_form($form);
}

/**
 * Helper function to return a list of role ids
 *
 * @return
 * An array of roles allowed by the MySite administrator
 */
function mysite_type_user_rids() {
  $default = variable_get('mysite_user_settings', array('2' => 2));
  // it is possible to get a 0 element in the array
  unset($default[0]);
  $str = '';
  foreach ($default as $key => $value) {
    if ($value > 0) {
      $roles[$key] = $value;
    }
  }
  return $roles;
}

/**
 * Filter the list of users based on allowed roles.
 *
 * Note that role 1 == anonymous users and role 2 == authenticated users
 *
 * @param $result
 *   A database object created by a user query
 *
 * @return $users
 *   An array of users who are allowed, based on role settings
 */
function mysite_type_user_allowed($result) {
  $roles = mysite_type_user_rids();
  // if all users are not enabled, then get an array of users in the allowed roles
  // doing it this way cuts database queries significantly.
  if ($roles[2] != 2) {
    $allowed = array();
    foreach ($roles as $key => $value) {
      if ($key > 2 && $roles[$key] == $value) {
        $data = db_query("SELECT uid FROM {users_roles} WHERE rid = %d", $key);
        while ($check = db_fetch_object($data)) {
          $allowed[] = $check->uid;
        }
      }
    }
  }
  // now check each user ($item) to see if they are allowed
  while ($item = db_fetch_object($result)) {
    $add = FALSE;
    // check anonymous
    if ($roles[1] == 1 && $item->uid == 0) {
      $add = TRUE;
    }
    // check all (authenticated) users
    if ($roles[2] == 2 && $item->uid > 0) {
      $add = TRUE;
    }
    // check by role, since only some roles are allowed
    elseif ($item->uid > 0) {
      if (in_array($item->uid, $allowed)) {
        $add = TRUE;
      }
    }
    if ($add) {
      $users[] = $item;
    }
  }
  return $users;
}
