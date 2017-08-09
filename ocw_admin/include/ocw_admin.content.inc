<?php

function ocw_content_create(&$form_state) {
	$form = array();

	$form['contents_info_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'コンテンツの設定',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['contents_info_field']['title'] = array(
		'#title' => 'タイトル',
		'#type' => 'textfield',
		'#description' => 'コンテンツのタイトル',
		'#default_value' => '',
		'#required' => TRUE,
		'#weight' => -10,
	);

	$form['contents_info_field']['alias'] = array(
		'#title' => 'エイリアス',
		'#type' => 'textfield',
		'#description' => 'コンテンツのURLエイリアス',
		'#default_value' => '',
		'#required' => TRUE,
		'#weight' => -9,
	);

	$form['contents_info_field']['colmuns'] = array(
		'#title' => 'カラム',
		'#type' => 'radios',
		'#options' => array('0' => '2カラム', '1' => '3カラム'),
		'#default_value' => '1',
		'#required' => TRUE,
		'#weight' => -8,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);

	return $form;
}

function ocw_content_create_validate($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	
	$alias = $post_values['alias'];
	$old_alias = $form_state['storage']['old_alias'];
	
	if ($alias !== '-' && menu_get_item($alias) && (!$old_alias || $old_alias != $alias)) {
		form_set_error('alias', '入力されたエイリアスは既に存在するURLです');
	}
}

function ocw_content_create_submit($form, &$form_state) {
	$uid = $form_state['storage']['uid'];
	if (!$uid) {
		$uid = _get_next_uid('ocw_contents');
	}
	
	$tag = 'content_' . $uid;

	$post_values = $form_state['clicked_button']['#post'];
	_check_contents_list_values($post_values, $form_state['storage']);
	
	if ($form_state['storage']['menu_use'] === '0') {
		$menu_body = NULL;
	}
	else {
		$navigation_menu = $form_state['storage']['ocw_navigation_menu'];
		$menu_body = _get_navigation_menu($navigation_menu);
	}
		
	$center_body = get_ocw_contents_html($form_state['storage'], 'contents_center');
	if ($post_values['colmuns'] === '1') {
		$right_body = get_ocw_contents_html($form_state['storage'], 'contents_right');
	}
	else {
		$right_body = NULL;
	}
	
	_update_content_values(NULL, $tag, array(
			'type' => 'ocw_content',
			'title' => $post_values['title'],
			'body' => serialize(
				array(
					'menu' => $menu_body,
					'center' => $center_body,
					'right' => $right_body,
				)
			),
			'options' => serialize(
				array(
					'columns' => $post_values['colmuns'],
					'contents_menu' => array(
						'type' => $form_state['storage']['menu_use'],
						'items' => $form_state['storage']['ocw_navigation_menu'],
					),
					'contents_center' => array(
						'count' => $form_state['storage']['contents_center-count'],
						'body' => $form_state['storage']['contents_center'],
					),
					'contents_right' => array(
						'count' => $form_state['storage']['contents_right-count'],
						'body' => $form_state['storage']['contents_right'],
					)
				)
			),
			'url' => $post_values['alias']
		)
	);
	
	if ($post_values['alias'] !== '-') {
		path_set_alias('OCW/content/'.$uid);
		path_set_alias('OCW/content/'.$uid, $post_values['alias'], NULL, 'ja');
	}
	
	drupal_set_message('設定を保存しました');
	
	drupal_goto('OCW/content/'.$uid.'/edit');
}

function get_ocw_contents_html($storage, $field) {
	$output = '';

	if ($storage[$field]) {
		foreach($storage[$field] as $index => $forms) {
			$textarea_value = '';
			$textfield_value = '';
			foreach($forms as $id => $values) {
				switch($values['#type']) {
					case 'textarea': $textarea_value = $values['#default_value']; break;
					case 'textfield': $textfield_value = $values['#default_value']; break;
				}
			}
			
			switch($forms['contents_type']['#value']) {
				case 'text':
					$output .= '<div class="clear-box">'. $textarea_value . '</div>';
					break;
				case 'block':
					$output .= _get_block_contents($textfield_value, $textarea_value);
					break;
				case 'video':
					$output .= _get_video_tag($textfield_value, FALSE);
					break;
			}
		}
	}
	
	return $output;
}

