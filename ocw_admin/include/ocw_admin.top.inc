<?php

function ocw_admin_top(&$form_state) {
	$contents = _get_content_values('body, options', 'type = "admin" and tag like "top%"', 'tag');
	$form = array();

	$form['#attributes']['enctype'] = "multipart/form-data";
	
	$form['top_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'トップページの設定',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['top_field']['teaser'] = array(
		'#title' => 'ホームページ概要',
		'#type' => 'textarea',
		'#description' => 'タイトルページロゴの下に表示される文章',
		'#default_value' => $contents['top_teaser']['body'],
		'#required' => TRUE,
		'#weight' => -10,
	);

	$form['top_field']['explain'] = array(
		'#title' => 'OCW説明',
		'#type' => 'textarea',
		'#description' => 'タイトルページロゴの右に表示される文章',
		'#default_value' => $contents['top_explain']['body'],
		'#required' => TRUE,
		'#weight' => -9,
	);

	$form['image_field'] = array(
		'#type' => 'fieldset',
		'#title' => '背景画像の設定',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['image_field']['list']['#tree'] = TRUE;
	$form['image_field']['list']['#theme'] = 'ocw_admin_list_form';
	$list = _get_ocw_file_list('admin');
	$options = array();
	$default_value = '';
	foreach ($list as $file => $values) {
		$form['image_field']['list'][$file]['path'] = array('#value' => $values['path']);
		$form['image_field']['list'][$file]['name'] = array('#value' => $values['name']);
		$hash = _get_hash_file_string($values['path'].$values['name']);
		$options[$hash] = '';

		if ($hash === $contents['top_image']['options']) {
			$default_value = $hash;
		}
	}

	$form['image_field']['list']['image_default'] = array(
		'#type' => 'radios',
		'#options' => $options,
		'#default_value' => $default_value,
		'#required' => TRUE,
		'#title' => '背景画像'
	);

	$form['image_field']['upload'] = array(
		'#type' => 'file',
		'#title' => '<br />背景画像を選択',
		'#size' => 20,
	);

	$form['image_field']['upload_submit'] = array(
		'#type' => 'submit',
		'#value' => 'アップロード',
		'#submit' => array('ocw_admin_top_upload_submit'),
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);
	
	_set_ocw_admin_form($form);

	return $form;
}

function ocw_admin_top_submit($form, &$form_state) {
	$values = $form_state['values'];
	$selected_hash = $values['list']['image_default'];

	_update_content_values(NULL, 'top_teaser', array('type' => 'admin', 'body' => $values['teaser']));
	_update_content_values(NULL, 'top_explain', array('type' => 'admin', 'body' => $values['explain']));

	$list = _get_ocw_file_list('admin', $selected_hash);
	if ($list) {
		_update_content_values(NULL, 'top_image', array(
			'type' => 'admin',
			'url' => $list['path'].$list['name'],
			'options' => $selected_hash,
		));
	}
	else {
		drupal_set_message('ファイルが存在しません', 'error');
	}
	
	drupal_set_message('設定を保存しました');
}

function ocw_admin_top_upload_submit($form, &$form_state) {
	$fieldName = 'upload';

	if (isset($_FILES['files']) && is_uploaded_file($_FILES['files']['tmp_name'][$fieldName])) {
		$upload_path = variable_get('ocw_contents_path', file_directory_path().'/ocw') . '/admin';
		
		if (!is_dir($upload_path)) {
			$admin_path = file_create_path('ocw/admin');
			file_check_directory($admin_path, FILE_CREATE_DIRECTORY);
		}
		
		$validators = array(
			'file_validate_is_image' => array(),
		);

		if ($file = file_save_upload($fieldName , $validators)) {
			file_copy($file, $upload_path . '/' . $file->filename, FILE_EXISTS_RENAME);
			drupal_set_message('アップロードを完了しました');
		}
	}
}

function ocw_admin_top_image_delete_confirm(&$form_state, $selected_hash) {
	$list = _get_ocw_file_list('admin', $selected_hash);
	if ($list) {
		$path = $list['path'].$list['name'];
	}
	else {
		drupal_set_message('ファイルが存在しません', 'error');
		drupal_goto('admin/OCW/top');
	}
	
	$form['image_path'] = array(
		'#type' => 'value',
		'#value' => $path,
	);
	
	$form['form_start'] = array(
		'#type' => 'markup',
		'#value' => '<div class="confirm-form">'
	);
	
	$form['message'] = array(
		'#type' => 'markup',
		'#value' => t('<h2>本当に %title を削除してもよいですか？</h2>', array('%title' => basename($path))),
	);
	
	$form['preview'] = array(
		'#type' => 'markup',
		'#value' => '<img src="'.base_path().$path.'" width="300"/><br />',
	);
	
	$form = confirm_form($form,
		'削除確認',
		isset($_GET['destination']) ? $_GET['destination'] : 'admin/OCW/top',
		t('この操作は元に戻すことができません.'),
		'削除',
		'キャンセル'
	);
	
	$form['form_end'] = array(
		'#type' => 'markup',
		'#value' => '</div>'
	);
	
	_set_ocw_admin_form($form);

	return $form;
}

function ocw_admin_top_image_delete_confirm_submit($form, &$form_state) {
	unlink($form['image_path']['#value']);
	drupal_goto('admin/OCW/top');
}

function ocw_admin_top_news(&$form_state) {
	if (!$form_state['storage']) {
		$news_values_db = _get_content_values('options', 'type = "admin" and tag = "top_news"', 'tag');
		$form_state['storage']['news_values'] = unserialize($news_values_db['top_news']['options']);
	}
	
	$news_values = $form_state['storage']['news_values'];
	
	$form = array();
	
	$form['news_add_field'] = array(
		'#type' => 'fieldset',
		'#title' => '新しいニュースを追加',
		'#collapsible' => FALSE,
		'#weight' => $weight,
		'#attributes' => $attributes,
		'#weight' => -10,
	);
	
	$form['news_add_field']['news_content'] = array(
		'#type' => 'textfield',
		'#title' => '内容',
		'#size' => 80,
		'#default_value' => '',
		'#id' => 'news-item-content',
		'#weight' => -5,
	);

	$form['news_add_field']['news_date'] = array(
		'#type' => 'date',
		'#title' => '日付',
		'#default_value' => array(
			'year' => format_date(time(), 'custom', 'Y'),
			'month' => format_date(time(), 'custom', 'n'),
			'day' => format_date(time(), 'custom', 'j'),
		),
		'#id' => 'news-item-date',
		'#weight' => -4,
	);

	$form['news_add_field']['news_add'] = array(
		'#type' => 'submit',
		'#value' => '追加',
		'#id' => 'news-add-submit',
		'#weight' => -3,
		'#submit' => array('ocw_admin_top_news_add_submit'),
		'#validate' => array('ocw_admin_top_news_add_validate'),
	);
	
	$form['news_add_field']['news_modify_dummy'] = array(
		'#type' => 'submit',
		'#value' => '更新',
		'#submit' => array('ocw_admin_top_news_add_submit'),
		'#validate' => array('ocw_admin_top_news_add_validate'),
		'#attributes' => array('style' => 'display: none'),
	);
	
	$form['news_list'] = array(
		'#type' => 'fieldset',
		'#title' => 'ニュース一覧',
		'#collapsible' => FALSE,
		'#weight' => -5,
	);

	$form['news_list']['news_items']['#tree'] = TRUE;
	$form['news_list']['news_items']['#theme'] = 'ocw_news_form';
	$form['news_list']['news_items']['#weight'] = 0;
	
	if ($news_values) {
		foreach ($news_values as $key => $values) {
			$form['news_list']['news_items'][$key]['content'] = array('#value' => $values['content']);
			$form['news_list']['news_items'][$key]['date'] = array('#value' => $values['date']);
			$form['news_list']['news_items'][$key]['sort'] = array(
				'#type' => 'weight',
				'#delta' => count($news_values),
				'#default_value' => $key,
			);
		}
	}
	
	$form['edit_news_item'] = array(
		'#type' => 'hidden',
		'#value' => '-1',
		'#id' => 'edit-news-item',
	);
	
	$form['news_delete'] = array(
		'#type' => 'submit',
		'#id' => 'news-delete-submit',
		'#attributes' => array('style' => 'display: none'),
		'#submit' => array('ocw_admin_top_news_delete_submit'),
	);
	
	$form['delete_news_item'] = array(
		'#type' => 'hidden',
		'#value' => '-1',
		'#id' => 'delete-news-item',
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
		'#weight' => 10,
	);
	
	_set_ocw_admin_form($form);
	
	return $form;
}

function ocw_admin_top_news_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	_change_ocw_admin_top_news_form_order($post_values, $form_state['storage']);

	_update_content_values(NULL, 'top_news', array(
		'type' => 'admin',
		'options' => serialize($form_state['storage']['news_values']),
	));
	
	drupal_set_message('設定を保存しました');
}


