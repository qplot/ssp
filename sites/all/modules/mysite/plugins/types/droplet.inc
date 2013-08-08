<?php
// $Id: droplet.inc,v 1.22 2008/04/06 23:08:27 agentken Exp $

/**
 * @file
 * Creates custom content for MySite and handles blocks and views.
 *
 * @ingroup mysite_plugins
 */

/**
 * Implements mysite_type_hook().
 *
 * This plugin is native to MySite.
 */
function mysite_type_droplet($get_options = TRUE) {
  $type = array(
    'name' => t('Droplet'),
    'description' => t('<b>Droplets</b>: Content defined by the site administrator.'),
    'include' => 'droplet',
    'prefix' => t(''),
    'suffix' => t(''),
    'category' => t('MySite'),
    'weight' => -10,
    'form' => FALSE,
    'label' => t('Add Droplet'),
    'help' => t('Droplets are helpful tools that you can add to your personal page.  Search by name or select from the list provided.'),
    'search' => TRUE,
  );
  $basic_settings = variable_get('mysite_basic_droplet_settings', array());
  $type = array_merge($type, $basic_settings);
  if ($get_options) {
    $type['options'] = mysite_type_droplet_options();
  }
  return $type;
}

/**
 * Implements mysite_type_hook_active().
 */
function mysite_type_droplet_active($type) {
  // there must be at least one droplet
  $count = db_result(db_query("SELECT COUNT(myid) FROM {mysite_content} WHERE type = 'droplet'"));
  if ($count > 0) {
    return array($type => TRUE);
  }
  else {
    return array($type => FALSE, 'message' => l(t('You must create a Droplet before activating this content type.'), 'admin/settings/mysite/type/droplet'));
  }
}

/**
 * Implements mysite_type_hook_options().
 */
function mysite_type_droplet_options() {
  $options = array();
  $sql = "SELECT myid, title FROM {mysite_content} WHERE type = 'droplet' ORDER BY title";
  $result = db_query($sql);
  while ($item = db_fetch_object($result)) {
    $options['name'][] = mysite_type_droplet_title($item->myid, $item->title);
    $options['type_id'][] = $item->myid;
    $options['type'][] = 'droplet';
    $options['icon'][] = mysite_get_icon('droplet', $item->myid);
  }
  return $options;
}

/**
 * Implements mysite_type_hook_title().
 */
