<?php
$click_function = $_GET['func'];
if (!$click_function) {
	$click_function = '[link]';
}
$exts = $_GET['ext'];
if (!$exts) {
	$exts = '';
}

echo get_contents_list($_GET['id'], $_GET['base'], $_GET['dir'], $click_function, $exts);

function get_contents_list($id, $base, $dir, $click_function, $exts) {
	$subject = @file_get_contents($base.$dir);
	if ($subject === FALSE) {
		return '<div class="contents-list"><p class="error"><b>ファイル一覧を取得できませんでした</b></p></div>';
	}
	
	$pattern = '/<tr><td.*alt="\[(?<type>.+?)\]".*<a\shref="(?<url>.+?)">(?<name>.+?)<\/a>.*<\/tr>/'; 
	preg_match_all($pattern, $subject, $matches);
	
	$output = '<div class="contents-list"><p>'. $base. $dir . '</p>';
	$output.= '<ul>';
	for($i = 0; $i < count($matches[0]); $i++) {
		$type = $matches['type'][$i];
		$url = $matches['url'][$i];
		$name = $matches['name'][$i];
		
		if ($url === '/') {
			continue;
		}
		
		if ($type === 'DIR') {
			$output.= '<li class="content-file ';
			
			if ($i == 0) {
				$path = $url;
				$name = "．．";
				
				$output.= 'dir-up">';
			}
			else {
				$path = $dir . $url;
				$name = substr($name, 0, strlen($name)-1);
				
				$output.= 'dir">';
			}
				
			$link = '<a href="#" onclick="get_contents_from_http(\''.$id.'\', \''.$base.'\', \''.$path.'\', \''.$click_function.'\', \''.$exts.'\'); return false;">';
			$output.= $link . $name . '</a>';
		}
		else {
			$ext = pathinfo($url, PATHINFO_EXTENSION);
			
			if ($exts) {
				$exts_r = array();
				if (strpos($exts, '|') === FALSE) {
					$exts_r[] = $exts;
				}
				else {
					$exts_r = preg_split("/\|/", $exts);
				}
				$is_exist = FALSE;
				foreach ($exts_r as $key => $ext_i) {
					if ($ext === $ext_i) {
						$is_exist = TRUE;
						break;
					}
				}
				
				if (!$is_exist) {
					$output.= '</li>';
					continue;
				}
			}
			
			$output.= '<li class="content-file content-'.$ext.'">';
			
			$link = str_replace("[link]", "'". $base.$dir.$url . "'", $click_function);
			if ($ext === 'flv') {
				$link = str_replace("http://", "rtmp://", $link);
			}
			$link = str_replace("\\'", "'", $link);
			
			$output.= '<a href="'. $link .'">'.$name.'</a>';
		}
		$output.= '</li>';
	}
	$output.= '</ul></div>';
	
	return $output;
}
?>