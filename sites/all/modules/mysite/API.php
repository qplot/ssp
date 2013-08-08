<?php
// $Id: API.php,v 1.23 2008/04/06 23:08:25 agentken Exp $


/**
 * @defgroup mysite_hooks MySite Hooks
 * Internal module hooks used by MySite includes.
 *
 * These hooks are called by invoking:
 * @code mysite_{$type}_{$name}_hook() @endcode
 * This method is used to avoid conflict with Drupal's core hooks and to ensure that only activated
 * include files are loaded and executed by the system.
 *
 * The common method for calling a MySite hook is to use:
 * @code module_invoke('mysite_{$type}', {$name}. '_hook', $args) @endcode
 *
 * MySite contributors and plugin authors do not need to worry about this syntax unless they need
 * to invoke functions from other MySite includes.
 *
 * - NOTE: It is understood that this is a non-standard implementation of the Drupal hook() system.  This code
 *     may be re-factored in later releases.
 */

/**
 * @defgroup mysite_plugins MySite Plugins
 * Files which use the MySite API to add features and functionality.
 *
 * MySite uses four types of plugins:
 * - 'formats' control the display of content on a page.
 * - 'layouts' control the position of content on a page.
 * - 'styles' affect the design of content on a page.
 * - 'types' define the content on a page.
 */

/**
 * @file
 * Documentation file for MySite's internal hooks.
 *
 * @ingroup mysite
 */

