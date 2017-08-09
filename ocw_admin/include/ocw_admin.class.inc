<?php

function ocw_class_list(&$form_state) {
	$class_values = _get_class_values('*');
	$form = array();

	$form['list_field'] = array(
		'#type' => 'fieldset',
		'#title' => '公開科目リスト',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['list_field']['list']['#tree'] = TRUE;
	$form['list_field']['list']['#theme'] = 'ocw_class_list_form';
	
	$options = array();
	$default_value = array();
	foreach ($class_values as $class_id => $values) {
		foreach ($values as $key => $value) {
			$form['list_field']['list'][$class_id][$key] = array('#value' => $value);
		}
		$options[$class_id] = '';
		$default_value[] = $values['open']? $class_id: 0;
	}

	$form['list_field']['list']['open_status'] = array(
		'#title' => '公開',
		'#type' => 'checkboxes',
		'#options' => $options,
		'#default_value' => $default_value,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);

	_set_ocw_admin_form($form);
	
	return $form;
}

function ocw_class_list_submit($form, &$form_state) {
	$class_values = _get_class_values('open');
	$open_status = $form_state['values']['list']['open_status'];
	
	foreach($open_status as $uid => $value) {
		$open = ((int)$value > 0)? 1: 0;
		if ($class_values[$uid]['open'] != $open) {
			_update_class_values($uid, array(
					'open' => $open,
				)
			);
		}
	}
	
	drupal_set_message('設定を保存しました');
}

function ocw_class_info_edit(&$form_state, $uid=0) {
	$form = array();

	if ($uid > 0) {
		$class_values = _get_class_values('faculty, class, professor, type, url, profile', 'uid='.$uid);
		$class_values = $class_values[$uid];
		$class_values['profile'] = unserialize($class_values['profile']);
	}
	$form_state['storage']['uid'] = $uid;

	$form['basic_field'] = array(
		'#type' => 'fieldset',
		'#title' => '基本情報',
		'#collapsible' => FALSE,
		'#weight' => -10,
	);

	$form['basic_field']['faculty'] = array(
		'#title' => '学部・研究科',
		'#type' => 'textfield',
		'#default_value' => $uid > 0? $class_values['faculty']: '',
		'#required' => TRUE,
		'#size' => 30,
		'#weight' => -10,
		'#attributes' => array('onchange' => 'change_profile_preview("faculty", this.value);'),
	);

	$form['basic_field']['class'] = array(
		'#title' => '講義名',
		'#type' => 'textfield',
		'#default_value' => $uid > 0? $class_values['class']: '',
		'#required' => TRUE,
		'#size' => 30,
		'#weight' => -9,
		'#attributes' => array('onchange' => 'change_profile_preview("class", this.value);'),
	);

	$form['basic_field']['professor'] = array(
		'#title' => '教員名',
		'#type' => 'textfield',
		'#default_value' => $uid > 0? $class_values['professor']: '',
		'#required' => TRUE,
		'#size' => 15,
		'#weight' => -8,
		'#attributes' => array('onchange' => 'change_profile_preview("professor", this.value);'),
	);

	$form['basic_field']['type'] = array(
		'#title' => '区分',
		'#type' => 'textfield',
		'#default_value' => $uid > 0? $class_values['type']: '',
		'#required' => TRUE,
		'#size' => 15,
		'#weight' => -7,
	);

	$form['basic_field']['link_url'] = array(
		'#title' => '外部URL',
		'#type' => 'textfield',
		'#default_value' => $uid > 0? $class_values['url']: '',
		'#size' => 50,
		'#weight' => -6,
	);

	$form['profile_field'] = array(
		'#type' => 'fieldset',
		'#title' => '講義概要',
		'#collapsible' => FALSE,
		'#weight' => -8,
	);

	drupal_add_js(
		'set_profile_preview("'.$form['basic_field']['faculty']['#default_value'].'","'
							   .$form['basic_field']['class']['#default_value'].'","'
							   .$form['basic_field']['professor']['#default_value'].'"'
						   .');', 'inline');

	$preview_tag = '<br /><div id="profile-preview" class="tag-preview">'
					. $form['basic_field']['class']['#default_value'] . '<br />'
					. $form['basic_field']['faculty']['#default_value'] . '&nbsp;&nbsp;'
					. $form['basic_field']['professor']['#default_value']
				 . '</div>';

	$form['profile_field']['profile_radio'] = array(
		'#type' => 'radios',
		'#options' => array(
			0 => '自動 (講義名, 学部・研究科, 教員名)'. $preview_tag, 1 => '手動'),
		'#default_value' => $uid > 0? $class_values['profile']['radio']: 0,
		'#required' => TRUE,
		'#title' => '講義概要の生成',
		'#weight' => -10,
	);

	$form['profile_field']['profile'] = array(
		'#title' => '講義概要（手動）',
		'#type' => 'textarea',
		'#default_value' => $uid > 0? $class_values['profile']['content']: '',
		'#rows' => 10,
		'#weight' => -9,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);

	_set_ocw_admin_form($form);
	
	return $form;
}

function ocw_class_info_edit_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$uid = $form_state[storage]['uid'];
	$next_uid = _get_next_uid('ocw_classes');

	_update_class_values($uid > 0? $uid: NULL, array(
			'faculty' => $post_values['faculty'],
			'class' => $post_values['class'],
			'professor' => $post_values['professor'],
			'type' => $post_values['type'],
			'url' => $post_values['link_url'],
			'profile' => serialize(array(
				'radio' => $post_values['profile_radio']['#value'],
				'content' => $post_values['profile']
			))
		)
	);
	
	drupal_set_message('設定を保存しました');

	if ($uid == 0) {
		drupal_goto('admin/OCW/class/' . $next_uid . '/edit');
	}
}

