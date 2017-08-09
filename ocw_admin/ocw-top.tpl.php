<?php
// $Id: node.tpl.php,v 1.7 2007/08/07 08:39:36 goba Exp $
?>

<div id="top-logo-block" style="background-image: url('<?php print $base_path.$background_image; ?>');">
  <div id="top-site-teaser" style="background-image: url('<?php print $base_path.$theme_path.'/img/logo.png'; ?>');"><?php print $teaser; ?></div>
  <div id="top-site-explain"></div>
  <div id="top-site-explain-content">
    <?php print '<img src="'.$base_path.$theme_path.'/img/logo_small.png"/>' ?>
    <?php print $explain; ?>
  </div>
</div>
<div id="ocw-top-contents-block">
  <div class="blank-line"></div>
  <div id="ocw-top-left-content" style="width: <?php print $news_width; ?>px">
    <div class="title">News</div>
    <div class="ocw-top-content"><?php print $news; ?></div>
  </div>
  <div id="ocw-top-right-content" style="width: <?php print $links_width; ?>px">
    <div class="title">Links</div>
    <div class="ocw-top-content"><?php print $links; ?></div>
  </div>
</div>