/**
 * Basic definition of the content type.
 *
 * This hook is used by MySite Type plugins to define the available content types.
 *
 * This hook is invoked by the following functions:
 * - mysite_menu()
 * - mysite_configure_form()
 * - mysite_content_form()
 * - mysite_content_menu()
 *
 * The function is almost always wrapped inside an IF statement, since MySite plugins
 * require that their "parent" modules be active.
 *
 * Some of these settings can be altered through the UI, so the $basic_settings routine
 * is important.
 *
 * @param $get_options
 *   A boolean flag indicating whether to append content selection options to the $type array. Default is TRUE.
 *   NOTE: Leaving this value blank will invoke mysite_type_hook_options() which can be resource intensive.
 * @return
 *   An array of data that defines this content type.  The data takes the form:
 *   - 'name': String. The display name of the include type. Required.
 *   - 'description': String. The include description to show to the administrator.  Required.
 *   - 'include': String. The filename of the include (without extension).  Required.
 *   - 'prefix': String. A text string to prepend to content item names.  Optional.
 *   - 'suffix': String. A text string to append to content item names.  Optional.
 *   - 'category': String. The plugin category used for sorting content types.  Required. Possible values are:
 *   - -- "Aggregation": Content having to do with feed aggregation.
 *   - -- "Content": Generic value. Default.
 *   - -- "MySite": Content native to the MySite module.
 *   - -- "Usability": Items related to design, themes, or other UI areas.
 *   - 'weight': Numeric. The sort order of an include within a category list.  Required.
 *   - 'form': Boolean. A flag that indicates whether the include uses mysite_type_hook_form().  Required.
 *   - 'label': String. The content link to show to users. Required.
 *   - 'help': String. Help text to show to users that describes this content.  Required.
 *   - 'search': Boolean. A flag indicating that this content can be searched using mysite_type_hook_search().  Required.
 *   - 'admin': Boolean. A flag indicating that this content is only shown on MySite configuration pages.  Required if TRUE.
 *  The order of the array does not matter.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook($get_options = TRUE) {
  if (module_exists('node')) {
    $type = array(
      'name' => t('Hook'),
      'description' => t('<b>Hook</b>: Definition for the MySite configuration page.'),
      'include' => 'hook',
      'prefix' => t(''),
      'suffix' => t(''),
      'category' => t('Content'),
      'weight' => 0,
      'form' => FALSE,
      'label' => t('Add Hook'),
      'help' => t('User help to display on Add Content pages.'),
      'search' => TRUE,
      'admin' => FALSE
    );
    $basic_settings = variable_get('mysite_basic_type_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_hook_options();
    }
    return $type;
  }
}

/**
 * Checks to see if this include can be enabled. Sets a message to the administrator if FALSE.
 *
 * This check is generally a permissions check, but can also check mysite_type_hook_settings() to ensure that
 * an include has been configured.  See mysite_type_term_active() for an example.
 *
 * Invoked by mysite_configure_form().
 *
 * @param $type
 *   The content type of this include.  Passed by mysite_configure_form().
 * @return
 *   array($type => TRUE, 'message' => t('A String'))
 *   An array where $type is the type string, set to a boolean value indicating that the include can be used.
 *   The message element is a string of content to render if the $type returns FALSE.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_active($type) {
  // some users must be allowed to view content, otherwise, give a configuration message
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
    $message = l(t('No users have permission to access site content'), 'admin/user/access');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Sets the content selection options for this type.
 *
 * The plugin must find the active and allowed content options that users may add to their personal MySite page.
 * Typically, this is done by pulling back all id keys for a given category.
 *
 * Invoked by mysite_type_hook().
 *
 * @return
 *   An array of options for this type.  These define the content that can be added to a MySite collection.  The array is
 *   an associative array with [n] levels.  Elements include:
 *   - 'group': String. An name for the parent element of this content.  Optional.  Used with hierarchical content like taxonomies.  See mysite_type_term_options() for an example.
 *   - 'name': String.  The default display name for this content.  Required.  Calls mysite_type_hook_title() in order to add the prefix and suffix values defined by mysite_type_hook().
 *   - 'type_id': Numeric.  This is the native id key for the content, such as the term id (tid) for a taxonomy term. Required.
 *   - 'type': String. The content type, usually identical to the name of the include.  Required.
 *   - 'icon': Special.  Invokes mysite_get_icon() to return the browser icon for this content, if the mysite_icon.module is enabled.  Required. This value will either be a string indicating the filename of a MySite Icon upload or an array indicating the path and filename of another file.  See mysite_type_user_options() for an example.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_options() {
  $options = array();
  $sql = "SELECT m.id, m.title, g.group FROM {mytable} m INNER JOIN {group} g ON m.id = g.id";
  $result = db_query($sql);
  $items = array();
  while ($item = db_fetch_object($result)) {
    $items[] =$item;
  }
  foreach ($items as $key => $value) {
    $options['group'][] = $value->group;
    $options['name'][] = mysite_type_hook_title($value->id, $value->title);
    $options['type_id'][] = $value->id;
    $options['type'][] = 'hook';
    $options['icon'][] = mysite_get_icon('hook', $value->id);
  }
  return $options;
}

/**
 * Returns the title of a content element.
 *
 * This hook has two uses.  First, it lets us return a title element based on a given type and id.  Second, it adds the prefix and suffix strings from mysite_type_hook() to the name element.
 *
 * Invoked by:
 * - mysite_content_add()
 * - mysite_block_handler()
 * - mysite_type_hook_options()
 *
 * @param $type_id
 *   The numeric identifier for this element. Required.
 * @param $title
 *   The title string to display.  Optional. If present, the prefix and suffix from mysite_type_hook() will be added.
 * @return
 *   The prepared title string.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $sql = "SELECT title FROM {mytable} WHERE id = %d ORDER BY title";
      $title = db_result(db_query($sql, $type_id));
    }
    $type = mysite_type_hook(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find type title'), 'error');
  return;
}

/**
 * Returns the data for a requested content element.
 *
 * This is the core MySite page content function.  It is used to return the items to display on a user's personal page.
 * For each content element on the page, this function is invoked, passing the $type_id to retrieve the appropriate content.
 *
 * NOTE: When pulling back node content, be sure to wrap the database call in db_rewrite_sql() in order to respect node_access rules.
 *
 * NOTE: MySite does not use hook_view() for node content because we don't assume that all MySite content elements are nodes.  Since we want the data array to be standardized (and since we have our own Format functions), custom node templates are not invoked.
 *
 * NOTE: For easier theming by the Layout and Format plugins, we do output filter routines on the data within this function.
 *
 * Invoked by mysite_page().
 *
 * @param $type_id
 *   The numeric identifier for this element. Required.  Set to NULL for a logic check.
 * @param $settings
 *    User-specific settings for this content type, if applicable.  Optional.  See profile.inc for an example usage.
 * @return
 *   An array of information about this content element, used to render the output. The array is [n] levels deep (even if n == 1), with an associative array at each level and some 'parent' data at the top of the array.  Returns an error message on failure. The data is returned in the format:
 *   - 'base': The Drupal path or a full URL to the data source.  Used to link to "read more."  Optional.
 *   - 'xml': The Drupal path or a full URL to an XML/RSS/ATOM feed. Not currently implemented.  Optional.
 *   - 'image': An optional image to display, typically an RSS image for a feed.  Optional. See mysite_type_feed_data() for and example.
 *   - [n] 'type': The content type.  Usually the same as the plugin name. Required.
 *   - [n] 'link': A full <a href="path">Title</a> HTML string, usually generated by Drupal's l() function.  Required.
 *   - [n] 'title': The plain-text title of the item. Required. You must use check_plain() here.
 *   - [n] 'subtitle': The plain-text subtitle of the item, typically the source name. Optional. Use check_plain().
 *   - [n] 'date': The UNIX timestamp of the last update for the item. Optional.
 *   - [n] 'uid': The Drupal user id of the item's author.  Optional.
 *   - [n] 'author': The username of the item's author.  Optional. You must use check_plain() here.
 *   - [n] 'teaser': The teaser element to display. Optional.  Node items should use mysite_teaser() to ensure proper filtering.
 *   - [n] 'content': An HTML content element that should be used for the whole content item. Optional.  This feature is used by MySite Droplets to render Block and View content. If the 'content' element is present, it will be printed and other data (like 'teaser') may not. See mysite_type_droplet_data() for an example.
 *   - [n] 'nid': The node id of the item. Optional.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    $sql = db_rewrite_sql("SELECT n.nid, n.changed FROM {node} n INNER JOIN {term_node} t ON n.nid = t.nid WHERE t.nid = %d AND n.status = 1 ORDER BY n.changed DESC");
    $result = db_query_range($sql, $type_id, 0, variable_get('mysite_elements', 5));
    $data = array(
      'base' => '',
      'xml' => '',
      'image' => '',
      );
    $items = array();
    $i = 0;
    $type = mysite_type_hook(FALSE);
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
      $items[$i]['content'] = NULL;
      $items[$i]['nid'] = $node->nid;
      $i++;
    }
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find type data'), 'error');
  return;
}

/**
 * Returns content to add to the MySite block.
 *
 * This function checks the page that the user is on and returns MySite actions appropriate to the current path.  While the return value is an HTML string, the $data array passed to mysite_block_handler() is key here.
 * The $data array takes the format:
 * - 'uid': The user id of the person viewing the page.
 * - 'type': The content type defined by this include.
 * - 'type_id': The id key for this particular page.  That is, if viewing 'taxonomy/term/2', this value is '2'.
 *
 * NOTE: As of 5.x.3, we no longer pass the 'title' element here.  It is checked inside mysite_block_handler().
 *
 * NOTE: The call to mysite_block_handler() is here rather than within mysite_block() in case the initial IF check fails.  Doing so saves us needless function calls.
 *
 * NOTE: In theory, you could add additional HTML to the $content, but mysite_block is expecting to receive the <ul><li>link to action</li></ul> content generated by mysite_block_handler().  Therefore, adding extra HTML is not recommended.
 *
 * NOTE: This function does not handle block content for node pages.  See mysite_type_hook_block_node().
 *
 * See mysite_type_user_block() and mysite_type_term_block() for examples.
 *
 * Invoked by mysite_block().
 *
 * @param $arg
 *   An array generated by mysite_block().  Consists of arg(0) through arg(5).  Prevents calling arg() multiple times during the hook.
 * @param $op
 *    The block operation ($op) being performed.  Always 'view'.  Deprecated.
 * @return
 *    An HTML string to append to the MySite block, formatted by mysite_block_handler().
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_block($arg, $op = 'view') {
  global $user;
  if (user_access('access content') && ($arg[0] == 'mypath' && is_numeric($arg[1]))) {
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'hook';
    $data['type_id'] = $arg[1];
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Returns block content when viewing a node page.
 *
 * Node pages are a special case, since mysite_type_hook_block() assumes that arg(1) is a unique content key.  For nodes, arg(1) is the node id, so we use this hook instead.
 *
 * See mysite_type_hook_block() for details on the $data array passed to mysite_block_handler().
 *
 * Invoked by mysite_block().
 *
 * @param $nid
 *    The node id of the page being viewed.
 * @param $type
 *    The node type of the page being viewed.
 * @return
 *    The content to be displayed by mysite_block(), formatted by mysite_block_handler().
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_block_node($nid, $type) {
  if ($type == 'mynodetype') {
    global $user;
    $id = db_result(db_query("SELECT id FROM {mytable} WHERE nid = %d", $nid));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = $type;
    $data['type_id'] = $id;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Searches options for a given type.
 *
 * Designed to let users search the data provided by mysite_type_hook_options().
 *
 * Invoked by mysite_get_content_element().
 *
 * @param $uid
 *   The user id of the user performing the search.  Value is passed from mysite_get_content_element().
 * @return
 *   A search form generated via FormsAPI using mysite_type_hook_search_form().
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_hook_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_hook_search()
 *
 * This form is designed for simple searching of a content group.  It should have a title element, hidden values for 'uid' and 'type' and a named 'submit' button.
 *
 * The form will be processed by mysite_type_hook_search_form_submit().
 *
 * NOTE: If you need more elaborate forms for special functions, use mysite_type_hook_form().
 *
 * @param $uid
 *   The id of the user performing the search.
 *
 * @return
 *   A form according to FormsAPI.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_search_form($uid) {
  $form['add_hook']['hook_title'] = array('#type' => 'textfield',
    '#title' => t('Search title'),
    '#default_value' => $edit['hook_title'],
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('Search form description.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/hook'
  );
  $form['add_hook']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_hook']['type'] = array('#type' => 'hidden', '#value' => 'hook');
  $form['add_hook']['submit'] = array('#type' => 'submit', '#value' => t('Add Hook Name'));
  return $form;
}

/**
 * Form submit handler for mysite_type_hook_search().
 *
 * The purpose of this function is to find matches to the user's search.  An array of matches is then passed to mysite_search_handler() in order to display the proper options and messages to the user.
 *
 * While function has no return value, it really uses mysite_search_handler() to respond to the form.  This function must pass a $data array that is an associative array that is [n] levels deep.
 * - [n] 'uid': The user id of the person whose MySite page is being updated.
 * - [n] 'type': The content type being searched.
 * - [n] 'type_id': The id key for the content type.
 * - [n] 'title': The title of the content item, derived using mysite_type_hook_title().
 * - [n] 'description': A short description of the content item.
 *
 * Invoked by mysite_type_hook_search_form().
 *
 * @param $form_id
 *   The form_id defined in FormsAPI, passed by mysite_type_hook_search().
 * @param $form_values
 *   An array of data passed by the form according to the FormsAPI.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT id, title FROM {mytable} WHERE title LIKE LOWER('%s%%')", $form_values['hook_title']);
  $count = 0;
  while ($item = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $item->id;
    $data[$count]['title'] = mysite_type_hook_title($item->id, $item->title);
    $data[$count]['description'] = t('Description of @title', array('@title' => $item->title));
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'hook');
  return;
}

/**
 * AJAX autocomplete for aggregator titles.
 *
 * Since plugins don't have their own menu items, we pass all autocomplete requests through
 * mysite_autocomplete, which then invokes this internal hook.
 *
 * See http://drupal.org/node/42552 for additional information about AJAX autocomplete.
 *
 * Invoked by mysite_autocomplete().
 *
 * @param $string
 *   The search string entered by the user.
 * @return
 *   An array of items that matched the search routine.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT id, title FROM {mytable} WHERE title LIKE LOWER('%s%%')", $string, 0, 10);
  while ($data = db_fetch_object($result)) {
    $matches[$data->title] = check_plain($data->title);
  }
  return $matches;
}

/**
 * Cron cleanup function.
 *
 * Since plugins are only included dynamically, we can't put hook_cron() functions directly into type includes.
 * Instead we invoke them using the mysite_type_hook_clear() function.
 *
 * The function should check to see that all type_id keys stored by users are still active in the parent database.  For example,
 * the mysite_type_user_clear() function checks to see that all user ids stored in the {mysite_data} table are still active users
 * of the site.
 *
 * If this function finds no illegal data, it will return (correctly) an empty array.
 *
 * Invoked by mysite_cron().
 *
 * @param $type
 *   The content type passed by mysite_cron().
 * @return
 *   An array of items to delete from all user MySite pages.
 *   The key is the mysite_id (mid) and the value is the corresponding data from {mysite_data}
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT id FROM {mytable} WHERE id = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->id)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}

/**
 * Add a form element distinct to a content type
 *
 * This hook is used when content types need to add a special form to the MySite Content page.  Used specifically by the feed.inc plugin to allow users to add RSS/ATOM feeds.
 * See mysite_type_feed_form() for usage example.
 *
 * The form that you implement should have the normal FormsAPI elements of _form, _validate, and _submit.
 *
 * Invoked by mysite_get_content_element().
 *
 * @param $param
 * You may pass any parameters that can be received by your implementation of mysite_type_hook_custom_form().
 * @return
 * HTML generated by drupal_get_form()
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_form($param) {
  if (user_access('mypermission')) {
    $output = '<p>'. t('User message.') .'</p>';
    $output .= drupal_get_form('mysite_type_hook_custom_form', $param);
  }
  return $output;
}

/**
 * FormsAPI for mysite_type_hook_custom_form()
 *
 * @param $param
 * Any parameters passed by mysite_type_hook_form().
 * @return
 * An array conforming to the FormsAPI
 *
 * @ingroup mysite_hooks
 */