function ocw_class_syllabus_edit(&$form_state, $uid=0) {
	$form = array();

	if ($uid > 0) {
		$class_values = _get_class_values('syllabus, video', 'uid='.$uid);
		$class_values = $class_values[$uid];
		
		$video_uid = (int)$class_values['video'];
		$video_url = _get_content_values('url', 'uid='.$video_uid);
		$video_url = $video_url[$video_uid]['url'];
	}
	$form_state['storage']['uid'] = $uid;

	$form['syllabus_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'シラバス',
		'#collapsible' => FALSE,
		'#weight' => -9,
	);

	if ($form_state['storage']['syllabus']) {
		$syllabus = $form_state['storage']['syllabus'];
		$video_url = $form_state['storage']['video'];
	}
	else {
		$syllabus = $uid > 0? $class_values['syllabus']: '';
		$video_url = $video_uid > 0? $video_url: '';
	}
	
	$form['syllabus_field']['syllabus'] = array(
		'#title' => '内容',
		'#type' => 'textarea',
		'#default_value' => $syllabus,
		'#rows' => 25,
		'#weight' => -10,
	);

	$form['video_field'] = array(
		'#type' => 'fieldset',
		'#title' => '紹介ビデオ',
		'#collapsible' => FALSE,
		'#weight' => -8,
	);
	
	$form['video_field']['video_path'] = array(
		'#title' => 'ビデオURL',
		'#type' => 'textfield',
		'#default_value' => $video_url,
		'#size' => 50,
		'#weight' => -10,
		'#field_suffix' => drupal_render($form['preview_submit']),
		'#id' => 'content_path_field',
	);

	$form['video_field']['file_tree'] = array(
		'#type' => 'markup',
		'#value' => _get_ocw_contents_list('content_path_field', '/ocw/flv/', 'flv|jpg|png|gif'),
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);

	return $form;
}

function ocw_class_syllabus_edit_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$uid = $form_state[storage]['uid'];

	_update_class_values($uid, array(
			'syllabus' => $post_values['syllabus'],
			'video' => $post_values['video_path']? _get_content_id($post_values['video_path']): 0,
		)
	);
	
	drupal_set_message('設定を保存しました');
}