function ocw_admin_top_news_add_validate($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$news_content = $post_values['news_content'];
	$news_date = $post_values['news_date'];
	
	if (!$news_content) {
		form_set_error('news_content', 'ニュースの内容を入力してください');
	}
}

function ocw_admin_top_news_add_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	_change_ocw_admin_top_news_form_order($post_values, $form_state['storage']);
	
	$edit_item_id = $form_state['storage']['edit_news_item'];
	
	if ($edit_item_id !== '-1') {
		$form_state['storage']['news_values'][$edit_item_id] = array(
			'content' => $form_state['storage']['news_content'],
			'date' => $form_state['storage']['news_date'],
		);		
	}
	else {
		$new_news = array(
			'content' => $form_state['storage']['news_content'],
			'date' => $form_state['storage']['news_date'],
		);
		
		if ($form_state['storage']['news_values']) {
			array_unshift($form_state['storage']['news_values'], $new_news);
		}
		else {
			$form_state['storage']['news_values'][] = $new_news;
		}
	}
}

function ocw_admin_top_news_delete_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	_change_ocw_admin_top_news_form_order($post_values, $form_state['storage']);
	
	$delete_item_id = $form_state['storage']['delete_news_item'];
	
	if ($delete_item_id !== '-1') {
		unset($form_state['storage']['news_values'][$delete_item_id]);
	}
}