function mysite_type_feed_custom_form($param) {
  $form['new_feed'] = array(
    '#type' => 'textfield',
    '#title' => t('Form element'),
    '#title' => t('Your Field Name'),
    '#size' => 40
  );
  return $form;
}

/**
 * Register changes to MySite page.
 *
 * When the user updates a MySite page, this hook allows type plugins to
 * register any needed changes.  See path.inc for an example.
 *
 * @param
 *   $uid == the user id of the owner of the MySite page.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_updated($uid) {
  if (!empty($uid)) {
    db_query("UPDATE {mytable} SET last_visit = %d WHERE uid = %d", time(), $uid);
  }
}

/**
 * Content settings hook.
 *
 * Allows plugins to add elements to a content configuration form.
 *
 * Invoked by mysite_content_settings_form().
 *
 * @param $data
 *   Array with information from the {mysite_data} table for this given content.
 * @return
 *   An array of elements formatted according to the Drupal FormsAPI. These will be appended to
 *   the main form and do not require submit functions. When naming form elements, be careful
 *   of possible namespace collisions.
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_content_form($data) {
  $extra['myvalue'] = array(
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 80,
    '#title' => 'test'
  );
  return $extra;
}

/**
 * Content settings validation hook.
 *
 * Allows plugins to validate elements added to a content configuration form. See
 * mysite_type_profile_content_form_validate() for an example.
 *
 * Invoked by mysite_content_settings_form_validate().
 *
 * @param $form_values
 *   Array of data passed by mysite_content_settings_form().
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_content_form_validate($form_values) {
  if (empty($form_values['myvalue'])) {
    form_set_error('myvalue', t('myvalue is required'));
  }
}

/**
 * Content settings submit hook.
 *
 * Allows plugins to act on elements added to a content configuration form. See
 * mysite_type_profile_content_form_submit() for an example.
 *
 * It is assumed that these values will be saved as a serialized array in the {mysite_data}
 * column for the given mid element. These user-specific settings allow the mysite_type_hook_data()
 * function to customize content output on a per-user basis.
 *
 * NOTE: Be sure to name the keys so that the data can be understood when invoked
 * by the mysite_type_hook_data() function.
 *
 * Invoked by mysite_content_settings_form_submit().
 *
 * @param $form_values
 *   Array of data passed by mysite_content_settings_form().
 *
 * @ingroup mysite_hooks
 */