function ocw_class_content_edit(&$form_state, $uid=0) {
	$form = array();

	if ($form_state['storage']['uid'] == NULL) {
		$form_state['storage']['uid'] = $uid;
	}

	if ($form_state['storage']['class_values'] == NULL) {
		$db_class_values = _get_class_values('contents', 'uid='.$uid);
		$db_class_values = $db_class_values[$uid]['contents'];
		
		$form_state['storage']['class_values'] = unserialize($db_class_values);
	}
	$class_values = $form_state['storage']['class_values'];
	
	if ($class_values === '') {
		$class_values = array();
	}
	
	$class_values = $class_values;

	$edit_id = $form_state['storage']['edit_id'];
	$is_edit = $edit_id != NULL && $edit_id >= 0;

	$edit_content_id = $form_state['storage']['edit_content_id'];
	$is_edit_content = $edit_content_id != NULL && $edit_content_id >= 0;

	$form['dummy_submit'] = array(
		'#type' => 'submit',
		'#value' => '講義ラベルを変更',
		'#submit' => array('ocw_class_content_edit_submit'),
		'#weight' => -15,
		'#attributes' => array('style' => 'display: none')
	);

	$form['new_content_field'] = array(
		'#type' => 'fieldset',
		'#title' => '新しい講義ラベルを作成',
		'#collapsible' => FALSE,
		'#weight' => -10,
	);

	$form['new_content_field']['label_submit'] = array(
		'#type' => 'submit',
		'#value' => '作成',
		'#validate' => array('ocw_class_label_add_validate'),
		'#submit' => array('ocw_class_label_add_submit'),
	);

	$form['new_content_field']['new_label'] = array(
		'#title' => '講義ラベル',
		'#type' => 'textfield',
		'#description' => '<p>空白の場合は [ 第*回 ]　(*は講義ラベル数+1)</p>',
		'#default_value' => '',
		'#size' => 15,
		'#weight' => -10,
		'#field_suffix' => drupal_render($form['new_content_field']['label_submit']),
	);

	$form['table_header'] = array(
		'#type' => 'markup',
		'#value' => '<table id="class-form-block"><tr><td><div id="class-form-block-left">',
		'#weight' => -9,
	);
	
	$form['content_field'] = array(
		'#type' => 'fieldset',
		'#title' => '講義内容',
		'#collapsible' => FALSE,
		'#weight' => -8,
	);

	if ($class_values) {	
		$class_list_form = array(
			'#tree' => TRUE,
			'#theme' => 'ocw_class_label_list_form',
			'#weight' => -10,
		);
		
		if ($is_edit) {
			$edit_class_label_form = array(
				'#type' => 'textfield',
				'#size' => 20,
				'#value' => $class_values[$edit_id]['label'],
				'#attributes' => array('onchange' => 'change_class_label('.$edit_id.', -1, this);')
			);
		}
		
		foreach ($class_values as $key => $values) {
			if (!$values) {
				unset($class_values[$key]);
				continue;
			}
			
			if ($is_edit && $edit_id == $key) {
				$class_list_form[$key]['label'] = array('#value' => drupal_render($edit_class_label_form));
			}
			else {
				$class_list_form[$key]['label'] = array('#value' => $class_values[$key]['label']);
			}
			$class_list_form[$key]['sort'] = array(
				'#type' => 'weight',
				'#delta' => count($class_values),
				'#default_value' => $key,
			);
		}
		
		$form['content_field']['list'] = $class_list_form;
	}
	if (!$class_values || !count($class_values)) {
		$form['content_field']['list'] = array(
			'#type' => 'markup',
			'#value' => '<div class="clear-box"><b>講義ラベルを追加してください</b></div>'
		);
	}

	$form['table_content'] = array(
		'#type' => 'markup',
		'#value' => '</div><div id="class-form-block-top">',
		'#weight' => -7,
	);
	
	$form['class_content_field'] = array(
		'#type' => 'fieldset',
		'#title' => '講義コンテンツ',
		'#collapsible' => FALSE,
		'#weight' => -6,
	);

	if ($is_edit) {
		$content_urls = _get_content_values('url', 'type="content"');
		
		if ($is_edit_content) {
			$edit_values = $class_values[$edit_id]['contents'][$edit_content_id];
			$edit_content_label_form = array(
				'#type' => 'textfield',
				'#size' => 10,
				'#value' => $edit_values['label'],
				'#attributes' => array('onchange' => 'change_class_label('.$edit_id.', '.$edit_content_id.', this);')
			);
			
			$edit_content_url = $content_urls[$edit_values['uid']]['url'];
			$edit_content_url_form = array(
				'#type' => 'textfield',
				'#size' => 50,
				'#value' => $edit_content_url,
				'#attributes' => array('readonly' => ''),
			);
		}

		$content_list_form = array();
		$content_list_form['#tree'] = TRUE;
		$content_list_form['#theme'] = 'ocw_class_content_list_form';
		$content_list_form['#weight'] = -10;

		foreach ($class_values[$edit_id]['contents'] as $key => $values) {
			if (!$values) {
				unset($class_values[$edit_id]['contents'][$key]);
				continue;
			}
			
			if ($is_edit_content && $edit_content_id == $key) {
				$content_list_form[$key]['label'] = array('#value' => drupal_render($edit_content_label_form));
				$content_list_form[$key]['path'] = array(
					'#value' => drupal_render($edit_content_url_form),
					'#ext' => _get_extension($edit_content_url),
					'#is_edit' => TRUE,
				);
			}
			else {
				$url = $content_urls[$values['uid']]['url'];
				$content_list_form[$key]['label'] = array('#value' => $values['label']);
				$content_list_form[$key]['path'] = array(
					'#value' => $url,
					'#ext' => _get_extension($url),
				);
			}
			$content_list_form[$key]['sort'] = array(
				'#type' => 'weight',
				'#delta' => count($class_values[$edit_id]['contents'])*2,
				'#default_value' => $key,
			);
		}
		$form['class_content_field']['content_list'] = $content_list_form;
	}
	else {
		$form['class_content_field']['content_list'] = array(
			'#type' => 'markup',
			'#value' => '<p><b>編集する講義ラベルを選択してください</b></p>',
		);
	}

	$form['table_explorer'] = array(
		'#type' => 'markup',
		'#value' => '</div><div id="class-form-block-down">',
		'#weight' => -5,
	);

	$form['add_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'コンテンツ',
		'#collapsible' => FALSE,
		'#weight' => -4,
	);

	$form['add_field']['content_path'] = array(
		'#type' => 'textfield',
		'#field_prefix' => 'URL：　&nbsp;',
		'#size' => 50,
		'#weight' => -10,
		'#maxlength'=> 256,
		'#id'=> 'content_path_field'
	);

	$form['add_field']['add_submit'] = array(
		'#type' => 'submit',
		'#value' => '追加',
		'#validate' => array('ocw_class_content_add_validate'),
		'#submit' => array('ocw_class_content_add_submit'),
	);

	$form['add_field']['content_label'] = array(
		'#type' => 'textfield',
		'#field_prefix' => 'ラベル：',
		'#field_suffix' => drupal_render($form['add_field']['add_submit']),
		'#size' => 15,
		'#weight' => -9,
	);

	$form['add_field']['content_upload'] = array(
		'#type' => 'markup',
		'#value' => _get_ocw_contents_list('content_path_field'),
	);

	$form['table_footer'] = array(
		'#type' => 'markup',
		'#value' => '</div></td></tr></table>',
		'#weight' => -3,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
		'#submit' => array('ocw_class_content_save_submit'),
		'#weight' => 10,
	);

	$form['edit_id'] = array(
		'#type' => 'hidden',
		'#value' => ($form_state['storage']['edit_id'] != NULL)? $form_state['storage']['edit_id']: -1,
		'#id' => 'edit_class_id',
	);
	
	$form['edit_content_id'] = array(
		'#type' => 'hidden',
		'#value' => ($form_state['storage']['edit_content_id'] != NULL)? $form_state['storage']['edit_content_id']: -1,
		'#id' => 'edit_class_content_id',
	);

	$form['pre_edit_id'] = array(
		'#type' => 'hidden',
		'#value' => ($form_state['storage']['edit_id'] != NULL)? $form_state['storage']['edit_id']: -1,
	);

	$form['pre_ocw_class_values'] = array(
		'#type' => 'hidden',
		'#value' => serialize($class_values),
	);
	
	$form['class_values_json'] = array(
		'#type' => 'hidden',
		'#value' => json_encode($class_values),
		'#id' => 'class_values_json',
	);
	
	$form['dummy_form_default_submit'] = array(
		'#type' => 'hidden',
		'#value' => '0',
		'#id' => 'dummy_form_default_submit',
		'#weight' => 19
	);
	
	return $form;
}