function mysite_type_droplet_title($type_id = NULL, $title = NULL) {
  if (!empty($type_id)) {
    if (is_null($title)) {
      $sql = "SELECT title FROM {mysite_content} WHERE myid = %d";
      $result = db_fetch_object(db_query($sql, $type_id));
      $title = $result->title;
    }
    $type = mysite_type_droplet(FALSE);
    $title = $type['prefix'] .' '. $title .' '. $type['suffix'];
    $title = trim(rtrim($title));
    return $title;
  }
  drupal_set_message(t('Could not find droplet title'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_data().
 */
function mysite_type_droplet_data($type_id = NULL, $settings = NULL) {
  if (!empty($type_id)) {
    // get the stored data for the droplet
    $droplet = mysite_get_custom('droplet', $type_id);
    // pass $droplet by reference in order to parse the subtype and key
    $content = mysite_type_droplet_get_content($droplet);
    $data = array(
      'base' => $droplet->base,
      'xml' => $droplet->xml
      );
    $items = array();
    $i = 0;
    $items[$i]['type'] = 'droplet';
    $items[$i]['link'] = NULL;
    $items[$i]['title'] = check_plain($droplet->title);
    $items[$i]['content'] = $content;
    $data['items'] = $items;
    return $data;
  }
  drupal_set_message(t('Could not find droplet data'), 'error');
  return;
}

/**
 * Implements mysite_type_hook_search().
 */
function mysite_type_droplet_search($uid = NULL) {
  if (!is_null($uid)) {
    $output .= drupal_get_form('mysite_type_droplet_search_form', $uid);
    return $output;
  }
}

/**
 * FormsAPI for mysite_type_droplet_search
 */
function mysite_type_droplet_search_form($uid) {
  $form['add_droplet']['droplet_title'] = array('#type' => 'textfield',
    '#title' => t('Search by name'),
    '#maxlength' => 64,
    '#size' => 40,
    '#description' => t('The name of the content droplet you wish to add.'),
    '#required' => FALSE, // this must be false, since all the search for are the same form
    '#autocomplete_path' => 'autocomplete/mysite/droplet'
  );
  $form['add_droplet']['uid'] = array('#type' => 'hidden', '#value' => $uid);
  $form['add_droplet']['type'] = array('#type' => 'hidden', '#value' => 'droplet');
  $form['add_droplet']['submit'] = array('#type' => 'submit', '#value' => t('Add Content'));
  return $form;
}

/**
 * Implements mysite_type_hook_search_form_submit().
 */
function mysite_type_droplet_search_form_submit($form_id, $form_values) {
  // we use LIKE here in case JavaScript autocomplete support doesn't work.
  // or in case the user doesn't autocomplete the form
  $result = db_query("SELECT myid, title FROM {mysite_content} WHERE type = 'droplet' AND title LIKE LOWER('%s%%')", $form_values['droplet_title']);
  $count = 0;
  while ($droplet = db_fetch_object($result)) {
    $data[$count]['uid'] = $form_values['uid'];
    $data[$count]['type'] = $form_values['type'];
    $data[$count]['type_id'] = $droplet->myid;
    $data[$count]['title'] = mysite_type_droplet_title($droplet->myid, $droplet->title);
    $data[$count]['description'] = check_plain($droplet->title);
    $count++;
  }
  // pass the $data to the universal handler
  mysite_search_handler($data, 'droplet');
  return;
}

/**
 * Implements mysite_type_hook_autocomplete().
 */
function mysite_type_droplet_autocomplete($string) {
  $matches = array();
  $result = db_query_range("SELECT myid, title FROM {mysite_content} WHERE type = 'droplet' AND title LIKE LOWER('%s%%')", $string, 0, 10);
  while ($droplet = db_fetch_object($result)) {
    $matches[$droplet->title] = check_plain($droplet->title);
  }
  return $matches;
}

/**
 * Implements mysite_type_hook_clear().
 */
function mysite_type_droplet_clear($type) {
  // fetch all the active records of this type and see if they really exist in the proper table
  $sql = "SELECT mid, uid, type_id, title FROM {mysite_data} WHERE type = '%s'";
  $result = db_query($sql, $type);
  $data = array();
  while ($item = db_fetch_array($result)) {
    $sql = "SELECT myid FROM {mysite_content} WHERE type = 'droplet' AND myid = %d";
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
function mysite_type_droplet_settings() {
  if (empty($_POST)) {
    mysite_check_settings('droplet');
  }
  $arg = arg(5);
  if (is_numeric($arg)) {
    $droplet_id = $arg;
    $output = drupal_get_form('mysite_type_droplet_settings_form', $droplet_id);
  }
  elseif ($arg == 'new') {
    $output = drupal_get_form('mysite_type_droplet_settings_form', NULL);
  }
  elseif ($arg == 'delete') {
    $id = arg(6);
    $output = drupal_get_form('mysite_type_droplet_settings_form', $id);
  }
  else {
    $header = array(t('Droplet'), t('Subtype'), t('Actions'), t('Original'));
    if ($use_icons = module_exists('mysite_icon')) {
      $header[] = t('Icon');
    }
    $sql= "SELECT myid, type_key, title FROM {mysite_content} WHERE type='droplet' ORDER BY title";
    $result = pager_query($sql . tablesort_sql($header), 25);
    $rows = array();
    $omit = array();
    while ($data = db_fetch_object($result)) {
      $type =  mysite_type_droplet_type($data->type_key);
      $type_key = mysite_type_droplet_key($data->type_key);
      $display = TRUE;
      switch ($type) {
        case 'view':
          $title = db_result(db_query("SELECT name FROM {view_view} WHERE vid = %d", $type_key));
          if ($title) {
            $original = l($title, 'admin/build/views/edit/'. $type_key);
          }
          else {
            drupal_set_message(t('%title view no longer exists and has been removed from your Droplets.', array('%title' => $data->title)));
            db_query("DELETE FROM {mysite_content} WHERE myid = %d", $data->myid);
            $display = FALSE;
          }
          break;
        case 'block':
          $info = explode('|', $type_key);
          $func = $info[0] .'_block';
          if (function_exists($func)) {
            $block = $func('list');
            $original = l($block[$info[1]]['info'], 'admin/build/block/configure/'. implode('/', $info));
          }
          else {
            drupal_set_message(t('%title block no longer exists and has been removed from your Droplets.', array('%title' => $data->title)));
            db_query("DELETE FROM {mysite_content} WHERE myid = %d", $data->myid);
            $display = FALSE;
          }
          break;
        default:
          $key = '';
          $original = '';
          break;
      }
      if ($display) {
        $row = array(check_plain($data->title), $type, l('edit', 'admin/settings/mysite/type/droplet/'. $data->myid) .' | '. l('delete', 'admin/settings/mysite/type/droplet/delete/'. $data->myid), $original);
        if ($use_icons) {
          $icon = mysite_icon_lookup('droplet', $data->myid);
          if ($icon->iid) {
            $icon_links =  l(t('change'), 'admin/settings/mysite/icons/change/'. $icon->iid) .' | '. l(t('delete'), 'admin/settings/mysite/icons/delete/'. $icon->iid);
          }
          else {
            $icon_links =  l(t('add icon'), 'admin/settings/mysite/icons/add/droplet/'. $data->myid);
          }
          $row[] = $icon_links;
        }
        $rows[] = $row;
        $omit[] = mysite_type_droplet_type($data->type_key) .':'. mysite_type_droplet_key($data->type_key);
      }
    }
    $output = t('<h3>Create New Droplet</h3>');
    $output .= '<ul>';
    $output .= '<li>'. l(t('Create a new droplet'), 'admin/settings/mysite/type/droplet/new');
    $output .= '<p>'. t('You may copy code from <a href="!url">Google gadgets</a> or <a href="!wdb">WidgetBox</a>
      into a new droplet.  Be sure to use the <em>Full HTML</em> input format.',
      array('!url' => 'http://www.google.com/ig/directory?synd=open&source=gapi', '!wdb' => 'http://widgetbox.com/')) .'</p></li>';
    $output .= '</ul>';

    // add droplet based on a block
    $blocks = _block_rehash();
    sort($blocks);
    $options = array();
    foreach ($blocks as $block) {
      $key =  'block:'. $block['module'] .'|'. $block['delta'];
      if (!in_array($key, $omit)) {
        $options[$key .'~+~'. $block['info']] = $block['info'];
      }
    }
    if (!empty($options)) {
      $output .= t('<h3>Create Droplet from existing blocks</h3>');
      $output .= drupal_get_form('mysite_type_droplet_block_create', $options);
    }

    // add droplet based on a view
    if (module_exists('views')) {
      $options = array();
      $result = db_query("SELECT vid, name, description, menu_title, page_title, block_title, url, page, menu, block FROM {view_view} ORDER BY name");
      $views = array();
      while ($view = db_fetch_object($result)) {
        $views[] = $view;
      }
      uasort($views, '_views_sort_arrays');
      foreach ($views as $view) {
        $key =  'view:'. $view->vid;
        if (!in_array($key, $omit) && !empty($view->page_title)) {
          $options[$key .'~+~'. $view->page_title] = $view->page_title .' : '. $view->description;
        }
      }
      if (!empty($options)) {
        $output .= t('<h3>Create Droplet from existing views</h3>');
        $output .= drupal_get_form('mysite_type_droplet_view_create', $options);
      }
    }

    $output .= t('<h3>Current Droplets</h3>');
    if (!empty($rows)) {
      $output .= theme('table', $header, $rows);
      $output .= theme_pager(array(), 25);
    }
    else {
      $output .= t('<p>No droplets have been created.</p>');
    }
  }
  return $output;
}

/**
 * FormsAPI for Droplet creation from a block
 */
function mysite_type_droplet_block_create($options) {
    $form['submit'] = array('#type' => 'submit', '#value' => t('Create Droplet'), '#prefix' =>  '<div style="margin: 0 20px 0 20px"><div style="float: right;">', '#suffix' => '</div>');
    $form['blockid'] = array(
      '#type' => 'select',
      '#title' => t('Block to transform'),
      '#options' => $options,
      '#suffix' => '</div>',
      '#description' => t('Select the appropriate block to create a new droplet.'),
    );
  return $form;
}

/**
 * FormsAPI for Droplet creation from a block
 */
function mysite_type_droplet_block_create_submit($form_id, $form_values) {
  $split = explode('~+~', $form_values['blockid']);
  $string = $split[0];
  $title = $split[1];
  $myid = mysite_get_myid($type = 'droplet', $string, $title);
  drupal_goto('admin/settings/mysite/type/droplet/'. $myid);
}

/**
 * FormsAPI for Droplet creation from a view
 */
function mysite_type_droplet_view_create($options) {
    $form['submit'] = array('#type' => 'submit', '#value' => t('Create Droplet'), '#prefix' =>  '<div style="margin: 0 20px 0 20px"><div style="float: right;">', '#suffix' => '</div>');
    $form['viewid'] = array(
      '#type' => 'select',
      '#title' => t('View to transform'),
      '#options' => $options,
      '#suffix' => '</div>',
      '#description' => t('Select the appropriate view to create a new droplet. <br />NOTE: Droplets cannot be created from <em>default views</em>. <br />Not all views make good droplets.'),
    );
  return $form;
}

/**
 * FormsAPI for Droplet creation from a view
 */
function mysite_type_droplet_view_create_submit($form_id, $form_values) {
  $split = explode('~+~', $form_values['viewid']);
  $string = $split[0];
  $title = $split[1];
  $myid = mysite_get_myid($type = 'droplet', $string, $title);
  drupal_goto('admin/settings/mysite/type/droplet/'. $myid);
}

/**
 * FormsAPI for droplet creation and deletion
 */
function mysite_type_droplet_settings_form($droplet_id = NULL) {
  if (arg(5) != 'delete') {
    if (!is_null($droplet_id)) {
      $droplet = mysite_get_custom('droplet', $droplet_id);
      // pass $droplet by reference in order to parse the subtype and key
      $content = mysite_type_droplet_get_content($droplet);
      $base_disabled = FALSE;
      if ($droplet->subtype == 'view') {
        $view = views_load_view($droplet->key);
        $droplet->base = $view->url;
        $base_disabled = TRUE;
      }
    }
    else {
      $droplet->subtype = 'droplet';
      $droplet->type_key = 'droplet:';
      $droplet->format = 2;
    }
    if (empty($edit)) {
      $edit = (array) $droplet;
      $edit['content'] = $droplet->content_raw;
    }
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['mysite_droplet'] = array(
      '#type' => 'fieldset',
      '#title' => t('Droplet settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['mysite_droplet']['title'] = array('#type' => 'textfield',
      '#title' => t('Droplet title'),
      '#default_value' => $edit['title'],
      '#maxlength' => 64,
      '#size' => 40,
      '#description' => t('The name of the droplet, as seen by the end user.'),
      '#required' => TRUE
    );
    if ($droplet->subtype == 'droplet') {
      if (!empty($droplet->myid)) {
        if (empty($content)) {
          $content = t('There is no content to render.');
        }
        $form['mysite_droplet']['preview'] = array('#type' => 'markup',
          '#prefix' => t('<p><b>Droplet preview: </b></p><p><hr /></p><p>'),
          '#value' => $content,
          '#suffix' => t('</p><p><hr /></p><p class="description">Content preview.</p>'),
          '#required' => FALSE
        );
      }
      $form['mysite_droplet']['content'] = array('#type' => 'textarea',
        '#title' => t('Droplet content'),
        '#default_value' => $edit['content'],
        '#rows' => 5,
        '#cols' => 40,
        '#description' => t('The content to display to the end user.'),
        '#required' => TRUE
      );
      $form['mysite_droplet']['format'] = filter_form($value = $edit['format'], $weight = NULL, $parents = array('format'));
    }
    else {
      if (empty($content)) {
        $content = t('There is no content to render.');
      }
      $form['mysite_droplet']['content'] = array('#type' => 'markup',
        '#prefix' => t('<p><b>Droplet content: </b></p><p><hr /></p>'),
        '#value' => $content,
        '#suffix' => t('<p><hr /></p><p class="description">This content cannot be edited.</p>'),
        '#required' => FALSE
      );
    }
    $form['mysite_droplet']['base'] = array('#type' => 'textfield',
      '#title' => t('Droplet link'),
      '#default_value' => $edit['base'],
      '#maxlength' => 255,
      '#size' => 40,
      '#description' => t('Optional: The URL to click for more of this content.  Use http:// or a Drupal-formatted "path/to/item".'),
      '#required' => FALSE,
      '#disabled' => $base_disabled
    );
    $form['mysite_droplet']['xml'] = array('#type' => 'textfield',
      '#title' => t('Droplet xml link'),
      '#default_value' => $edit['xml'],
      '#maxlength' => 255,
      '#size' => 40,
      '#description' => t('Optional: The path to the RSS/XML output for this content.  Use http:// or a Drupal-formatted "path/to/item".'),
      '#required' => FALSE
    );
    if (module_exists('mysite_icon') && $edit['myid']) {
      $icon = mysite_icon_lookup('droplet', $edit['myid']);
      $image = theme_mysite_icon('droplet', $droplet_id, $droplet->title, $icon);
      $link = l('Edit droplet icon', 'admin/settings/mysite/icons/change/'. $icon->iid) .' | '. l('Delete droplet icon', 'admin/settings/mysite/icons/delete/'. $icon->iid);
      if (!$icon->iid) {
        $link = l('Add droplet icon', 'admin/settings/mysite/icons/add/droplet/'. $edit['myid']);
      }
      $form['mysite_droplet']['image'] = array(
          '#value' => $image .'<p><b>'. t('Droplet Icon:') .'</b></p><p>'. $link .'</p>'
      );
    }
    $form['mysite_droplet']['type_key'] = array(
      '#type' => 'hidden', '#value' => $edit['type_key']
    );
    $form['mysite_droplet']['type'] = array(
      '#type' => 'hidden', '#value' => 'droplet'
    );
    $form['mysite_droplet']['myid'] = array(
      '#type' => 'hidden', '#value' => $edit['myid']
    );
    $form['mysite_droplet']['confirm'] = array(
      '#type' => 'hidden', '#value' => 0
    );
    $form['mysite_droplet']['submit'] = array('#type' => 'submit', '#value' => t('Save Droplet'));
    if (!empty($droplet->myid)) {
      $form['mysite_droplet']['delete'] = array('#type' => 'submit', '#value' => t('Delete This Droplet'));
    }
  }
  else {
    $droplet_id = arg(6);
    $droplet = mysite_get_custom('droplet', $droplet_id);
    if (!empty($droplet->myid)) {
      $form['mysite_droplet']['myid'] = array(
        '#type' => 'hidden', '#value' => $droplet->myid
      );
      $form['mysite_droplet']['confirm'] = array(
        '#type' => 'hidden', '#value' => 1
      );
      $form['mysite_droplet']['content'] = array('#type' => 'markup',
        '#value' => t('<p>You are about to delete the droplet "@title."  Do you wish to continue?</p>', array('@title' => $droplet->title)),
      );
      $form['mysite_droplet']['delete'] = array('#type' => 'submit', '#value' => t('Confirm Deletion'));
      $form['mysite_droplet']['cancel'] = array('#type' => 'markup',
        '#value' => l(t('Cancel'), 'admin/settings/mysite/type/droplet/'. $droplet->myid)
      );
    }
    else {
      $form['mysite_droplet']['content'] = array('#type' => 'markup',
        '#value' => t('<p>An invalid selection has been detected.</p>'),
      );
    }
  }
  return $form;
}

/**
 * FormsAPI handler for mysite_type_droplet_settings_form()
 */
function mysite_type_droplet_settings_form_validate($form_id, &$form_values) {
  $sql = "SELECT myid FROM {mysite_content} WHERE title = '%s'";
  $result = db_fetch_object(db_query($sql, $form_values['title']));
  if ($form_values['op'] != $form_values['delete'] && !empty($result->myid) && $form_values['myid'] != $result->myid) {
    form_set_error('title', t('Another droplet already exists with the specified title.'));
  }
}

/**
 * FormsAPI handler for mysite_type_droplet_settings_form()
 */
function mysite_type_droplet_settings_form_submit($form_id, &$form_values) {
  // insert a new droplet
  if ($form_values['myid'] == 0 && $form_values['submit'] == $form_values['op']) {
    // create a unique type_key for droplets
    $type_key = $form_values['type_key'] . md5($form_values['title']);
    $form_values['myid'] = mysite_get_myid($form_values['type'], $type_key, $form_values['title'],  $form_values['content'],  $form_values['format'],  $form_values['base'],  $form_values['xml']);
    $message = t('Your droplet has been created successfully.');
  }
  elseif ($form_values['submit'] == $form_values['op']) {
    // prepare the content variable for update
    if (!empty($form_values['content'])) {
      $content = serialize($form_values['content']);
    }
    $sql = "UPDATE {mysite_content} SET title = '%s', content = '%s', format = %d, base = '%s', xml = '%s' WHERE myid = %d";
    db_query($sql, $form_values['title'], $content, $form_values['format'], $form_values['base'], $form_values['xml'], $form_values['myid']);
    $message = t('Your droplet has been updated successfully.');
  }
  elseif ($form_values['delete'] == $form_values['op']) {
    // DELETE
    if ($form_values['confirm'] == 0) {
      drupal_goto('admin/settings/mysite/type/droplet/delete/'. $form_values['myid']);
    }
    $sql = "DELETE from {mysite_content} WHERE myid = %d";
    db_query($sql, $form_values['myid']);
    $sql = "DELETE from {mysite_data} WHERE type = 'droplet' AND type_id = %d";
    db_query($sql, $form_values['myid']);
    drupal_set_message(t('Your droplet has been deleted.'));
    return 'admin/settings/mysite/type/droplet';
  }
  drupal_set_message($message);
  return 'admin/settings/mysite/type/droplet/'. $form_values['myid'];
}

/**
 * Abstraction helper for deriving content from a multi-key type_id
 *
 * Blocks and Views Droplets are stored in the {mysite_content} table with compound keys in the form $type:$type_id. We take the $droplet by reference, create its keys and return its content.
 *
 * @param &$droplet
 * A droplet object passed by reference.
 * @return
 * The code required to generate the content for this item.  May be HTML, PHP, or JavaScript.
 */
function mysite_type_droplet_get_content(&$droplet) {
  // parse the droplet type_id to get the sub-type and key.
  $droplet->subtype = mysite_type_droplet_type($droplet->type_key);
  $droplet->key = mysite_type_droplet_key($droplet->type_key);
  // droplet content must be fetched according to type.
  $content = module_invoke('mysite_type_droplet', $droplet->subtype .'_content', $droplet);
  return $content;
}

/**
 * Helper function to parse the type_key
 *
 * @param $type_key
 * The type_key taken from {mysite_content}
 * @return
 * The content type (droplet, block, or view)
 */
function mysite_type_droplet_type($type_key) {
  $subtype = explode(':', $type_key);
  $type = $subtype[0];
  return $type;
}

/**
 * Helper function to parse the type_key
 *
 * @param $type_key
 * The type_key taken from {mysite_content}
 * @return
 * The content type_id for the droplet type
 */
function mysite_type_droplet_key($type_key) {
  $subtype = explode(':', $type_key);
  $key = $subtype[1];
  return $key;
}

/**
 * Helper function for getting droplet data
 *
 * @param $droplet
 * A droplet object
 * @return
 * The content to render, after filtering appropriately
 * @ingroup includes
 */
function mysite_type_droplet_droplet_content($droplet) {
  return check_markup($droplet->content, $droplet->format, FALSE);
}

/**
 * Helper function for getting block data
 *
 * @param $droplet
 * A droplet object
 * @return
 * The block content to render, filtered by the parent module.
 */
function mysite_type_droplet_block_content($droplet) {
  // for blocks, the key is stored as "module|delta"
  $array = explode('|', $droplet->key);
  $block = module_invoke($array[0], 'block', 'view', $array[1]);
  return $block['content'];
}

/**
 * Helper function for getting views data
 *
 * @param $droplet
 * A droplet object
 * @return
 * The view content to render, filtered by the parent module.
 */
function mysite_type_droplet_view_content($droplet) {
  // for blocks, the key is stored as "module|delta"
  $view = views_load_view($droplet->key);
  $output = views_build_view('embed', $view, $args, $view->use_pager, $view->nodes_per_page);
  return $output;
}
