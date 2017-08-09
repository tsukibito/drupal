<?php

function ocw_admin_title(&$form_state) {
	$contents = _get_content_values('body, options', 'type = "admin" and tag like "title%"', 'tag');
	$form = array();

	$form['#attributes']['enctype'] = "multipart/form-data";

	$form['title_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'タイトルコンテンツの設定',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['title_field']['title_teaser'] = array(
		'#title' => 'ホームページ概要',
		'#type' => 'textarea',
		'#description' => 'タイトルページロゴの下に表示される文章',
		'#default_value' => $contents['title_teaser']['body'],
		'#required' => TRUE,
		'#weight' => -10,
	);

	$form['title_field']['title_explain'] = array(
		'#title' => 'OCW説明',
		'#type' => 'textarea',
		'#description' => 'タイトルページロゴの右に表示される文章',
		'#default_value' => $contents['title_explain']['body'],
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

		if ($hash === $contents['title_image']['options']) {
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

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);

	return $form;
}

function ocw_admin_title_submit($form, &$form_state) {
	$values = $form_state['values'];
	$title_hash = $values['list']['image_default'];

	_update_content_values(NULL, 'title_teaser', array('type' => 'admin', 'body' => $values['title_teaser']));
	_update_content_values(NULL, 'title_explain', array('type' => 'admin', 'body' => $values['title_explain']));
	_update_content_values(NULL, 'title_image', array('type' => 'admin', 'options' => $title_hash));

	$list = _get_ocw_file_list('admin');
	foreach ($list as $file => $values) {
		$path = $values['path'].$values['name'];
		$hash = _get_hash_file_string($path);
		if ($hash == $title_hash) {
			_update_content_values(NULL, 'title_image', array('type' => 'admin', 'url' => $path));
		}
	}
	
	drupal_set_message('設定を保存しました');
}

function ocw_admin_title_upload(&$form_state) {
	$form = array();
	$form['#attributes']['enctype'] = "multipart/form-data";

	$form['title_upload'] = array(
		'#type' => 'file',
		'#title' => '背景画像を選択',
		'#size' => 20,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'アップロード',
	);

	return $form;
}

function ocw_admin_title_upload_submit($form, &$form_state) {
	$fieldName = 'title_upload';

	if (isset($_FILES['files']) && is_uploaded_file($_FILES['files']['tmp_name'][$fieldName])) {
		$upload_path = variable_get('ocw_contents_path', file_directory_path().'/ocw') . '/admin';
		$validators = array(
			'file_validate_is_image' => array(),
		);

		if ($file = file_save_upload('title_upload', $validators)) {
			file_copy($file, $upload_path . '/' . $file->filename, FILE_EXISTS_RENAME);
			drupal_set_message('アップロードを完了しました');
		}
	}

	drupal_goto('admin/OCW/title');
}

function ocw_admin_title_image_delete_confirm(&$form_state, $list_id) {
	$list = _get_ocw_file_list('admin');

	$form['image_info'] = array(
		'#type' => 'value',
		'#value' => $list[$list_id]['path'].$list[$list_id]['name'],
	);

	return confirm_form($form,
		t('Are you sure you want to delete %title?', array('%title' => $list[$list_id]['name'])),
		isset($_GET['destination']) ? $_GET['destination'] : 'admin/OCW/title',
		t('This action cannot be undone.'),
		t('Delete'),
		t('Cancel')
	);
}

function ocw_admin_title_image_delete_confirm_submit($form, &$form_state) {
	unlink($form['image_info']['#value']);
	drupal_goto('admin/OCW/title');
}
