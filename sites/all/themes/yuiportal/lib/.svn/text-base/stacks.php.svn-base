<?php

function yuiportal_mysite_empty_column($mysite) {
  global $user;
  $output = t('This column is empty, you may <a href="!url">add content</a> to it', array('!url' => url('mysite/'. $mysite->uid .'/content/'. $mysite->page)));
  return $output;
}

function yuiportal_prepare_columns($mysite, $data = array(), $cols = 1) {
  global $user;
  if($user->uid == 0 ) return '';
  // reset the key to zero if there is only one column
  /*
  if (count($data) == 1) {
    sort($data);
    $new = $data;
  }
  else {
    // otherwise, slide existing data to a new array and unset the old
    $new = array();
    for ($i = 0; $i < $cols; $i++) {
      $new[$i] = $data[$i];
      unset($data[$i]);
    }
    // if there is leftover data, merge it into the last element of the new array;
    if (!empty($data)) {
      // set the columns correctly for the new keys
      $set = $cols - 1;
      if (empty($new[$set])) {
        $new[$set] = array();
      }
      // loop the remaining data and merge it to the last region
      foreach ($data as $array) {
        if (!empty($array)) {
          $new[$set] = array_merge($new[$set], $array);
        }
      }
    }
  }
  */
  $new = &$data;
  if ($cols > 1) {
    // handles empty regions gracefully
    for ($i = 0; $i < $cols; $i++) {
      if (empty($new[$i])) {
        if( false ){ //if (*/ user_access('administer mysite') || (user_access('edit mysite') && $user->uid == $mysite->uid)) {
          $new[$i][0]['title'] = t('Add content');
          $new[$i][0]['mid'] = NULL;
          $new[$i][0]['format'] = $mysite->format;
          $new[$i][0]['output']['items'][0]['content'] = theme('mysite_empty_column', $mysite);
        }
        else {
          // this should probably be themed
          $new[$i][0] = array();
        }
      }
    }
  }
  // We only have one column, so return it.
  else {
    $new = $new[0];
  }
  return $new;
}

function yuiportal_layout_stacks() {
  $data = array();
  $data['regions'] = array('0' => t('Main'), '1' => t('Left'), '2' => t('Right'),'3' => t('Footer'));
  $data['count'] = count($data['regions']);
  $data['name'] = t('Stacked');
  $data['description'] = t('One main area and two equal-width columns.');
  $data['image'] = 'stacks.png';
  return $data;
}
	
function yuiportal_mysite_stacks_layout($content) {
	// break the array into pieces
	$owner = $content['owner'];
	$mysite = $content['mysite'];
	$columns = mysite_layout_stacks();
	//print_r($content['data']);
	$data = yuiportal_prepare_columns($mysite, $content['data'], $columns['count']);
	//print_r($data);
	//$data = $content['data'];
	$header = $content['header'];
	// print the header message, if present
	if (isset($header)) {
		$output = '<div class="messages">'.	$header .'</div>';
	}
	// ajax-generated message class
	$output .= '<div class="mysite-ajax"></div>';	
	// cycle through the data sets and make columns
	for($i = 0; $i < count($data); $i++ ) {
		$set = $data[$i];
		$col = $i;
		switch($col)
		{
			default://main
				$output .= '<div class="yui-g" style="width:100%;float:right;">';
				break;
			case 1://first
				$output .= '<div class="yui-g" style="width:100%;float:right;"><div style="width:55%" class="yui-u first">';
				break;
			case 2:
				$output .= '<div style="width:43%" class="yui-u">';
				break;
		}
		
		$output .= '<div id="mysite-sort'.$col.'" class="col mysite-sortable">';
		if( is_array($set) )
		foreach ($set as $key => $value) {
			if( empty($value['title']) ) break;// go to next widget
				
			if ($value['mid'] && !$value['locked'])
				$output .= '<div class="mysite-group collapsible sortable-item" id="m'. $value['mid'] .'">';
			else
				$output .= '<div class="mysite-group collapsible">';
			
			$output .= '<span class="mysite-header">'. $value['title'] .'</span>';
			$output .= '<div class="mysite-content">';
			if (!empty($value['output'])) {
				if (!empty($value['output']['image'])) {
					$output .= $value['output']['image'];
				} 
				$output .= theme('mysite_'. $value['format'] .'_item', $value['output']['items']);
			}
			else {
				//$output .= t('<p>No content found.</p>');
			}
			$output .= '</div><!--/.mysite-content-->';
			$output .= '<div class="mysite-footer">';
			if (!empty($value['output']['base'])) {
				$output .= '<div class="mysite-footer-left">'. l(t('Read more'), $value['output']['base']) .'</div>';
			}
			$output .= ' <div class="mysite-footer-right">'. $value['actions'] .'</div> ';
			$output .= '</div><!--/.mysite-footer-->';		
			$output .= '</div><!--/.mysite-group-->';	 
		}
		$output .= '</div><!--/#col-'.$col.'-->';
		if($col == 0 )
			$output .= '</div><!--/.yui-g-->';
		else if( $col == 2 )
			$output .= '</div><!--/.yui-u--></div><!--/.yui-g-->';
		else
			$output .= '</div><!--/.yui-u-->';
	}
	$output .= '</div><!--/.yui-gc-->';
	print theme('page', $output, variable_get('mysite_fullscreen', 1));
	return;
}
