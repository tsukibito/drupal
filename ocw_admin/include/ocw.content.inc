<?php

function ocw_content_top(&$form_state) {
	$news_values = _get_content_values('options', 'type = "admin" and tag = "top_news"', 'tag');	
	$news_values = unserialize($news_values['top_news']['options']);
	
	$links_values = _get_content_values('options', 'type = "admin" and tag = "top_links"', 'tag');	
	$links_values = unserialize($links_values['top_links']['options']);
	
	$form = array();
	
	$body = theme('ocw_top', _get_top_news($news_values), _get_top_links($links_values));
	
	$form['body'] = array(
		'#type' => 'markup',
		'#value' => $body,
	);
	
	return $form;
}

function _get_top_news($news_values) {
	$output = '';
	
	if ($news_values) {
		$output.= '<div class="ocw-news-block">';
		foreach($news_values as $key => $values) {
			$output.= '<p>';
			$output.= $values['content'];
			$output.= '&nbsp;&nbsp;(' . $values['date']['year'] . '/' . $values['date']['month'] . '/'. $values['date']['day'] . ')';
			$output.= '</p>';
		}
		$output.= '</div>';
	}
	
	return $output;
}

function _get_top_links($links_values) {
	$output = '<div class="ocw-links-block">';
	$output.= _get_navigation_menu($links_values, '_blank', FALSE);
	$output.= '</div>';
	
	return $output;
}

function ocw_content_class_list(&$form_state) {
	$search_word = $_GET['search_word']? $_GET['search_word']: '';
	$order_key = $_GET['order_key']? $_GET['order_key']: 'faculty';
	$pre_order_key = $_GET['pre_order_key']? $_GET['pre_order_key']: '';
	$order_state = $_GET['order_state']? (int)$_GET['order_state']: 0;
	
	$reload = $_GET['reload'];
	
	$order_query = 'order by ' . $order_key . ($order_state? ' desc': '');
	$class_values = _get_class_values('*', _get_search_query($search_word), 'uid', $order_query);
	
	$center_body = '<div id="class-table-block">';
	$center_body.= _get_class_table($class_values, $order_key, $order_state);
	if ($search_word) {
		$center_body.= '<div style="text-align: right"><p><small><a href="#" onclick="search_class(1); return false;">全ての講義を表示する</a></small></p></div>';
	}
	$center_body.= '</div>';
	
	if (!$reload) {
		$search_button = array(
			'#type' => 'button',
			'#value' => '検索',
			'#attributes' => array(
				'onclick' => 'search_class(); return false;',
			)
		);
		
		$search_form = array(
			'class_search_form' => array(
				'#type' => 'textfield',
				'#title' => 'キーワード',
				'#description' => '半角空白でAND検索',
				'#size' => 30,
				'#field_suffix' => drupal_render($search_button),
				'#id' => 'class-search-form',
				'#attributes' => array(
					'style' => 'width: 210px;',
				),
			),
		);
		
		$right_body = _get_block_contents('講義を探す', drupal_render($search_form));
		
		$right_content_values = array_pop(_get_content_values('body', 'title = "<class-list>" and url = "-"'));
		$right_content_values = unserialize($right_content_values['body']);
		$right_body.= '<br />'. $right_content_values['right'];
		
		$body = theme('ocw_content', NULL, $center_body, $right_body);
	}
	else {
		$body = $center_body;
	}

	$form = array();
	
	$form['#attributes'] = array(
		'onsubmit' => 'search_class(); return false;'
	);
	
	$form['body'] = array(
		'#type' => 'markup',
		'#value' => $body,
	);

	$form['order_key'] = array(
		'#type' => 'hidden',
		'#id' => 'order_key',
		'#value' => $order_key,
	);
	
	$form['order_state'] = array(
		'#type' => 'hidden',
		'#id' => 'order_state',
		'#value' => $order_state,
	);
	
	$form['search_word'] = array(
		'#type' => 'hidden',
		'#id' => 'search_word',
		'#value' => $search_word,
	);
	
	return $form;
}

function _get_search_query($keywords) {
	if ($keywords) {
		if (strpos($keywords, ' ') !== FALSE) {
			$keywords = split(' ', $keywords);
		}
		else {
			$keywords = array($keywords);
		}
		
		$query = '';		
		for($i = 0; $i < count($keywords); $i++) {
			$query.= '(faculty like "%'.$keywords[$i].'%" or '
					.'class like "%'.$keywords[$i].'%" or '
					.'professor like "%'.$keywords[$i].'%" or '
					.'type like "%'.$keywords[$i].'%" or '
					.'syllabus like "%'.$keywords[$i].'%")';
			if ($i+1 < count($keywords)) {
				$query .= ' and ';
			}
		}
	}
	else {
		$query = NULL;
	}
	
	return $query;
}

