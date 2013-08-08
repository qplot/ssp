<?php
require_once('lib/stacks.php');

// Implementation of hook_form_alter
/*
function heat_ticket_form_alter($form_id, &$form) {
  die();
  if (isset($form['type']) && $form['type']['#value'] .'_node_settings' == $form_id) {
    $form['author']['#type'] = 'value';
    $form['author']['name'] = array('#type'=>'value', '#value'=>$form['author']['name']['#default_value']);
    $form['author']['date'] = array('#type'=>'value', '#value'=>$form['author']['date']['#default_value']);


    $form['options']['#type'] = 'value';
    $form['options']['status'] = array('#type'=>'value', '#value'=>$form['options']['status']['#default_value']);
    $form['options']['moderate'] = array('#type'=>'value', '#value'=>$form['options']['moderate']['#default_value']);
    $form['options']['promote'] = array('#type'=>'value', '#value'=>$form['options']['promote']['#default_value']);
    $form['options']['sticky'] = array('#type'=>'value', '#value'=>$form['options']['sticky']['#default_value']);
    $form['options']['revisions'] = array('#type'=>'value', '#value'=>$form['options']['revisions']['#default_value']);

    $form['user_comments']['#type'] = 'value';
    $form['user_comments']['comment'] = array('#type'=>'value', '#value'=>$form['user_comments']['comment']['#default_value']);

    $form['menu']['#type'] = 'value';
	$form['log']['#type'] = 'value';
  }
}*/

function selector($str)
{
	if( is_numeric($str) ) return 'n'.$str;
	return strtolower(preg_replace('/[^a-zA-Z0-9-]+/','-', trim($str)));
}

function yuiportal_mysite_teasers_item($item)
{
  $output = '';
  foreach ($item as $element) {
      $output .= $element['teaser'];
    }
	return $output;
}


function shorten($str)
{
	$abbr = array(
		'anonymous' => 'anon',
		'authenticated' => 'auth'
	);
	foreach( $abbr as $long => $short )
		$str = str_replace($long,$short,$str);
	return $str;
}
function _phptemplate_variables($hook, $vars = array() )
{
	global $user;
	
	/* CSS Cleanup */
	$css = drupal_add_css();
	//unset($css['all']['module']['modules/system/system.css']);
	unset($css['all']['module']['modules/system/defaults.css']);
	unset($css['all']['module']['modules/node/node.css']);
	unset($css['all']['module']['modules/user/user.css']);
	unset($css['all']['module']['modules/aggregator/aggregator.css']);
	unset($css['all']['module'][drupal_get_path('module','mysite').'/mysite.css']);
	unset($css['all']['module'][drupal_get_path('module','mysite').'/mysite_links.css']);
	unset($css['all']['module'][drupal_get_path('module','devel').'/devel.css']);
	unset($css['all']['module'][drupal_get_path('module','cck').'/date.css']);
	unset($css['all']['module'][drupal_get_path('module','cck').'/fieldgroup.css']);
	unset($css['all']['module'][drupal_get_path('module','og').'/og.css']);
	$vars['styles'] = drupal_get_css($css);

/*
 * <style type="text/css" media="all">@import "/sites/all/modules/devel/devel.css";</style>
<style type="text/css" media="all">@import "/modules/system/system.css";</style>
<style type="text/css" media="all">@import "/sites/all/modules/cck/content.css";</style>

<style type="text/css" media="all">@import "/sites/all/modules/date/date.css";</style>
<style type="text/css" media="all">@import "/sites/all/modules/og/og.css";</style>
<style type="text/css" media="all">@import "/sites/all/modules/cck/fieldgroup.css";</style>
<style type="text/css" media="all">@import "/sites/all/modules/mysite/mysite_links.css";</style>
<style type="text/css" media="all">@import "/sites/all/themes/yuiportal/style.css";</style>
 */


	/* Body Class */
	$body_class[] = $hook;
	$body_class[] = $vars['node']->type;
	if( $vars['node']->type == 'part' ) $body_class[] = 'product'; 
	$body_class[] = drupal_is_front_page() ? 'home' : 'inner';
		
	$i = 0;
	while ($arg = arg($i++))//e.g. catalog canopies
		$body_class[] = $arg;
	
	$body_class[] = 'layout-'.$vars['layout'];//e.g. layout-none, layout-left ( if sidebar present )
	
	foreach( $user->roles as $role )//e.g. authenticated-user
		$body_class[] = $role;
	
	$vars['logged_in'] = $user->uid > 0;
	

	
	$body_class = join(' ',array_unique(array_map("selector",$body_class)));//clean up the classes and join them.	
	$vars['body_class'] = shorten($body_class);
	
	/* Node Class */
	$node_class = array($vars['node']->type,$vars['node']->nid);
	if( !$vars['status']) $node_class[] = 'unpublished';
	if( $vars['sticky']) $node_class[] = 'sticky';
	$node_class = join(' ',array_map("selector",array_unique($node_class)));//clean up the classes and join them.
	$vars['node_class'] = shorten($node_class);
	
	/* Paths */
	$theme_path = base_path().path_to_theme();
	$vars['img_dir'] = $theme_path.'/img/';
	$vars['css_dir'] = $theme_path.'/css/';
	$vars['js_dir'] = $theme_path.'/js/';
	
	return $vars;
}

function yuiportal_user_profile($account, $fields) {
  drupal_goto('user/'.$account->uid.'/contact');
  $output = '<div class="profile">';
  $output .= theme('user_picture', $account);
  foreach ($fields as $category => $items) {
    if (strlen($category) > 0) {
      $output .= '<h2 class="title">'. check_plain($category) .'</h2>';
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
  $output .= '</div>';

  return $output;
}
?>