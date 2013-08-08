<?php
// $Id: popular.inc,v 1.6 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Creates a list of the 100 most popular MySite page elements.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * This plugin is native to MySite.
 */
function mysite_type_popular($get_options = TRUE) {
  $type = array(
    'name' => t('Most popular'),
    'description' => t('<b>Most Popular</b>: The MySite content most requested by users.'),
    'include' => 'popular',
    'prefix' => t(''),
    'suffix' => t(''),
    'category' => t('MySite'),
    'weight' => 0,
    'form' => FALSE,
    'label' => t('Add Popular Content'),
    'help' => t('Browse the list of the 100 most popular items on personal user pages.'),
    'search' => FALSE
  );
  $basic_settings = variable_get('mysite_basic_popular_settings', array());
  $type = array_merge($type, $basic_settings);
  if ($get_options) {
    $type['options'] = mysite_type_popular_options();
  }
  return $type;
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_popular_active($type) {
  // make sure some data exists in the {mysite_data} table.
  $count = db_result(db_query("SELECT COUNT(mid) as count FROM {mysite_data}"));
  if ($count > 0) {
    return array($type => TRUE);
  }
  else {
    return array($type => FALSE, 'message' => t('There is no content stored in MySite pages.  Try again later.'));
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_popular_options() {
  $options = array();
  // the results of this query are cached every 8 hours for optimal performance
  $cache = cache_get('mysite:popular');
  $items = unserialize($cache->data);
  if (empty($items)) {
    $sql = "SELECT type, type_id FROM {mysite_data}";
    $result = db_query($sql);
    $items = array();
    while ($item = db_fetch_object($result)) {
      // profiles are a special case, and should be counted as one item
      if ($item->type == 'profile') {
        $item->type_id = 0;
      }
      $items[$item->type .'-'. $item->type_id] = $items[$item->type .'-'. $item->type_id] + 1;
    }
    arsort($items);
    // trim to 100 items
    $list = array_chunk($items, 100, TRUE);
    $items = $list[0];
    cache_set('mysite:popular', 'cache', serialize($items), time() + (3600 * 8));
  }
  // load the includes to ensure we have the functions we need
  $types = mysite_load_includes('types');
  foreach ($items as $key => $value) {
    $data = explode('-', $key);
    $type = $data[0];
    $type_id = $data[1];
    // make sure that the items are active
    if (in_array($type, $types)) {
      // check profiles, you must add your own
      if ($type == 'profile') {
        $type_id = arg(1);
      }
      // get the generic name of this content item
      $func = 'mysite_type_'. $type .'_title';
      $options['name'][] = $func($type_id, NULL);
      $options['type_id'][] = $type_id;
      $options['type'][] = $type;
      $icon = mysite_get_icon($type, $type_id);
      $options['icon'][] = $icon;
    }
  }
  return $options;
}

/**
 * Implements mysite_type_hook_clear()
 */
function mysite_type_popular_clear() {
  // all we need to do is clear the popular data from the cache
  cache_clear_all('mysite:popular', 'cache');
  // return an empty array to the handler
  return array();
}

/**
 * Note: All other MySite functions are handled by the specific Type files.
 */