function ocw_content_class(&$form_state, $uid, $content_type=NULL, $video_id=0) {
	$content_values = _get_content_values('body', 'tag = "settings_menu"', 'tag');
	$class_values = _get_class_values('*', 'uid='.$uid);
	$class_values = $class_values[$uid];
	
	$is_syllabus = $content_type === 'syllabus';
	
	if ($is_syllabus) {
		$content = $class_values['syllabus'];
	}
	else {
		$contents = unserialize($class_values['contents']);
		$content = _get_class_content_table($contents, $uid);
	}
	
	$profile = _get_class_profile($class_values);
	
	$content_uids = array();
	if ($contents) {
		foreach($contents as $class_num => $class_contents) {
			foreach($class_contents['contents'] as $content_num => $content_data) {
				$content_uids[$content_data['uid']] = $content_data['label'];
			}
		}
	}
	
	//$is_auto_play = $video_id? ' autoPlay="true"': '';
	$is_auto_play = $is_syllabus? FALSE: TRUE;
	if (!$video_id || !$content_uids[$video_id] || $is_syllabus) {
		$video_id = $class_values['video'];
	}
	if ($video_id) {
		$video_url = _get_content_values('url', 'uid='.$video_id);
		$video_url = $video_url[$video_id]['url'];
		
		$extension = _get_extension($video_url);
		if ($extension === 'flv') {
			$left_body = '<div id="ocw-class-video-block">'._get_video_tag($video_url, $is_auto_play).'<p>&nbsp;</p></div>';
		}
		else if ($extension === 'jpg' || $extension === 'png' || $extension === 'gif') {
			$left_body = '<div id="ocw-class-video-block"><img src="'. $video_url.'" width="100%" /><p>&nbsp;</p></div>';
		}
	}
	else {
		$left_body = '<div id="ocw-class-video-block"></div>';
	}
	
	$left_body.= _get_block_contents('講義概要', $profile) . '<br />';

	$contents = '<ul><li'.(!$is_syllabus? ' class="active"': '').'>'._set_content_field('OCW/class/'.$uid, 'ocw-center-block ocw-class-content-list', '講義ノート', !$is_syllabus).'</li>';
	$contents.= '<li'.($is_syllabus? ' class="active"': '').'>'._set_content_field('OCW/class/'.$uid.'/syllabus', 'ocw-center-block ocw-class-content-list ocw-class-video-block', 'シラバス', $is_syllabus).'</li><ul>';
	$left_body.= '<div id="ocw-class-content-list">' . _get_block_contents('コンテンツ', $contents) . '</div>';

	$body = theme('ocw_content', NULL, $content, $left_body);

	$form = array();
	$form['body'] = array(
		'#type' => 'markup',
		'#value' => $body,
	);
	
	drupal_set_title($class_values['class']);

	return $form;
}

function _get_class_table($class_values, $order_key=NULL, $order_state=0) {
	$path = base_path() . drupal_get_path('module', 'ocw_admin');
	if ($order_state === 0) {
		$oder_char = ' <img src="'.$path.'/include/icons/arrow-asc.png" alt="アイコンの並べ替え" title="降順で並び替える" width="13" height="13" />';
	}
	else if ($order_state === 1){
		$oder_char = ' <img src="'.$path.'/include/icons/arrow-desc.png" alt="アイコンの並べ替え" title="降順で並び替える" width="13" height="13" />';
	}
	else {
		$oder_char = '';
	}

	$column_names = array(
		'faculty' => '学部・研究科',
		'class' => '講義名',
		'professor' => '教員名',
		'type' => '区分',
	);

	$output = '<table class="class-table" border="0" cellpadding="0" cellspacing="0">';
	foreach($column_names as $key => $label) {
		$output .= "<th class=\"td-".$key."\"><a href=\"#\" onclick=\"order_class('".$key."'); return false;\" class=\"class-table-order\">".$label.($order_key==$key? $oder_char: '')."</a></th>";
	}
	
	$count = 0;
	if ($class_values) {
		foreach($class_values as $uid => $values) {
			if (!$values['open']) continue;
			
			$output.= '<tr>';
			foreach($column_names as $key => $label) {
				if ($key === 'class') {
					$output.= '<td class="td-class">';
					if ($values['url']) {
						$output.= '<a href="'.$values['url'].'" target="_blank">' . $values['class'] . '</a>';
					}
					else {
						$output.= l($values['class'], 'OCW/class/'.$uid);
					}
					$output.= '</td>';
				}
				else {
					$output.= '<td class="td-'.$key.'">'.$values[$key].'</td>';
				}
			}
			$output.= '</tr>';
			
			$count++;
		}
	}
		
	if ($count == 0) {
		$output.= '<tr><td colspan="4">条件に一致する講義は存在しないか公開されていません</td></tr>';
	}
	$output.= '</table>';

	return $output;
}

function _get_class_content_table($class_values, $uid) {
	$content_urls = _get_content_values('url', 'type="content"');
	$output = '<table class="contents-table" border="0" cellpadding="0" cellspacing="0">';
	
	if ($class_values) {
		foreach($class_values as $key => $values) {
			$output.= '<tr>';
			$output.= '<td class="td-label" colspan="3">'.$values['label'].'</td>';
			$output.= '</tr>';

			$output.= '<tr>';
			$cnt = 0;
			foreach($values['contents'] as $cid => $content) {
				if ($cnt > 0 && $cnt % 3 == 0) {
					$output.= '</tr><tr>';
				}
				$path = $content_urls[$content['uid']]['url'];
				$ext = _get_extension($path);
				
				$output.= '<td class="td-content-label"><ul><li class="content-html content-'. $ext .'">';
				if ($ext === 'flv') {
					$path = 'OCW/class/'.$uid.'/video/'.$content['uid'];
					$output.= _set_content_field($path, 'ocw-class-video-block', $content['label']);
				}
				else {
					$output.= l($content['label'], $path, array('attributes' => array('target' => '_blank')));
				}
				$output.= '</li></ul></td>';
				
				$cnt++;
			}
			
			if ($cnt % 3 != 0) {
				for ($i = 0; $i < 3-($cnt % 3); $i++) {
					$output.= '<td class="td-content-label"></td>';
				}
			}
			$output.= '</tr>';
		}
	}
	$output.= '</table>';

	return $output;
}

function _set_content_field($path, $id, $label, $active=FALSE) {
	$active_class = $active? ' class="active"': '';
	return '<a href="#" onclick="set_content_field(\''.$path.'\',\''.$id.'\'); return false;"'.$active_class.'>' . $label . '</a>';
}
