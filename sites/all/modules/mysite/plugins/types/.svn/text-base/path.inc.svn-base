<?php
// $Id: path.inc,v 1.9 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Creates path alises for MySite pages.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Path module must be enabled for this plugin to register.  Does not provide content.
 */
function mysite_type_path($get_options = TRUE) {
  if (module_exists('path')) {
    $type = array(
      'name' => t('Path'),
      'description' => t('<b>Path Aliases</b>: Creates <em>mysite/USERNAME</em> path aliases for all MySite users.'),
      'include' => 'path',
      'category' => t('Usability'),
      'weight' => 0,
      'form' => FALSE,
      'admin' => TRUE
    );
    if ($get_options) {
      $type['options'] = array();
    }
    return $type;
  }
}

/**
 * Implementation of mysite_type_hook_updated()
 *
 * When the user updates a MySite page, this will create a path alias
 * using the format 'mysite/USERNAME'.  If you wish to delete these
 * aliases after they have been created, you will need to do so using the
 * Path module.
 *
 * Aliases are created when a user updates their MySite page.
 *
 * Note: if a user changes their username, the path does not change.
 *
 * @param $uid
 * The user id of the owner of the MySite page.
 */
function mysite_type_path_updated($uid) {
  $types = variable_get('mysite_content', NULL);
  if (!empty($types['path']) && $uid > 0) {
    $alias = variable_get('mysite_path_rules', 0);
    $sql = "SELECT uid, name FROM {users} WHERE uid = %d";
    $result = db_query($sql, $uid);
    $myuser = db_fetch_object($result);
    $path = "mysite/$myuser->name";
    $check = drupal_lookup_path('alias', $path);
    if ($check == FALSE) {
      path_set_alias("mysite/$myuser->uid/view", $path);
    }
  }
}
