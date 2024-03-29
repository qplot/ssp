<?php

/**
 * @file Workflow-ng configuration forms for the node module 
 */

/*
 *  Check for content types - Configuration form
 */
function workflow_ng_condition_content_is_type_form($settings = array()) {
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Content types'),
    '#options' => node_get_types('names'),
    '#multiple' => TRUE,
    '#default_value' => isset($settings['type']) ? $settings['type'] : array(),
    '#required' => TRUE,
  );
  return $form;
}

function workflow_ng_condition_content_is_type_submit($form_id, $form_values) {
  return array('type' => $form_values['type']);
}

/*
 * Action node publish configuration form
 */
function workflow_ng_action_node_publish_form($settings = array(), $argument_info) {
  $form = array();
  $form['published'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => $settings['published'],
  );
  return $form;
}

function workflow_ng_action_node_publish_submit($form_id, $form_values) {
  return array('published' => $form_values['published']);
}

/*
 * Action node promote configuration form
 */
function workflow_ng_action_node_promote_form($settings = array(), $argument_info) {
  $form = array();
  $form['promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Promoted to front page'),
    '#default_value' => $settings['promote'],
  );
  return $form;
}

function workflow_ng_action_node_promote_submit($form_id, $form_values) {
  return array('promote' => $form_values['promote']);
}

/*
 * Action "Add a node" configuration form
 */
function workflow_ng_action_add_node_form($settings = array(), $argument_info) {
  $form = array();
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Content type to be used'),
    '#options' => node_get_types('names'),
    '#default_value' => $settings['type'],
    '#description' => t('Select a single content type that will be created when this action is invoked.'),
    '#required' => TRUE,
  );
  $form['node_access'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create content only if the acting user (i.e. logged in user) has access rights to do so'),
    '#default_value' => $settings['node_access'],
    '#description' => t('Enable this in order for the system to check if the user has access rights for creating the 
      required content. Note that even if the user arguments are set to \'content author \' the system will still check the \'acting user\'' ),
  );  
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $settings['title'],
    '#description' => t('Enter the title of the new created content.'),
    '#required' => TRUE,
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_add_node_submit($form_id, $form_values) {
  //returns the needed settings
  $settings = workflow_ng_token_get_settings(array('title'), $form_values);
  $settings += array('type' => $form_values['type'], 'node_access' => $form_values['node_access']);
  return $settings;
}

/*
 * Action "Load a node" configuration form
 */
function workflow_ng_action_load_node_form($settings = array(), $argument_info, $info) {
  $form = array();
  $form['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Content ID'),
    '#default_value' => $settings['nid'],
    '#required' => TRUE,
  );
  $form['vid'] = array(
    '#type' => 'textfield',
    '#title' => t('Content Revision ID'),
    '#default_value' => $settings['vid'],
    '#description' => t('If you want to load a specific revision. Leave it empty to load the current revision.'),
  );
  workflow_ng_token_replacement_help($form, $argument_info);
  return $form;
}

function workflow_ng_action_load_node_submit($form_id, $form_values) {
  //returns the needed settings
  return workflow_ng_token_get_settings(array('nid', 'vid'), $form_values);
}
