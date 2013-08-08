<?php
// $Id: profile.inc,v 1.18 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Handles display of the core user profile.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 */
function mysite_type_profile($get_options = TRUE) {
  // if the user's profile is already active, then this turns off the submenu link
  static $_mysite_profile_admin;
  if (arg(0) == 'mysite' && !isset($_mysite_profile_admin)) {
    $_mysite_profile_admin = db_result(db_query("SELECT COUNT(mid) FROM {mysite_data} WHERE type = 'profile' AND uid = %d", arg(1)));
  }
  $type = array(
    'name' => t('Profile'),
    'description' => t('<b>User Profiles</b>: Selected information about the user'),
    'include' => 'profile',
    'prefix' => t(''),
    'suffix' => t(''),
    'category' => t('Content'),
    'weight' => 10,
    'form' => FALSE,
    'admin' => $_mysite_profile_admin,
    'label' => t('Add Your Profile'),
    'help' => t('You may add your profile to your personal page.'),
    'search' => FALSE
  );
  $basic_settings = variable_get('mysite_basic_profile_settings', array());
  $type = array_merge($type, $basic_settings);
  if ($get_options) {
    $type['options'] = mysite_type_profile_options();
  }
  return $type;
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_profile_active($type) {
  $default = variable_get('mysite_profile_settings', mysite_type_profile_defaults());
  $br = '';
  // some users must be allowed to access profiles, otherwise, give a configuration message
  $result = db_query("SELECT perm FROM {permission}");
  $check = '';
  while ($perms = db_fetch_object($result)) {
    $check .= $perms->perm;
  }
  if (stristr($check, 'access user profiles')) {
    $value = TRUE;
  }
  else {
    $value = FALSE;
    $message = l(t('There are no users with permission to access profiles.'), 'admin/user/access');
    $br = '<br />';
  }
  if (empty($default)) {
    $value = FALSE;
    $message .= $br . l(t('No profile settings are configured for MySite.'), 'admin/settings/mysite/type/user');
  }
  return array($type => $value, 'message' => $message);
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_profile_options() {
  $profile = mysite_type_profile_get();
  if (!empty($profile)) {
    $options['name'][] = mysite_type_profile_title($profile->uid, $profile->name);
    $options['type_id'][] = $profile->uid;
    $options['type'][] = 'profile';
    $icon = mysite_get_icon('profile', $profile->uid);
    if (empty($icon)) {
      $icon = array('path' => file_directory_path() .'/'. variable_get('user_picture_path', 'pictures'), 'file' => $profile->picture); // this is a special case
    }
    $options['icon'][] = $icon;
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_profile_title($type_id = NULL, $title = NULL) {
  global $user;
  if (isset($type_id)) {
    // if this is my profile, say so
    if ($user->uid == $type_id || $type_id == 0) {
      $title = t('My profile');
    }
    elseif (is_null($title)) {
      $profile = user_load(array('uid' => $type_id));
      $title = $profile->name;
    }
    $type = mysite_type_profile(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find profile title.'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_profile_data($type_id = NULL, $settings = NULL) {
  $profile = mysite_type_profile_get();
  if (!empty($profile->uid)) {
    $mysite = mysite_get($profile->uid);
  }
  if ($mysite->status == 1) {
    $allowed = variable_get('mysite_profile_settings', mysite_type_profile_defaults());
    if (empty($settings)) {
      $settings = $allowed;
    }
    $allowed = $settings;
    $content = mysite_type_profile_load($profile, $allowed);
  }
  else {
    $content = theme('mysite_type_profile_anonymous', $profile);
  }
  $data = array(
    'base' => 'user/'. $profile->uid,
    'xml' => NULL,
    );
  $items = array();
  $i = 0;
  $type = mysite_type_profile(FALSE);
  $items[$i]['type'] = 'profile';
  $items[$i]['link'] = l($profile->name, 'user/'. $profile->uid, array('target' => $type['link_target']));
  $items[$i]['title'] = mysite_type_profile_title($profile->uid, $profile->name);
  $items[$i]['subtitle'] = NULL;
  $items[$i]['date'] = NULL;
  $items[$i]['uid'] = $profile->uid;
  $items[$i]['author'] =NULL;
  $items[$i]['teaser'] = NULL;
  $items[$i]['content'] = $content;
  $items[$i]['nid'] = NULL;
  $data['items'] = $items;
  return $data;
}

/**
 * Implements mysite_type_hook_block().
 */
function mysite_type_profile_block($arg, $op = 'view') {
  global $user;
  if ($arg[1] == $user->uid && ($arg[0] == 'user' && is_numeric($arg[1]))) {
    $data = array();
    $data['uid'] = $user->uid;
    $data['type'] = 'profile';
    $data['type_id'] = $user->uid;
    $content = mysite_block_handler($data);
    return $content;
  }
}

 /**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_profile_settings() {
  mysite_check_settings('profile');
  $default = variable_get('mysite_profile_settings', mysite_type_profile_defaults());
  $data = mysite_type_profile_elements();
  return drupal_get_form('mysite_type_profile_settings_form', $default, $data);
}

/**
 * FormsAPI for mysite_type_profile_settings
 */
function mysite_type_profile_settings_form($default, $data) {
  $form['mysite_settings_form'] = array('#value' => t('There are no valid profile options.'));
  if (!empty($data)) {
    $form['mysite_settings_form'] = array('#value' => t('Select the profile elements that will display by default on a user MySite page.  Users may edit these settings when they configure their MySite profile element.'));
    $extra = mysite_type_profile_form_items($default, $data);
    $form['mysite_settings_form'] = array_merge($form['mysite_settings_form'], $extra);
    $form['mysite_profile_settings']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));
    $form['mysite_profile_settings']['reset'] = array('#type' => 'submit', '#value' => t('Reset to defaults'));
  }
  return $form;
}

/**
 * Prepare profile items for mysite_type_profile_settings
 */
function mysite_type_profile_form_items($allowed, $data) {
  foreach ($data as $key => $value) {
    $options = array();
    $current = array();
    foreach ($value as $k => $v) {
      $options[$v['name']] = (!empty($v['title'])) ? $v['title'] : $key;
      if (in_array($v['name'], $allowed)) {
        $current[$v['name']] = $v['name'];
      }
    }
    // urlencode() the $key to handle spaces
    $form[urlencode($key)]= array(
      '#type' => 'checkboxes',
      '#title' => $key,
      '#value' => $current,
      '#options' => $options,
      '#required' => FALSE,
    );
  }
  return $form;
}


/**
 * Forms API
 */
function mysite_type_profile_settings_form_submit($form_id, $form_values) {
  if ($form_values['op'] == $form_values['submit']) {
    // process the vars
    $save = mysite_type_profile_process($form_values);
    if (!empty($save)) {
      $save = array_unique($save);
      variable_set('mysite_profile_settings', $save);
    }
    else {
      form_set_error('form', t('At least one option must be selected.'));
    }
  }
  else {
    //reset the vars!
    $save = mysite_type_profile_defaults();
    variable_set('mysite_profile_settings', $save);
  }
}

/**
 * Implements mysite_type_hook_content_form()
 */
function mysite_type_profile_content_form($data) {
  $allowed = unserialize($data['settings']);
  if (empty($allowed)) {
    $allowed = variable_get('mysite_profile_settings', mysite_type_profile_defaults());
  }
  $elements = mysite_type_profile_elements($data['uid']);
  $extra = array();
  $extra['notice'] = array('#value' => t('Select the profile elements to show on your personal page.'));
  $form = mysite_type_profile_form_items($allowed, $elements);
  $extra = array_merge($extra, $form);
  return $extra;
}

/**
 * Implements mysite_type_hook_content_form_validate()
 */
function mysite_type_profile_content_form_validate($form_values) {
  // process the vars
  $save = mysite_type_profile_process($form_values);
  if (empty($save)) {
    form_set_error('form', t('At least one option must be selected.'));
  }
}

/**
 * Implements mysite_type_hook_content_form_submit()
 */
function mysite_type_profile_content_form_submit($form_values) {
  // process the vars
  $save = mysite_type_profile_process($form_values);
  $settings = serialize($save);
  db_query("UPDATE {mysite_data} SET settings = '%s' WHERE mid = %d", $settings, $form_values['mid']);
  drupal_set_message(t('Personal settings saved.'));
}

/**
 * Helper function to process the form data
 */
function mysite_type_profile_process($form_values) {
  $save = array();
  foreach ($form_values as $key => $value) {
    if (is_array($value)) {
      foreach ($value as $k => $v) {
        if (!empty($v)) {
          $save[] = $key;
          $save[] = $v;
        }
      }
    }
  }
  $save = array_unique($save);
  return $save;
}

/**
 * Finds the baseline profile elements
 *
 * CHECK -- is this sufficient?
 */
function mysite_type_profile_elements($uid = 1) {
  $data = array();
  if ($uid == 0) {
    $uid = 1;
  }
  // we use the administrator's profile to get elements
  $profile = user_load(array('uid' => $uid));
  $raw = mysite_type_profile_fields($profile);
  $fields = array();
  // picture is a special case, so we add it here
  $picture = t('User picture');
  $fields['Picture']['mysite_picture'] = array(
    'title' => $picture,
    'name' => 'mysite_picture',
    'category' => $picture,
    'weight' => 0,
    'visibility' => PROFILE_PUBLIC
  );
  // prefix the 'name' with an underscore _ to avoid naming collisions later
  foreach ($raw as $category => $value) {
    foreach ($value as $key => $name) {
      $fields[$category][$key] = $name;
      // get rid of whitespace
      $fields[$category][$key]['name'] = $key;
      $fields[$category][$key]['category'] = $category;
      $fields[$category][$key]['weight'] = 0;
      $fields[$category][$key]['visibility'] = PROFILE_PUBLIC;
      unset($fields[$category][$key]['value']);
      unset($fields[$category][$key]['class']);
    }
  }
  if (module_exists('profile')) {
    $result = db_query('SELECT title, name, category, weight, visibility FROM {profile_fields} ORDER BY category, weight');
    while ($field = db_fetch_array($result)) {
      if ($field['visibility'] == PROFILE_PUBLIC || $field['visibility'] == PROFILE_PUBLIC_LISTINGS) {
        $string = mysite_type_profile_make_key($field['category'], $field['name']);
        $field['name'] = $string;
        $fields[$field['category']][$string] = $field;
      }
    }
  }
  //remove MySite from the options
  unset($fields['History']['history_mysite']);
  return $fields;
}

/**
 * Helper function to make namespace safe keys for profile elements
 */
function mysite_type_profile_make_key($category, $key) {
  $string = strtolower(urlencode($category) .'_'. urlencode($key));
  return $string;
}

/**
 * Helper function to load the right profile data
 */
function mysite_type_profile_get() {
  // set the user
  $uid = arg(1);
  $profile = user_load(array('uid' => $uid));
  return $profile;
}

/**
 * Helper function to load profile data for viewing
 *
 * This function is mostly cribbed from user_view(), but we need to
 * remove certain elements of the data.
 */
function mysite_type_profile_load($profile, $allowed) {
  $fields = mysite_type_profile_fields($profile);

  // Let modules change the returned fields - useful for personal privacy
  // controls. Since modules communicate changes by reference, we cannot use
  // module_invoke_all().
  foreach (module_implements('profile_alter') as $module) {
    $function = $module .'_profile_alter';
    $function($profile, $fields);
  }
  // we always remove MySite from the $fields array
  unset($fields['History']['_mysite']);
  if (!empty($fields)) {
    foreach ($fields as $key => $value) {
      // urlencode() the $key to handle spaces
      if (!in_array(urlencode($key), $allowed)) {
        unset($fields[$key]);
      }
      if (isset($fields[$key]) && is_array($value)) {
        foreach ($value as $label => $content) {
          $check = "$key_$label";
          if (!in_array($check, $allowed)) {
            unset($fields[$key][$label]);
            // in odd cases, the keys conflict
          }
        }
      }
    }
  }
  if (empty($fields)) {
    return t('No profile information was found.');
  }
  return theme('mysite_user_profile', $profile, $fields, $allowed);
}

/**
 * Get the fields
 */
function mysite_type_profile_fields($profile) {
  // Retrieve and merge all profile fields:
  $fields = array();
  foreach (module_list() as $module) {
    if ($data = module_invoke($module, 'user', 'view', '', $profile)) {
      foreach ($data as $category => $items) {
        foreach ($items as $key => $item) {
          $item['class'] = "$module-". $item['class'];
          $string = mysite_type_profile_make_key($category, $key);
          $fields[$category][$string] = $item;
        }
      }
    }
  }
  return $fields;
}

/**
 * Get the defaults for variables
 */
function mysite_type_profile_defaults() {
  if (variable_get('user_pictures', 0)) {
    return array('Picture', 'mysite_picture', 'History', 'history_0', 'history_history');
  }
  else {
    return array('History', 'history_0', 'history_history');
  }
}

/**
 * Theme
 */
function theme_mysite_user_profile($profile, $fields, $allowed) {
  $mysite = mysite_get($profile->uid);
  $output =   '<div class="profile">';
  if ($profile->picture && in_array('mysite_picture', $allowed)) {
    $output .= theme('user_picture', $profile);
  }
  $output .= '<h4>'. l(t("!name", array('!name' => $profile->name)), 'user/'. $profile->uid) .'</h4>';
  $output .= '<small>'. t('Page updated ') . format_date($mysite->updated, 'custom', 'M d') .'</small></div>';
  $output .= '<div class="profile">';
  foreach ($fields as $category => $items) {
    if (!empty($items)) {
      if (strlen($category) > 0) {
        $output .= '<h4 class="title">'. $category .'</h4>';
      }
      $output .= '<dl>';
      foreach ($items as $item) {
        if (isset($item['title'])) {
          $output .= '<dt class="'. $item['class'] .'">'. $item['title'] .'</dt>';
        }
        $output .= '<dd class="'. $item['class'] .'">'. $item['value'] .'</dd>';
      }
      $output .= '</dl>';
    }
  }
  $output .= '</div>';
  return $output;
}

/**
 * Theme
 */
function theme_mysite_type_profile_anonymous($profile) {
  $output = theme('user_picture', $profile);
  $output .= '<p>'. t('Site members can display their personal information in this space. You will be able to select which elements of your personal profile to display to others.') .'</p>';
  $output .= '<p>'. l(t('Login to your account'), 'user/login') .'</p>';
  $output .= '<p>'. l(t('Sign up for an account'), 'user/register') .'</p>';
  return $output;
}