function mysite_type_hook_content_form_submit($form_values) {
  $save = array('myvalue' => $form_values['myvalue']);
  $settings = serialize($save);
  db_query("UPDATE {mysite_data} SET settings = '%s' WHERE mid = %d", $settings, $form_values['mid']);
  drupal_set_message(t('Personal settings saved.'));
}

/**
 * Defines the layout theme for a user's MySite page.
 *
 * This function takes the data for a user's MySite page and passes it to the selected layout file for display.
 *
 * Pay careful attention to the CSS ids and classes, as they are used by mysite.css and mysite.js to enable collapse and drag-and-drop sorting functions.
 *
 * Invoked by mysite_page().  Invokes theme_mysite_hook_item().
 *
 * @param $content
 * An array of content to display.  This content consists of up to 4 sub-array elements:
 *    'owner' == the $user object of the owner of this MySIte page.
 *    'mysite' == information about this MySite page from mysite_get().
 *    'data' == the content data to display, from mysite_type_hook_data().  This data will be formatted by the selected format file.
 *    'header' == used to display any messages waiting for the user.
 * @return
 * A themed page view.
 *
 * @ingroup mysite_hooks
 */
function theme_mysite_hook_layout($content) {
  // break the array into pieces
  $owner = $content['owner'];
  $mysite = $content['mysite'];
  $columns = mysite_layout_default();
  $data = mysite_prepare_columns($mysite, $content['data'], $columns['count']);
  $header = $content['header'];
  // print the header message, if present
  if (isset($header)) {
    $output =  '<div class="messages">'.  $header .'</div>';
  }
  // ajax-generated message class
  $output .= '<div class="mysite-ajax"></div>';
  $output .= '<div class="mysite-sortable mysite-full-width" id="mysite-sort0">';
  // cycle through the data
  foreach ($data as $key => $value) {
    if ($value['mid']) {
      $output .= '<div class="mysite-group collapsible sortable-item" id="m'. $value['mid'] .'">';
    }
    else {
      $output .= '<div class="mysite-group">';
    }
    $output .= '<span class="mysite-header">'. $value['title'] .'</span>';
    $output .= '<div class="mysite-content">';
    if (!empty($value['output'])) {
      if (!empty($value['output']['image'])) {
        $output .= $value['output']['image'];
      }
      $output .= theme('mysite_'. $value['format'] .'_item', $value['output']['items']);
    }
    else {
      $output .= t('<p>No content found.</p>');
    }
    $output .= '</div>';
    $output .= '<div class="mysite-footer">';
    if (!empty($value['output']['base'])) {
      $output .= '<div class="mysite-footer-left">'. l(t('Read more'), $value['output']['base']) .'</div>';
    }
    $output .= ' <div class="mysite-footer-right">'. $value['actions'] .'</div> ';
    $output .= '</div>';
    $output .= '</div>';
  }
  $output .= '</div>';
  print theme('page', $output, variable_get('mysite_fullscreen', 1));
  return;
}

