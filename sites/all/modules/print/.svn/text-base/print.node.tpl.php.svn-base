<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $node->language; ?>" xml:lang="<?php print $node->language; ?>">

  <head>
    <title><?php print $node->title; ?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <?php print $robots_meta; ?>
    <base href="<?php print $node->url ?>/" />
    <?php print theme_get_setting("toggle_favicon") ? "<link rel='shortcut icon' href='".theme_get_setting("favicon")."' type='image/x-icon'/>\n" : ""; ?>
    <style type="text/css">
      <?php if ($node->undefcss) {include_once($node->printcss);} else {print "@import url($node->printcss)\n";} ?>
    </style>
  </head>
  <body<?php $node->sendtoprinter ? print ' onload="window.print();"' : ''; ?>>

    <?php $node->logo ? print '<img class="print-logo" src="'.$node->logo.'" alt="" border="0" />' : '';?>

    <div class="print-site_name">
    <?php variable_get('site_name', 0) && print t('Published on').' '.variable_get('site_name', 0).' ('.l($base_url, $base_url).')'; ?>
    </div>

    <div class="print-title"><?php print $node->title; ?></div>

    <div class="print-submitted">
      <?php print theme_get_setting("toggle_node_info_$node->type") ? t('By').' '.$node->name : ''; ?>
    </div>

    <div class="print-created">
      <?php print theme_get_setting("toggle_node_info_$node->type") ? t('Created').' '.format_date($node->created, 'small') : '' ?>
    </div>

    <p />

    <div class="print-content">
      <?php print $node->body; ?>
    </div>

    <hr class="print-hr" />

    <div class="print-source_url">
      <?php 
         if ($node->source_url) { 
           print '<strong>';
           print t('Source URL');
           $node->printdate ? print t(' (retrieved on ' . format_date(time(), 'small') . ')') : '';
           print ':</strong> <a href="'.$node->source_url.'">'.$node->source_url.'</a>';
        } 
      ?>
    </div>

    <div class="print-links">
      <!-- Output printer friendly links -->
      <?php $node->pfp_links ? print '<p><strong>'.t('Links:').'</strong><br />'.$node->pfp_links.'</p>' : ''; ?>
    </div>

    <div class="print-footer">
      <!-- Add your custom footer here. -->
    </div>

  </body>
</html>
