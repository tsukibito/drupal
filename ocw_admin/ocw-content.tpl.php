<?php
// $Id: node.tpl.php,v 1.7 2007/08/07 08:39:36 goba Exp $
?>
<div class="ocw-contents-block">
  <div style="width: <?php print $left_column; ?>px;" id="ocw-left-block">
    <div class="ocw-content"><?php print $left; ?></div>
  </div>

  <div style="width: <?php print $center_column; ?>px;" id="ocw-center-block">
    <div class="ocw-content"><?php print $center; ?></div>
  </div>

  <?php if ($right_column): ?>
  <div style="width: <?php print $right_column; ?>px;" id="ocw-right-block">
    <div class="ocw-content"><?php print $right; ?></div>
  </div>
  <?php endif; ?>
</div>