/**
 * Information about this layout.
 *
 * This hook is used to help display information about the layout to the end user.  It is also used to calculate the number of page regions to generate.
 *
 * Invoked by mysite_prepare_columns() and mysite_edit_form().
 *
 * @return
 * An array of data containing the following elements:
 *  - 'regions' == a positional array of named regions (similar to PHPTemplate regions) that will be shown to the end user.
 *  - 'count' == the number of regions in the layout.
 *  - 'name' == the layout name to present to the end user.
 *  - 'description' == a description of the layout shown to the end user.
 *  - 'image' == the name of the preview image used to represent this layout.
 *
 * @ingroup mysite_hooks
 */
function mysite_layout_hook() {
  $data = array();
  $data['regions'] = array('0' => t('Left'), '1' => t('Right'));
  $data['count'] = count($data['regions']);
  $data['name'] = t('My Layout');
  $data['description'] = t('A description of the layout.');
  $data['image'] = 'mylayout.png';
  return $data;
}

/**
 * Theme function hook for a content format.
 *
 * The MySite format files can choose to display any or all of the data supplied by mysite_type_hook_data().
 *
 * If the 'content' element of the array is present, the content item presents a single HTML block and should be printed in all cases.
 *
 * Invoked by mysite_theme_hook_layout().
 *
 * @param $item
 * An array of content to display in a MySite content element. An element of the array defined by mysite_type_hook_data().
 * @return $output
 * HTML content to be rendered.
 *
 * @ingroup mysite_hooks
 */
