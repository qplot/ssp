<?php
// $Id: theme.inc,v 1.12 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Allows users to switch themes on their MySite pages.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * Native to MySite.  Does not provide content.
 */
function mysite_type_theme($get_options = TRUE) {
  $type = array(
    'name' => t('Theme'),
    'description' => t('<b>Theme Switching</b>: Enables users to choose a theme for MySite.'),
    'include' => 'theme',
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

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_theme_active($type) {
  // must be more than one active theme
  $themes = system_theme_data();
  $options = array();
  foreach ($themes as $info) {
    if ($info->status) {
      $options[] = 'yes';
    }
  }
  if (count($options) > 1) {
    $value = TRUE;
  }
  else {
    $value = FALSE;
    $message = l(t('There must be more than one enabled theme.'), 'admin/build/themes');
    $br = '<br />';
  }
  // there must be more than one configured theme
  $allowed = variable_get('mysite_theme_settings', array());
  // the $allowed array contains either strings or zeros (0); remove the zeros and count
  $new = array_flip($allowed);
  unset($new[0]);
  if (count($new) < 2) {
    $value = FALSE;
    $message .= $br . l(t('More than one MySite theme must be configured.'), 'admin/settings/mysite/type/theme');
  }
  return array($type => $value, 'message' => $message);
}

 /**
 * Implements mysite_type_hook_settings().
 */
function mysite_type_theme_settings() {
  mysite_check_settings('theme');
  return drupal_get_form('mysite_type_theme_settings_form');
}

/**
 * FormsAPI for mysite_type_theme_settings
 */
function mysite_type_theme_settings_form() {
  $themes = system_theme_data();
  $allowed = mysite_type_theme_data($themes);
  $form['mysite_theme_settings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed themes'),
    '#default_value' => variable_get('mysite_theme_settings', array()),
    '#options' => $allowed['name'],
    '#required' => FALSE,
    '#description' => array('desc' => t('Select the themes that users may select for their MySite page.'), 'screens' => $allowed['screenshot'])
  );

  return system_settings_form($form);
}

/**
 * Theme the settings form.
 */
function theme_mysite_type_theme_settings_form($form) {
  $rows = array();
  $screens = $form['mysite_theme_settings']['#description']['screens'];
  $form['mysite_theme_settings']['#description'] = $form['mysite_theme_settings']['#description']['desc'];
  foreach (element_children($form['mysite_theme_settings']) as $key) {
    $row = array();
    $row[] = $screens[$key];
    $row[] = drupal_render($form['mysite_theme_settings'][$key]);
    $rows[] = $row;
  }
  $table = theme('table', array(), $rows);
  $buttons = drupal_render($form['buttons']);
  $top = drupal_render($form);
  $output = $top . $table . $buttons;
  return $output;
}

/**
 * Build the form element for the user selection of themes
 *
 * Invoked by mysite_edit_form().
 *
 * @param $themes
 * An array of themes allowed by the MySite administrator
 * @return
 * Radio button options for allowed themes, including screenshots.
 */
function mysite_type_theme_data($themes = array()) {
  $default = system_theme_data();
  $options = array();
  foreach ($default as $info) {
    if ($info->status && !empty($themes[$info->name])) {
      // check screenshot for use of the color module
      $info->screenshot = dirname($info->filename) .'/screenshot.png';
      if (variable_get('color_'. $info->name .'_screenshot', 0)) {
        $info->screenshot = variable_get('color_'. $info->name .'_screenshot', 0);
      }
      $screenshot = file_exists($info->screenshot) ? theme('image', $info->screenshot, t('Screenshot for %theme theme', array('%theme' => $info->name)), '', array('class' => 'screenshot'), FALSE) : t('no screenshot');
      $options['name'][$info->name]= ucwords(str_replace('_', ' ', $info->name));
      $options['screenshot'][$info->name] = $screenshot;
    }
  }
  return $options;
}
