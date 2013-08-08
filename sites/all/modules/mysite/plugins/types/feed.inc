<?php
// $Id: feed.inc,v 1.26 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles RSS/ATOM feeds defined by the Aggregator module.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Aggregator module must be enabled for this plugin to register.
 */
function mysite_type_feed($get_options = TRUE) {
  if (module_exists('aggregator')) {
    $type = array(
      'name' => t('Web Feeds'),
      'description' => t('<b>Web Feeds</b>: News items for a specific Aggregator feed.'),
      'include' => 'feed',
      'prefix' => t(''),
      'suffix' => t('feed'),
      'category' => t('Aggregation'),
      'weight' => 0,
      'form' => TRUE,
      'label' => t('Add Web Feed'),
      'help' => t('Web feeds from other sites can be added to your personal page. Use the search box, or select from the list provided.'),
      'search' => TRUE
    );
    $basic_settings = variable_get('mysite_basic_feed_settings', array());
    $type = array_merge($type, $basic_settings);
    if ($get_options) {
      $type['options'] = mysite_type_feed_options();
    }
    return $type;
  }
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_feed_active($type) {
  // there must be some aggregator categories
  $message = '';
  $br = '';
  $count = db_result(db_query("SELECT count(fid) FROM {aggregator_feed}"));
  if ($count > 0) {
    $value = TRUE;
  }
  else {
    $value = FALSE;
    $message .= l(t('There are no Aggregator feeds available.'), 'admin/content/aggregator/add/feed');
    $br =  '<br />';
  }
  // if user feeds are allowed, they must be configured
  $feed = variable_get('mysite_feed', 0);
  if ($feed) {
    $cats = variable_get('mysite_feed_categories', array());
    $default = variable_get('mysite_feed_default', NULL);
    if (array_sum($cats) == 0) {
      $value = FALSE;
      $message .= $br . l(t('User Feed categories are not configured.'), 'admin/settings/mysite/type/feed');
      $br =  '<br />';
    }
    if (empty($default)) {
      $value = FALSE;
      $message .= $br . l(t('Default User Feed category is not configured'), 'admin/settings/mysite/type/feed');
      $br =  '<br />';
    }
    // some users must be allowed to add feeds, otherwise, give a configuration message
    $result = db_query("SELECT perm FROM {permission}");
    $check = '';
    while ($perms = db_fetch_object($result)) {
      $check .= $perms->perm;
    }
    if (!stristr($check, 'add mysite feeds')) {
      $value = FALSE;
      $message .=  $br . l(t('There are no users with the "add mysite feeds" permissions'), 'admin/user/access');
    }
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_feed_options() {
  $options = array();
  $feeds = array();
  $uncat = t('Other feeds');
  $sql = "SELECT fid, title, image FROM {aggregator_feed} ORDER BY title";
  $result = db_query($sql);
  while ($item = db_fetch_object($result)) {
    $item->ctitle = $uncat;
    $sql = "SELECT acf.cid, ac.cid, ac.title FROM {aggregator_category} ac INNER JOIN {aggregator_category_feed} acf ON acf.cid = ac.cid WHERE acf.fid = %d";
    $cats = db_fetch_object(db_query($sql, $item->fid));
    if (!empty($cats->title)) {
      $item->ctitle = $cats->title;
    }
    $feeds[$item->ctitle][] = $item;
  }
  foreach ($feeds as $key => $value) {
    foreach ($value as $item) {
      $options['group'][] = $key;
      $options['name'][] = mysite_type_feed_title($item->fid, $item->title);
      $options['type_id'][] = $item->fid;
      $options['type'][] = 'feed';
      $icon = mysite_get_icon('feed', $item->fid);
      if (module_exists('mysite_icon')) {
        if (!empty($item->image) && empty($icon)) {
          $temp = mysite_type_feed_image($item->fid, $item->image);
          // Did the icon download succeed?  See mysite_type_feed_image().
          if ($temp != -1) {
            $icon = $temp;
          }
        }
      }
      $options['icon'][] = $icon;
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_feed_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $sql = "SELECT title FROM {aggregator_feed} WHERE fid = %d";
      $feed = db_fetch_object(db_query($sql, $type_id));
      $title = $feed->title;
    }
    $type = mysite_type_feed(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find feed title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_feed_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {

    $sql = "SELECT fid, title, url, image, checked FROM {aggregator_feed} WHERE fid = %d";
    $feed = db_fetch_object(db_query($sql, $type_id));

    $image = $feed->image;
    if (module_exists('mysite_icon')) {
      $icon = variable_get('mysite_icon_path', 'files/mysite') .'/'. mysite_get_icon('feed', $feed->fid);
      if (!empty($icon)) {
        $image = l(theme_image($icon), $feed->url, array(), NULL, FALSE, FALSE, TRUE);
      }
    }
    $data = array(
      'base' => 'aggregator/sources/'. $feed->fid,
      'xml' => $feed->url,
      'image' => $image
      );

    $sql = "SELECT i.iid, i.title, i.link, i.author, i.description, i.timestamp, f.title AS feedtitle FROM {aggregator_item} i INNER JOIN {aggregator_feed} f ON i.fid = f.fid WHERE i.fid = %d ORDER BY i.timestamp DESC";
    $result = db_query_range($sql, $feed->fid, 0, variable_get('mysite_elements', 5));
    $items = array();
    $i = 0;
    $type = mysite_type_feed(FALSE);
    while ($obj = db_fetch_object($result)) {
      $items[$i]['type'] = 'feed';
      $items[$i]['link'] = l($obj->title, $obj->link, array('target' => $type['link_target']));
      $items[$i]['title'] = check_plain($obj->title);
      $items[$i]['subtitle'] = NULL;
      $items[$i]['date'] = $obj->timestamp;
      $items[$i]['uid'] = NULL;
      if (!empty($obj->author)) {
        $items[$i]['author'] = check_plain($obj->author);
      }
      else {
        $items[$i]['author'] = check_plain($obj->feedtitle);
      }
      $items[$i]['teaser'] = aggregator_filter_xss($obj->description);
      $items[$i]['nid'] = NULL;
      $i++;
    }
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find feed data'), 'error');
  return;

}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_feed_block($arg, $op = 'view') {
  global $user;
  if (user_access('access news feeds') && $arg[0] == 'aggregator' && $arg[1] == 'sources' && is_numeric($arg[2])) {
    $sql = "SELECT fid, title FROM {aggregator_feed} WHERE fid = %d";
    $feed = db_fetch_object(db_query($sql, $arg[2]));
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'feed';
    $data['type_id'] = $feed->fid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_feed_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_feed_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_feed_search()
 */
function mysite_type_feed_search_form($uid) {
  // we cannot use 'title' here because of autocomplete form handler conflicts
  $form['add_feed']['feed_title'] = array('#type' => 'textfield',
    '#title' => t('Feed title'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The name of the feed; typically the name of the web site you syndicate content from.'),
    '#required' => TRUE,
    '#autocomplete_path' => 'autocomplete/mysite/feed'
  );
  $form['add_feed']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_feed']['type'] = array('#type' => 'hidden', '#value' => 'feed');
  $form['add_feed']['submit'] = array('#type' => 'submit', '#value' => t('Add Web Feed'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_feed_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT fid, title, description FROM {aggregator_feed} WHERE title LIKE LOWER('%s%%')", $form_values['feed_title']);
  $count = 0;
  while ($feed = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $feed->fid;
    $data[$count]['title'] = mysite_type_feed_title($feed->fid, $feed->title);
    $data[$count]['description'] = aggregator_filter_xss($feed->description);
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'feed');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_feed_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT title FROM {aggregator_feed} WHERE title LIKE LOWER('%s%%')", $string, 0, 10);
  while ($feed = db_fetch_object($result)) {
    $matches[$feed->title] = check_plain($feed->title);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_feed_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT fid FROM {aggregator_feed} WHERE fid = %d";
    $check = db_fetch_object(db_query($sql, $item['type_id']));
    if (empty($check->fid)) {
      $data[$item['mid']]  = $item;
    }
  }
  return $data;
}

 /**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_feed_settings() {
  mysite_check_settings('feed');
  // specific rules for the aggregator module.  Since we let users add feeds, we need to preset some default values for each feed.
  if (module_exists('aggregator')) {
    return drupal_get_form('mysite_type_feed_settings_form');
  }
  else {
    return t('The aggregator module is not active.  These features cannot be used.');
  }
}

/**
 * FormsAPI for mysite_type_feed_settings()
 */
function mysite_type_feed_settings_form() {
  $form['mysite_feed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Aggregator Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $use_feeds = variable_get('mysite_feed', 1);
  $form['mysite_feed']['mysite_feed'] = array(
    '#type' => 'radios', '#title' => t('Aggregator Feeds'), '#default_value' => $use_feeds,
    '#options' =>  array(1 => t('Allow users to add new feeds'), 0 => t('Do not allow users to add new feeds')),
    '#description' => t('Should users be able to add feeds to their MySite page?  If so, be sure to grant the "add mysite feeds" permission in <a href="@url">MySite access control</a>.', array('@url' => url('admin/user/access')))
  );
  if ($use_feeds == 1 && user_access('administer news feeds')) {
    $sql = "SELECT cid, title, description FROM {aggregator_category}";
    $result = db_query($sql);
    $options = array();
    while ($feed = db_fetch_object($result)) {
      $options[$feed->cid] = $feed->title;
    }
    if (empty($options)) {
      $form['mysite_feed']['no_feeds'] = array(
        '#value' => t('<p>There are no categories configured.  Please go to <a href="@url">aggregator categories</a> to add one. <br /><b>User Feeds</b> is a good choice.</p>', array('@url' => url('admin/content/aggregator/add/category'))) ,
      );
    }
    else {
      $form['mysite_feed']['mysite_feed_categories'] = array(
        '#type' => 'checkboxes', '#title' => t('Allowed Feed Categories'), '#default_value' => variable_get('mysite_feed_categories', NULL),
        '#options' =>  $options,
        '#description' => t('In which categories may users add new feeds?')
      );
      $feeds = variable_get('mysite_feed_categories', array());
      if (array_sum($feeds) > 0) {
        $implode = "(". implode(", ", $feeds) .")";
        $sql = "SELECT cid, title, description FROM {aggregator_category} WHERE cid IN %s";
        $result = db_query($sql, $implode);
      }
      else {
      $form['mysite_feed']['no_categories'] = array(
         '#value' => t('<p><em>There are no categories configured. After you select at least one category, you must select a default.</em></p>') ,
        );
      }
      $options = array();
      while ($feed = db_fetch_object($result)) {
        $options[$feed->cid] = $feed->title;
      }
      if (array_sum($feeds) > 0) {
        $form['mysite_feed']['mysite_feed_default'] = array(
          '#type' => 'radios', '#title' => t('Default Feed Category'), '#default_value' => variable_get('mysite_feed_default', NULL),
          '#options' =>  $options,
          '#required' => FALSE,
          '#description' => t('What is the default category for new feeds?')
        );
      }
    }
    $period = drupal_map_assoc(array(900, 1800, 3600, 7200, 10800, 21600, 32400, 43200, 64800, 86400, 172800, 259200, 604800, 1209600, 2419200), 'format_interval');
    $form['mysite_feed']['mysite_feed_update'] = array(
      '#type' => 'select', '#title' => t('Update interval'), '#default_value' => variable_get('mysite_feed_update', 3600),
      '#options' => $period,
      '#description' => t('Define the update interval for user-supplied feeds.  This value will be used for all user-submitted feeds.')
    );
  }
  return system_settings_form($form);
}

/**
 * Implements mysite_type_hook_form()
 *
 * Lets users add an XML feed directly through MySite
 */
function mysite_type_feed_form($owner) {
  if (empty($owner->uid)) {
    global $user;
    $owner = $user;
  }
  if (variable_get('mysite_feed', 0) == 1 && user_access('add mysite feeds', $owner)) {
    $output = '<h3>'. t('Add new feed') .'</h3>';
    $output .= '<p>'. t('You aren\'t limited to existing feeds. Add any RSS or Atom web feed from your favorite sites.') .'</p>';
    $output .= drupal_get_form('mysite_type_feed_custom_form', $owner);
  }
  return $output;
}

/**
 * Implements mysite_type_hook_custom_form()
 *
 * FormsAPI for mysite_type_feed_custom_form()
 */
function mysite_type_feed_custom_form($owner) {
  $feeds = variable_get('mysite_feed_categories', NULL);
  $refresh = variable_get('mysite_feed_update', 3600);
  $form['new_feed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add New Web Feed'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['new_feed']['title'] = array('#type' => 'textfield',
    '#title' => t('Feed title'),
    '#default_value' => $edit['title'],
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The name of the feed; typically the name of the web site you syndicate content from.'),
    '#required' => FALSE,
    '#autocomplete_path' => 'autocomplete/mysite/feed'
  );
  $form['new_feed']['url'] = array('#type' => 'textfield',
    '#title' => t('URL'),
    '#default_value' => $edit['url'],
    '#maxlength' => 255,
    '#size' => 40,
    '#description' => t('The fully-qualified URL of the feed.'),
    '#required' => FALSE,
  );
  $form['new_feed']['refresh'] = array(
      '#type' => 'hidden', '#value' => $refresh
  );
  $form['new_feed']['uid'] = array(
      '#type' => 'hidden', '#value' => $owner->uid
  );
  // categories
  if (empty($edit['category'])) {
    $edit['category'] = variable_get('mysite_feed_default', array());
  }
  if (!empty($edit['category'])) {
    $implode = "(". implode(", ", $feeds) .")";
    $sql = "SELECT cid, title, description FROM {aggregator_category} WHERE cid IN %s";
    $result = db_query($sql, $implode);
    $options = array();
    while ($category = db_fetch_object($result)) {
      $options[$category->cid] = check_plain($category->title);
    }
    if (empty($options)) {
      $form['new_feed']['no_feeds'] = array(
        '#value' => t('<p>There are no categories configured.  Please go to <a href="@url">aggregator categories</a> to add one. <br /><b>User Feeds</b> is a good choice.</p>', array('@url' => url('admin/content/aggregator/add/category'))) ,
      );
    }
    else if (count($options) > 1) {
      $form['new_feed']['category'] = array(
        '#type' => 'radios', '#title' => t('Feed Categories'), '#default_value' => $edit['category'],
        '#options' =>  $options,
        '#description' => t('Select a category for this feed')
      );
    }
    else {
      $form['new_feed']['category'] = array(
        '#type' => 'hidden', '#value' => $edit['category']
      );
    }
  }
  $form['new_feed']['submit'] = array('#type' => 'submit', '#value' => t('Submit New Feed'));
  return $form;
}

/**
 * FormsAPI for mysite_type_feed_custom_form()
 *
 * Mostly taken from Aggregator module, which has no API.
 */
function mysite_type_feed_custom_form_submit($form_id, $form_values) {
  if ($form_values['op'] == t('Submit New Feed')) {
    // Check for duplicate titles and see what we need to save
    if (isset($form_values['fid'])) {
      $result = db_query("SELECT fid, title, url, description FROM {aggregator_feed} WHERE (title = '%s' OR url='%s') AND fid <> %d", $form_values['title'], $form_values['url'], $form_values['fid']);
    }
    else {
      $result = db_query("SELECT fid, title, url, description FROM {aggregator_feed} WHERE title = '%s' OR url='%s'", $form_values['title'], $form_values['url']);
    }
    $success = FALSE;
    while ($feed = db_fetch_object($result)) {
      $form_values['url'] = $feed->url;
      if ($form_values['title'] && $form_values['url'] && strcasecmp($feed->title, $form_values['title']) == 0) {
        $check = mysite_check($form_values['uid'], 'feed', $feed->fid, $mid = NULL);
        unset($_SESSION['messages']['error']);
        if ($check == 0) {
          drupal_set_message(t('A feed named @feed already exists. <a href="@url">Would you like to add it to your MySite page</a>?', array('@feed' => $form_values['title'], '@url' => url(mysite_add_link($form_values['uid'], 'feed', $feed->fid)))), 'status');
        }
        else {
          drupal_set_message(t('A feed named @feed already exists and has been added to your MySite page.', array('@feed' => $form_values['title'], '@url' => url(mysite_add_link($form_values['uid'], 'feed', $feed->fid)))), 'status');
        }
        $success = TRUE;
        $form_values['url'] = NULL;
        $form_values['title'] = NULL;
      }
    }
    if (!$success) {
      if (empty($form_values['title'])) {
        form_set_error('title', t('You must enter a valid title.'));
      }
      if (empty($form_values['url'])) {
        form_set_error('url', t('You must enter a valid URL.'));
      }
    }
  }

  // if we need to save data, then we will
  if (!empty($form_values['title']) && !empty($form_values['uid'])) {
    // check the validity of the feed source
    // Request feed.
    $headers = array();
    $modified = 0;
    $headers['If-Modified-Since'] = gmdate('D, d M Y H:i:s', $modified) .' GMT';
    $result = drupal_http_request($form_values['url'], $headers);
    // we must make sure that the feed is XML -- why doesn't aggregator do this?
    if (!stristr($result->headers['Content-Type'], 'xml') && !stristr($result->headers['Content-type'], 'xml')) {
      drupal_set_message(t('<p>Failed to find RSS feed.  Please check the URL and try again.</p>'), 'error');
      return;
    }
    else {
      // Process HTTP response code.
      switch ($result->code) {
        case 301:
          $form_values['url'] = $result->redirect_url;
          $continue = TRUE;
          break;
        case 304:
        case 200:
        case 302:
        case 307:
          $continue = TRUE;
          break;
        default:
          $continue = FALSE;
          drupal_set_message(t('<p>Failed to parse RSS feed.  Please check the URL and try again.</p>'), 'error');
          return;
      }
    }
    // check for the real title of the feed and use it globally
    $xml_parser = drupal_xml_parser_create($result->data);
    xml_set_element_handler($xml_parser, 'mysite_feed_element_start', 'mysite_feed_element_end');
    xml_set_character_data_handler($xml_parser, 'mysite_feed_element_data');

    if (!xml_parse($xml_parser, $result->data, 1)) {
      watchdog('aggregator', t('The feed from %site seems to be broken, due to an error "%error" on line %line.', array('%site' => $feed['title'], '%error' => xml_error_string(xml_get_error_code($xml_parser)), '%line' => xml_get_current_line_number($xml_parser))), WATCHDOG_WARNING);
      drupal_set_message(t('The feed from %site seems to be broken, because of error "%error" on line %line.', array('%site' => $feed['title'], '%error' => xml_error_string(xml_get_error_code($xml_parser)), '%line' => xml_get_current_line_number($xml_parser))), 'error');
      $continue = FALSE;
    }
    xml_parser_free($xml_parser);
    global $channel;
    // Prepare the channel data:
    foreach ($channel as $key => $value) {
      $channel[$key] = trim($value);
      $channel[$key] = rtrim($value, " -");
    }

    // check the status of a corrected feed
    $result = db_query("SELECT fid, title, url, description FROM {aggregator_feed} WHERE title = '%s' OR url='%s'", $form_values['title'], $form_values['url']);
    $feed_check = db_fetch_object($result);
    if (empty($feed_check)) {
      $continue = TRUE;
    }
    else {
      $continue = FALSE;
      $check = mysite_check($form_values['uid'], 'feed', $feed_check->fid, $mid = NULL);
      unset($_SESSION['messages']['error']);
      if ($check == 0) {
        drupal_set_message(t('A feed named @feed already exists at that url. <a href="@url">Would you like to add it to your MySite page</a>?', array('@feed' => $feed_check->title, '@url' => url(mysite_add_link($form_values['uid'], 'feed', $feed_check->fid)))), 'status');
      }
      else {
        drupal_set_message(t('A feed named @feed already exists at that url and has been added to your MySite.', array('@feed' => $feed_check->title, '@url' => url(mysite_add_link($form_values['uid'], 'feed', $feed_check->fid)))), 'status');
      }
    }
    if ($continue) {
      // A single unique id for bundles and feeds, to use in blocks.
      $form_values['fid'] = db_next_id('{aggregator_feed}_fid');
      db_query("INSERT INTO {aggregator_feed} (fid, title, url, refresh, etag, modified, block) VALUES (%d, '%s', '%s', %d, '%s', %d, 5)", $form_values['fid'], $channel['title'], $form_values['url'], $form_values['refresh'], $result->headers['ETag'], $modified);
      // The feed is being saved, save the categories as well.
      if ($form_values['category']) {
        db_query('INSERT INTO {aggregator_category_feed} (fid, cid) VALUES (%d, %d)', $form_values['fid'], $form_values['category']);
      }
      menu_rebuild();
      watchdog('aggregator', t('Feed @feed added.', array('@feed' => $channel['title'])), WATCHDOG_NOTICE, l(t('view'), 'admin/content/aggregator'));
      drupal_set_message(t('The feed @feed has been added.', array('@feed' => $form_values['title'])));

      // refresh the feed to add content
      aggregator_refresh(aggregator_get_feed($form_values['fid']));

      // save to MySite
      mysite_content_add($form_values['uid'], 'feed', $form_values['fid'], $form_values['title']);
    }
  }
}

/**
 * Callback function used by the XML parser.
 *
 * Taken from Aggregator module.
 */
function mysite_feed_element_start($parser, $name, $attributes) {
  global $item, $element, $tag, $items, $channel;
  switch ($name) {
    case 'TITLE':
      if ($items == 0) {
        $element = $name;
        $items = 1;
      }
      break;
  }
  $tag = $name;
}

/**
 * Callback function used by the XML parser.
 *
 * Taken from Aggregator module.
 */
function mysite_feed_element_end($parser, $name) {
  global $element;
  switch ($name) {
    case 'TITLE':
      $element = '';
      break;
  }
}

/**
 * Callback function used by the XML parser.
 *
 * Taken from Aggregator module.
 */
function mysite_feed_element_data($parser, $data) {
  global $channel, $element, $items;
  switch ($element) {
    case 'TITLE':
      if ($items == 1) {
        $channel['title'] = $data;
      }
      break;
  }
}

/**
 * This function takes a Feed image and saves it locally.
 *
 * We do this for added security and speed.  Requires the MySite Icons module.  Feed icons will be downloaded and saved locally in the format feed-FID.ext.
 *
 * @param $fid
 *  The feed id, taken from {aggregator_feed}
 * @param $image
 * The image string taken from {aggregator_feed}
 *
 * @return
 * The filename string of the local copy of the file, or -1 on failure.
 */
function mysite_type_feed_image($fid, $image = NULL) {
  // This feature can be disabled under the module settings.
  $download = variable_get('mysite_icon_download', 1);
  if (!$download) {
    return -1;
  }
  $folder = variable_get('mysite_icon_path', 'mysite');
  $path = file_create_path(file_directory_path() .'/'. $folder);
  // create the directory, if possible and needed.
  $dir = mysite_icon_folder_check($path);
  if ($dir) {
    $temp = file_directory_temp();
    $newfile = '';
    if (!empty($image)) {
      if ($src = preg_match('/src="(.+?)"/', $image)) {
        $src = preg_replace('/.+src="(.+?)".+/',  '\1', $image);
        $src = check_url($src);
        $ext = explode('.', $src);
        $ext = '.'. array_pop($ext);
      }
    }
    $filename = 'feed-'. $fid . $ext;
    $file = file_check_location($path .'/'. $filename, $path);
    if (file_exists($file)) {
      $newfile = $path .'/'. $filename;
    }
    if (empty($newfile) && !empty($image)) {
      watchdog('MySite', t('Copying icon file for Feed ID %fid.', array('%fid' => $fid)), WATCHDOG_NOTICE);
      $file = drupal_http_request($src);
      $newfile = file_save_data($file->data, $temp .'/'. $filename, FILE_EXISTS_REPLACE);
      $info = image_get_info($newfile);
      if ($info['extension']) {
        if (image_get_toolkit()) {
          image_scale($newfile, $newfile, 120, 60);
        }
        $move = file_move($newfile, $path, FILE_EXISTS_REPLACE);
      }
      else {
        $filename = -1;
        watchdog('MySite', t('The transfer of a MySite feed icon failed -- bad file extension -- for Feed ID %fid.', array('%fid' => $fid)), WATCHDOG_ERROR);
      }
      if ($move) {
        $iid = db_next_id('{mysite_icon}_iid');
        db_query("INSERT INTO {mysite_icon} (iid, type, type_id, icon) VALUES (%d, '%s', %d, '%s')", $iid, 'feed', $fid, $filename);
      }
      else {
        $filename = -1;
        watchdog('MySite', t('The transfer of a MySite feed icon failed -- could not copy file -- for Feed ID %fid.', array('%fid' => $fid)), WATCHDOG_ERROR);
      }
    }
    return $filename;
  }
}
