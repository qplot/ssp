<?php if ($title == 'Access denied') { header( 'Location: /' ); } ?>
<?php
	global $user;
	$needs_to_login = (
		$user->uid == 0 &&
		arg(0) != 'user' &&
		strpos($head_title,'Page not found') === false
	);
?>
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language ?>" xml:lang="<?php print $language ?>">
    <head>
        <?php $head_title = $needs_to_login ? 'Please Sign In | Surazal Self-Service Portal' : $head_title; ?>
        <title>
            <?= $head_title ?>
        </title>
        <?php print $head ?>
        <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon" />
        <?php print $styles ?>
		<style media="print" type="text/css">
			* { color:#000 !important; }
			#hd,#nav,#ft { display:none; }
		</style>
		<script>var js_dir = "<?=$js_dir?>";/* used for loading additional js on fly */</script>
        <?php print $scripts ?>
    </head>
    <body class="<?= $body_class ?>">
        <div id="doc3" class="yui-t2">
            <div id="hd">
                <?php if ($site_name) { ?>
                <h1><a href="<?= $base_path ?>" id="sitename" title="<?= t('Home') ?>"><img src="<?=$img_dir?>logo.gif" width="244" height="166" alt="<?= $site_name ?>" /></a></h1>
                <?php } ?>
                <?php if ($site_slogan) { ?>
                <h2>
                    <?= $site_slogan ?>
                </h2>
                <?php } ?>
            </div>
            <!--/#hd-->
            <div id="nav">
                <?php if (isset($primary_links)) print theme('links', $primary_links, array('class' => 'links primary-links')) ?>
            </div>
            <!--#nav-->
            <div id="bd">
            	<?php global $user; if( count($user->og_groups) > 1 ): $cur_group = tnt_get_group(); ?>
					<div id="group-switcher">
					Viewing <strong><?= $cur_group->title ?></strong>
					<a href="#" id="switch-me">Switch?</a>
					<form style="display:none;" method="get">
					<select name="group">
						<?php $cur_group = tnt_get_group(); ?>
						<?php foreach($user->og_groups as $group ): ?>
							<?php if( $cur_group->nid != $group['nid'] ):?>
							<option value="<?= $group['nid'] ?>"><?= $group['title'] ?></option>
							<?php endif; ?>
						<?php endforeach ?>
					</select>
					<input type="submit" value="Switch" />
					or <a href="#" id="switch-cancel">Cancel</a>
					</form>
					</div>
				<?php endif; ?>
                <?php $tabs = ''; ?>
                <?php
  if( $needs_to_login )
  {
  	$title = 'Please Sign In';
	$content = '<p>Please use the form on the left to sign in.</p>'.
	'<p>If you do not have a login please contact <a target="_blank" href="http://surazalsystems.com/contact/">Surazal Systems Inc.</a></p>';
  }
?>
                <?php $main_content = '<h2>'.$title.'</h2><div class="tabs">'.$tabs.'</div><!--/.tabs-->'.$help.$content; ?>
                <div id="yui-main">
                    <div class="<?=( $sidebar_left )?'yui-b':'' ?>">
                        <?php if ($sidebar_right): ?>
                        <div class="yui-gc">
                            <div class="yui-u first">
                                <?= $main_content ?>
                            </div>
                            <!--/.yui-u.first -->
                            <div class="yui-u sidebar" id="right-sidebar">
                                <?= $sidebar_right ?>
                            </div>
                            <!--/#right-sidebar.yui-u-->
                        </div>
                        <!--/.yui-gc-->
                        <?php else: /*else if no sidebar */ ?>
                        <div class="yui-g">
                            <?= $main_content ?>
                        </div>
                        <!--/.yui-g wide-column-->
                        <?php endif;/*if ($sidebar_right)*/ ?>
                    </div>
                    <!--/.yui-b-->
                </div>
                <!--/#yui-main -->
                <?php if( $sidebar_left ): ?>
                <div class="yui-b sidebar" id="left-sidebar">
                    <?= $sidebar_left ?>
                </div>
                <!--/#left-sidebar.yui-b-->
                <?php endif; ?>
            </div>
            <!--/#bd-->
            <div id="ft">
                <?php if (isset($secondary_links)) print theme('links', $secondary_links, array('class' => 'links secondary-links')) ?>
                <?= $footer_message ?>
                <div id="copyright">
                    Copyright &copy; 2007-
                    <?= date('Y') ?>
                    Surazal Systems Inc. 
                </div>
                <!--/#copyright -->
            </div>
            <!--/#ft-->
        </div>
        <!--/#doc3.yui-t2-->
        <?= $closure ?>
        <script type="text/javascript" src="<?= $js_dir ?>global.js"></script>
        <?php if( arg(0) == 'heat' && arg(1) =='tickets' ): ?>
        <script type="text/javascript" src="<?= $js_dir ?>date.js"></script>
        <script type="text/javascript" src="<?= $js_dir ?>heat.js"></script>
		<?php endif; ?>
    </body>
</html>