function ocw_content_edit(&$form_state, $uid) {
	$form = array();

	if (!$form_state['storage']) {
		$content = _get_content_values('*', 'type="ocw_content" and uid='.$uid);
		$content = $content[$uid];
	
		$form_state['storage']['title'] = $content['title'];
		$form_state['storage']['alias'] = $content['url'] === '-'? '-': _get_ocw_content_alias($uid);
		
		$contents_options = unserialize($content['options']);
		$form_state['storage']['columns'] = $contents_options['columns'];
		
		$form_state['storage']['menu_use'] = $contents_options['contents_menu']['type'];
		$form_state['storage']['ocw_navigation_menu'] = $contents_options['contents_menu']['items'];
		
		$form_state['storage']['contents_center-count'] = $contents_options['contents_center']['count'];
		$form_state['storage']['contents_center'] = $contents_options['contents_center']['body'];
		$form_state['storage']['contents_right-count'] = $contents_options['contents_right']['count'];
		$form_state['storage']['contents_right'] = $contents_options['contents_right']['body'];
		
		$form_state['storage']['old_alias'] = $form_state['storage']['alias'];
		$form_state['storage']['uid'] = $uid;
		
		$form_state['storage']['navigation_menu_submit_function'] = '_check_contents_list_values';
	}
	else {
		_check_contents_list_field_values(array(), $form_state['storage'], 'contents_center');
		_check_contents_list_field_values(array(), $form_state['storage'], 'contents_right');
	}
	
	$form['div_start'] = array(
		'#type' => 'markup',
		'#value' => '<div id="main-block">',
		'#weight' => -15
	);

	$form['contents_info_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'コンテンツの設定',
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#weight' => -10
	);

	$form['contents_info_field']['title'] = array(
		'#title' => 'タイトル',
		'#type' => 'textfield',
		'#description' => 'コンテンツのタイトル',
		'#default_value' => $form_state['storage']['title'],
		'#required' => TRUE,
		'#weight' => -10,
	);

	$form['contents_info_field']['alias'] = array(
		'#title' => 'エイリアス',
		'#type' => 'textfield',
		'#description' => 'コンテンツのURLエイリアス',
		'#default_value' => $form_state['storage']['alias'],
		'#required' => TRUE,
		'#weight' => -9,
	);

	$form['contents_info_field']['colmuns'] = array(
		'#title' => 'カラム',
		'#type' => 'radios',
		'#options' => array('0' => '2カラム', '1' => '3カラム'),
		'#default_value' => $form_state['storage']['columns'],
		'#required' => TRUE,
		'#weight' => -8,
	);
	
	$form['menu_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'ナビゲーションメニュー設定',
		'#collapsible' => FALSE,
		'#weight' => -9,
	);
	
	$form['menu_field']['menu_use'] = array(
		'#title' => '使用するメニュー',
		'#type' => 'radios',
		'#options' => array(
			'0' => '共通メニュー',
			'1' => 'カスタムメニュー',
		),
		'#default_value' => $form_state['storage']['menu_use']? $form_state['storage']['menu_use']: '0',
		'#weight' => -10
	);
	
	$form['menu_field']['custom_menu_list'] = _set_navigation_menu_form($form_state, -9, 'カスタムメニュー', array(
			'style' => 'border: 1px solid #999; padding: 5px 10px'
		)
	);

	$form['table_start'] = array(
		'#value' => '<table><tr valign="top"><td style="width: 550px">',
		'#weight' => -8,
	);
	
	$form['column_center'] = _get_content_edit_form('contents_center', 'センターカラム', -7, $form_state['storage']);
	
	$form['table_center'] = array(
		'#value' => '</td><td style="width: 425px">',
		'#weight' => -6,
	);

	$form['right_center'] = _get_content_edit_form('contents_right', '右カラム', -5, $form_state['storage']);
	
	$form['table_end'] = array(
		'#value' => '</td></tr></table>',
		'#weight' => -4,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
		'#validate' => array('ocw_content_create_validate'),
		'#submit' => array('ocw_content_create_submit'),
	);
	
	$form['div_end'] = array(
		'#type' => 'markup',
		'#value' => '</div>',
		'#wight' => 15
	);
	
	$form['delete_field_id'] = array(
		'#type' => 'hidden',
		'#value' => '',
		'#id' => 'delete_field_id'
	);
	
	$form['delete_contents_id'] = array(
		'#type' => 'hidden',
		'#value' => '',
		'#id' => 'delete_contents_id'
	);
	
	return $form;
}