function theme_mysite_hook_item($item) {
  $output = '';
  foreach ($item as $element) {
    // if this is not a droplet, then build a content view
    if (empty($element['content'])) {
      $output .= '<h4>'. $element['link'] .'</h4>';
      if (!empty($element['subtitle'])) {
        $output .= '<div class="mysite-subtitle">&raquo; '. $element['subtitle'] .'</div>';
      }
      if (!empty($element['author'])) {
        $output .= '<div class="mysite-submitted">';
        $output .= t('submitted by !author on !time', array('!author' => $element['author'], '!time' => format_date(time(), 'medium')));
        $output .= '</div>';
      }
      $output .= $element['teaser'];
    }
    // this is a droplet, so output the content
    else {
      $output = theme('mysite_droplet', $element['content']);
    }
  }
  return $output;
}

/**
 * Format definition hook.
 *
 * Used by theme files to display sample formatting, this function registers a format file with MySite.
 *
 * Invoked by mysite_edit_form() and mysite_content_settings_form().
 *
 * @return
 * An array containing the format name and the HTML output formatted according the the pattern defined in the hook.
 *
 * @ingroup mysite_hooks
 */
function mysite_theme_hook() {
  $name = t('Format name');
  $output .= '<div class="mysite-sample mysite-content">';
  $output .= t('<h2><a href="#">Sample headline</a></h2>');
  $output .= t('submitted by johndoe on !time', array('!time' => format_date($element['date'], 'medium')));
  $output .= t('Sample layout text, including HTML formatting');
  $output .= '</div>';
  return array('format' => $name, 'sample' => $output);
}