function ocw_class_label_add_validate($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$class_label = $post_values['new_label'];

	if ($class_label === '') {
		$form_state['clicked_button']['#post']['new_label'] = '第'.(count($post_values['list'])+1).'回';
	}
}

function ocw_class_label_add_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$class_values = array();

	_check_change_values($post_values, $class_values);

	$class_label = $post_values['new_label'];
	$class_values[] = array(
		'label' => $class_label,
		'contents' => array(),
	);

	$form_state['storage']['class_values'] = $class_values;
}

function ocw_class_content_add_validate($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$content_label = $post_values['content_label'];
	$content_path = $post_values['content_path'];
	
	if (!$content_path) {
		form_set_error('content_path', '追加するコンテンツURLを指定してください');
	}
	if ($content_label === '') {
		$form_state['clicked_button']['#post']['content_label'] = basename($content_path);
	}
}

function ocw_class_content_add_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$class_values = array();
	$edit_id = $post_values['edit_id'];

	_check_change_values($post_values, $class_values);

	if ($edit_id >= 0) {
		$content_label = $post_values['content_label'];
		$content_path = $post_values['content_path'];
		$content_uid = _get_content_id($content_path);
		
		$class_values[$edit_id]['contents'][] = array(
			'label' => $content_label,
			'uid' => $content_uid,
		);
	}

	$form_state['storage']['class_values'] = $class_values;
}