function _change_ocw_admin_top_news_form_order($post_values, &$storage) {
	if ($storage) {
		$storage = array_merge($storage, $post_values);
	}
	else {
		$storage = $post_values;
	}
	
	if (count($storage['news_values']) && $storage['news_items']) {
		$menu_list = array();
		foreach($storage['news_items'] as $key => $weight) {
			$menu_list[$key] = $storage['news_values'][$key];
		}
		$storage['news_values'] = $menu_list;
	}
}

function ocw_admin_top_links(&$form_state) {
	$form = array();
	
	if (!$form_state['storage']) {
		$links_values_db = _get_content_values('options', 'type = "admin" and tag = "top_links"', 'tag');
		$form_state['storage']['ocw_navigation_menu'] = unserialize($links_values_db['top_links']['options']);
	}
	
	$form['links_form'] = _set_navigation_menu_form($form_state, -10, $title='リンク一覧');

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
		'#weight' => 10,
	);
	
	_set_ocw_admin_form($form);
	
	return $form;
}

function ocw_admin_top_links_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];	
	if ($form_state['storage']) {
		$form_state['storage'] = array_merge($form_state['storage'], $post_values);
	}
	else {
		$form_state['storage'] = $post_values;
	}
	
	_change_navigation_menu_form_order($form_state['storage']);
	
	_update_content_values(NULL, 'top_links', array(
		'type' => 'admin',
		'options' => serialize($form_state['storage']['ocw_navigation_menu']),
	));
	
	drupal_set_message('設定を保存しました');
}