function _get_content_edit_form($id, $title, $weight, $storage) {
	$form_elements = array(
		'#type' => 'fieldset',
		'#title' => $title,
		'#collapsible' => FALSE,
		'#weight' => $weight,
	);
	
	$contents_list = $storage[$id];
	
	if ($contents_list) {
		$contents_list_form = array(
			'#tree' => TRUE,
			'#theme' => 'ocw_contents_list_form',
			'#weight' => 0,
		);
		
		foreach ($contents_list as $key => $values) {
			$dummy_temp = array();
			
			$contents_list_form[$key]['form'] = array('#value' => form_builder($id.'-'.$key, $values, $dummy_temp));
			$contents_list_form[$key]['sort'] = array(
				'#type' => 'weight',
				'#delta' => count($contents_list),
				'#default_value' => $key,
			);
		}
		
		$form_elements[$id.'-list'] = $contents_list_form;
	}
	else {
		$form_elements[$id.'-list'] = array(
			'#type' => 'markup',
			'#value' => '<b>コンテンツを追加してください</b>',
			'#weight' => 0,
		);
	}
	
	$radio_form = array(
		$id.'-radios' => array(
			'#type' => 'radios',
			'#options' => array(
				'0' => 'テキスト',
				'1' => 'ブロック',
				'2' => 'ビデオ',
			),
			'#default_value' => '0',
		),
	);
	
	$dummy_temp = array();
	$radio_form = form_builder($id, $radio_form, $dummy_temp);
	
	$form_elements[$id.'-radios-list'] = array(
		'#type' => 'markup',
		'#value' => '<table><tr><td>'
						.drupal_render($radio_form[$id.'-radios']['0']).'</td><td style="padding: 0px 10px">'
						.drupal_render($radio_form[$id.'-radios']['1']).'</td><td>'
						.drupal_render($radio_form[$id.'-radios']['2'])
					.'</td></tr></table>',
		'#weight' => 1,
	);
	
	$form_elements[$id.'-submit'] = array(
		'#type' => 'submit',
		'#value' => 'コンテンツを追加',
		'#name' => $id,
		'#submit' => array('_add_content_to_id'),
		'#weight' => 2,
	);
	
	$form_elements['delete_contents_submit'] = array(
		'#type' => 'submit',
		'#value' => 'コンテンツを削除',
		'#id' => 'delete_contents_submit',
		'#submit' => array('_delete_content_from_id', 0),
		'#attributes' => array('style' => 'display: none')
	);
			
	return $form_elements;
}

function _add_content_to_id($form, &$form_state) {
	$id = $form_state['clicked_button']['#name'];
	$post_values = $form_state['clicked_button']['#post'];
	unset($post_values[$id]);

	if (!$form_state['storage'][$id.'-count']) {
		$form_state['storage'][$id.'-count'] = 0;
	}
	
	_check_contents_list_values($post_values, $form_state['storage']);
	
	$content_type = $post_values["$id-radios"];
	$form_elements = array();
	
	$contents_id = $form_state['storage'][$id.'-count']++;
	$content_id = $id.'-'.$contents_id;
	
	if (!$form_state['storage'][$id]) {
		$form_state['storage'][$id] = array();
	}
	
	$delete_form = '<div class="delete-content-button" field_id="'.$id.'" contents_id="'.$contents_id.'">削除</div>';
	
	switch($content_type) {
		case 0:
			$form_state['storage'][$id][$contents_id] = array(
				'content-title-'.$content_id => array(
					'#type' => 'markup',
					'#value' => '<div class="edit-content-title" style="background-color: #eff;">テキスト'.$delete_form.'</div>',
				),
				'content-textarea-'.$content_id => array(
					'#type' => 'textarea',
					'#title' => 'コンテンツ',
					'#rows' => 15,
					'#cols' => 40,
					'#default_value' => ''
				),
				'contents_type' => array(
					'#type' => 'hidden',
					'#value' => 'text',
				),
			);
			break;
		case 1:
			$form_state['storage'][$id][$contents_id] = array(
				'content-title-'.$content_id => array(
					'#type' => 'markup',
					'#value' => '<div class="edit-content-title" style="background-color: #fef;">ブロック'.$delete_form.'</div>',
				),
				'content-textfield-'.$content_id => array(
					'#type' => 'textfield',
					'#title' => 'タイトル',
					'#size' => 30,
				),
				'content-textarea-'.$content_id => array(
					'#type' => 'textarea',
					'#title' => 'コンテンツ',
					'#rows' => 10,
					'#cols' => 40,
				),
				'contents_type' => array(
					'#type' => 'hidden',
					'#value' => 'block',
				),
			);
			break;
		case 2:
			$form_state['storage'][$id][$contents_id] = array(
				'content-title-'.$content_id => array(
					'#type' => 'markup',
					'#value' => '<div class="edit-content-title" style="background-color: #ffe;">ビデオ'.$delete_form.'</div>',
				),
				'content-textfield-'.$content_id => array(
					'#type' => 'textfield',
					'#title' => 'ビデオパス',
					'#size' => 50,
					'#id' => 'content-textfield-'.$content_id
				),
				'content-list-'.$content_id => array(
					'#type' => 'markup',
					'#value' => _get_ocw_contents_list('content-textfield-'.$content_id, '/ocw/flv/', 'flv'),
				),
				'contents_type' => array(
					'#type' => 'hidden',
					'#value' => 'video',
				),
			);
			break;
	}
}