function _get_content_id($path) {
	$id = db_result(db_query("SELECT uid FROM {ocw_contents} WHERE type='content' and url='%s'", $path));
	if (!$id) {
		$id = _get_next_uid('ocw_contents');
		_update_content_values(NULL, NULL, array(
			'type' => 'content',
			'tag' => pathinfo($path, PATHINFO_EXTENSION),
			'url' => $path,
		));
	}
	
	return $id;
}

function ocw_class_content_edit_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$class_values = $form_state['storage']['class_values'];
	
	_check_change_values($post_values, $class_values);
	
	if ($post_values['edit_id'] !== '-1' || $post_values['edit_content_id'] !== '-1') {
		$form_state['storage']['edit_id'] = $post_values['edit_id'];
		$form_state['storage']['edit_content_id'] = $post_values['edit_content_id'];
	}
	
	$form_state['storage']['class_values'] = $class_values;
}

function ocw_class_content_save_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$class_values = $form_state['storage']['class_values'];
	
	_check_change_values($post_values, &$class_values);
	
	$uid = $form_state['storage']['uid'];
	_update_class_values($uid, array('contents' => serialize($class_values)));
	
	$form_state['storage'] = NULL;
	drupal_set_message('設定を保存しました');
}

function _check_change_values($post_values, &$class_values) {
	$class_label_id = $post_values['change_class_label_id'];
	$content_label_id = $post_values['change_content_label_id'];
	$change_labels_flag = $post_values['change_labels_flag'];
	$edit_id = $post_values['edit_id'];
	$pre_edit_id = $post_values['pre_edit_id'];

	$class_values = json_decode($post_values['class_values_json'], TRUE);
	
	if ($post_values['list']) {
		$new_class = array();
		foreach ($post_values['list'] as $key => $values) {
			$new_class[$key] = $class_values[$key];
		}
		$class_values = $new_class;
	}
	
	if ($post_values['content_list']) {
		$target_id =  $pre_edit_id === '-1'? $edit_id: $pre_edit_id;
		$new_contents = array();
		foreach ($post_values['content_list'] as $key => $values) {
			$new_contents[$key] = $class_values[$target_id]['contents'][$key];
		}
		$class_values[$target_id]['contents'] = $new_contents;
	}	
}

function ocw_class_delete(&$form_state, $uid=0) {
	$class_values = _get_class_values('faculty, class', 'uid='.$uid);
	$class_values = $class_values[$uid];

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
		'#value' => t('<h2>本当に %title を削除してもよいですか？</h2>', array('%title' => $class_values['faculty'].' '.$class_values['class'])),
	);
	
	$form = confirm_form($form,
		'削除確認',
		isset($_GET['destination']) ? $_GET['destination'] : 'admin/OCW/classes',
		t('この操作は元に戻すことができません.', array('%title' => $class_values['faculty'].' '.$class_values['class'])),
		'削除',
		'キャンセル'
	);
	
	$form['form_end'] = array(
		'#type' => 'markup',
		'#value' => '</div>'
	);

	return $form;
}

function ocw_class_delete_submit($form, &$form_state) {
	db_query("DELETE FROM {ocw_classes} WHERE uid=%d", $form['uid']['#value']);
	drupal_goto('admin/OCW/classes');
}

function ocw_class_list_goto() {
	drupal_goto('admin/OCW/classes');
}

