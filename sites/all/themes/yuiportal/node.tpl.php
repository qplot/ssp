<div class="<?= $node_class ?>">
	<?php if ($page == 0): ?>
		<h2 class="title"><a href="<?= $node_url?>"><?= $title?></a></h2>
	<?php endif; ?>
	<?php if( $node->type != 'book' ):?>
	<span class="submitted"><?= $submitted ?></span>
	<?php endif; ?>
	<span class="taxonomy"><?= $terms ?></span>
	<div class="content"><?= $content ?></div>
	<?php if( user_access('edit page content') ): ?><p><a href="/node/<?= $node->nid ?>/edit">Edit This</a></p><?php endif; ?>
</div>