function _delete_content_from_id($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];	
	_check_contents_list_values($post_values, $form_state['storage']);
	
	$field_id = $post_values['delete_field_id'];
	$contents_id = $post_values['delete_contents_id'];
	
	unset($form_state['storage'][$field_id][$contents_id]);
}

function _check_contents_list_values($post_values, &$storage) {
	if ($storage) {
		$storage = array_merge($storage, $post_values);
	}
	else {
		$storage = $post_values;
	}
	
	_check_contents_list_field_values($post_values, $storage, 'contents_center');
	_check_contents_list_field_values($post_values, $storage, 'contents_right');
	
	_change_navigation_menu_form_order($storage);
}

function _check_contents_list_field_values($post_values, &$storage, $field) {
	if ($storage[$field]) {
		foreach ($storage[$field] as $key => $values) {
			foreach ($values as $form_id => $form_values) {
				if ($form_values['#type'] !== 'markup') {
					$storage[$field][$key][$form_id]['#default_value'] = $storage[$form_id];
				}
			}
		}
	}
	
	if ($post_values[$field.'-list']) {
		$new_class = array();
		foreach ($post_values[$field.'-list'] as $key => $values) {
			$new_class[$key] = $storage[$field][$key];
		}
		$storage[$field] = $new_class;
	}
}

function ocw_content_delete(&$form_state, $uid) {
	$content_values = _get_content_values('title, body', 'uid='.$uid);
	$content_values = $content_values[$uid];

	$form['uid'] = array(
		'#type' => 'value',
		'#value' => $uid,
	);
	
	$form['form_start'] = array(
		'#type' => 'markup',
		'#value' => '<div class="confirm-form">'
	);
	$form['message'] = array(
		'#type' => 'markup',
		'#value' => t('<h2>本当に %title を削除してもよいですか？</h2>', array('%title' => $content_values['title'])),
	);
	
	$form = confirm_form($form,
		'削除確認',
		isset($_GET['destination']) ? $_GET['destination'] : 'admin/OCW/content',
		t('この操作は元に戻すことができません.'),
		'削除',
		'キャンセル'
	);
	
	$form['form_end'] = array(
		'#type' => 'markup',
		'#value' => '</div>'
	);

	return $form;

}

function ocw_content_delete_submit($form, &$form_state) {
	$uid = $form['uid']['#value'];
	
	db_query("DELETE FROM {ocw_contents} WHERE uid=%d", $uid);
	path_set_alias('OCW/content/'.$uid);
	
	drupal_goto('admin/OCW/content');
}

function ocw_content_list(&$form_state) {
	$form = array();

	$form['contents_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'コンテンツリスト',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['contents_field']['list']['#tree'] = TRUE;
	$form['contents_field']['list']['#theme'] = 'ocw_content_list_form';
	
	$list = _get_content_values('*', 'type = "ocw_content"');
	foreach ($list as $content => $values) {
		$form['contents_field']['list'][$content]['uid'] = array('#value' => $values['uid']);
		$form['contents_field']['list'][$content]['alias'] = array('#value' => _get_ocw_content_alias($values['uid']));
		$form['contents_field']['list'][$content]['title'] = array('#value' => $values['title']);
		$form['contents_field']['list'][$content]['body'] = array('#value' => $values['body']);
		$form['contents_field']['list'][$content]['columns'] = array('#value' => ((int)$values['options']+2));
	}

	return $form;
}

function ocw_content_view(&$form_state, $uid) {
	$content_values = _get_content_values('body, title', 'uid='.$uid);
	$content_body = unserialize($content_values[$uid]['body']);
	
	$body = theme('ocw_content', $content_body['menu'], _get_filter_html($content_body['center']), _get_filter_html($content_body['right']));
	
	$form = array();
	$form['body'] = array(
		'#type' => 'markup',
		'#value' => $body,
	);
	
	drupal_set_title($content_values[$uid]['title']);

	return $form;
}

function ocw_content_list_goto() {
	drupal_goto('admin/OCW/content');
}
