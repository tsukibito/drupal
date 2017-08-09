<?php

function ocw_admin_foot(&$form_state) {
	$contents = _get_content_values('body', 'type = "admin" and tag like "footer%"', 'tag');
	$form = array();

	$form['footer_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'フッターコンテンツの設定',
		'#collapsible' => FALSE,
		'#collapsed' => TRUE,
	);

	$form['footer_field']['footer_left_content'] = array(
		'#title' => '左側',
		'#type' => 'textarea',
		'#description' => 'フッターの左側に表示される文章',
		'#default_value' => $contents['footer_left']['body'],
		'#weight' => -10,
	);

	$form['footer_field']['footer_content'] = array(
		'#title' => '中央',
		'#type' => 'textarea',
		'#description' => 'フッターの中央に表示される文章',
		'#default_value' => $contents['footer']['body'],
		'#weight' => -9,
	);

	$form['footer_field']['footer_right_content'] = array(
		'#title' => '右側',
		'#type' => 'textarea',
		'#description' => 'フッターの右側に表示される文章',
		'#default_value' => $contents['footer_right']['body'],
		'#weight' => -8,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);

	return $form;
}

function ocw_admin_foot_submit($form, &$form_state) {
	$values = $form_state['values'];

	_update_content_values(NULL, 'footer_left', array('type' => 'admin', 'body' => $values['footer_left_content']));
	_update_content_values(NULL, 'footer', array('type' => 'admin', 'body' => $values['footer_content']));
	_update_content_values(NULL, 'footer_right', array('type' => 'admin', 'body' => $values['footer_right_content']));

	drupal_set_message('設定を保存しました');
}
