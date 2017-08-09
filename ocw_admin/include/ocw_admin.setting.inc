<?php

function ocw_admin_settings(&$form_state) {
	$content_values = _get_content_values('body, url, options', 'type = "admin" and tag like "settings%"', 'tag');
	$content_column = unserialize($content_values['settings_columns']['options']);
	
	if (!$form_state['storage']) {
		$form_state['storage']['menu_column'] = $content_column['menu'];
		$form_state['storage']['content_column'] = $content_column['content'];
		
		$form_state['storage']['contents_domain'] = $content_values['settings_contents']['options'];
		$form_state['storage']['contents_root'] = $content_values['settings_contents']['url'];
		
		$form_state['storage']['stream'] = $content_values['settings_stream']['options'];
		$form_state['storage']['stream_path'] = $content_values['settings_stream']['url'];
		
		$navigation_menu = unserialize($content_values['settings_menu']['options']);
		$form_state['storage']['ocw_navigation_menu'] = $navigation_menu? $navigation_menu: array();
	}
	
	$menu_column = $form_state['storage']['menu_column'];
	$content_column = $form_state['storage']['content_column'];
	$contents_domain = $form_state['storage']['contents_domain'];
	$contents_root = $form_state['storage']['contents_root'];
	$stream = $form_state['storage']['stream'];
	$stream_path = $form_state['storage']['stream_path'];
	$navigation_menu = $form_state['storage']['ocw_navigation_menu'];

	$form = array();

	$form['column_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'カラム設定',
		'#collapsible' => FALSE,
		'#weight' => -10,
	);

	$form['column_field']['menu_column'] = array(
		'#type' => 'textfield',
		'#title' => 'メニューカラム',
		'#description' => '全コンテンツ共通',
		'#field_suffix' => 'px',
		'#size' => 10,
		'#default_value' => $menu_column? $menu_column: 140,
		'#weight' => -10,
	);

	$form['column_field']['content_column'] = array(
		'#type' => 'textfield',
		'#title' => 'コンテンツカラム',
		'#description' => '3カラム表示用',
		'#field_suffix' => 'px',
		'#size' => 10,
		'#default_value' => $content_column? $content_column: 540,
		'#weight' => -9,
	);

	$form['contents_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'コンテンツ設定',
		'#collapsible' => FALSE,
		'#weight' => -9,
	);

	$form['contents_field']['contents_domain'] = array(
		'#type' => 'textfield',
		'#title' => 'ドメイン',
		'#description' => 'コンテンツのあるドメイン',
		'#size' => 50,
		'#default_value' => $contents_domain? $contents_domain: 'http://mm6.cms.k.hosei.ac.jp',
		'#weight' => -10,
	);

	$form['contents_field']['contents_root'] = array(
		'#type' => 'textfield',
		'#title' => 'ルートパス',
		'#size' => 50,
		'#default_value' => $contents_root? $contents_root: '/ocw/',
		'#weight' => -9,
	);

	$form['stream_field'] = array(
		'#type' => 'fieldset',
		'#title' => 'ストリーミング設定',
		'#collapsible' => FALSE,
		'#weight' => -8,
	);

	$form['stream_field']['stream'] = array(
		'#type' => 'textfield',
		'#title' => 'アプリケーション',
		'#description' => 'ストリームURL',
		'#size' => 50,
		'#default_value' => $stream? $stream: 'rtmp://mm6.cms.k.hosei.ac.jp/vodocw',
		'#weight' => -10,
	);

	$form['stream_field']['stream_path'] = array(
		'#type' => 'textfield',
		'#title' => 'ルートパス(flv)',
		'#size' => 50,
		'#default_value' => $stream_path? $stream_path: '/ocw/flv',
		'#weight' => -9,
	);

	$form['menu_field'] = _set_navigation_menu_form($form_state, -7);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => '設定を保存',
	);
	
	_set_default_form_id($form);

	return $form;
}

function ocw_admin_settings_submit($form, &$form_state) {
	$post_values = $form_state['clicked_button']['#post'];
	$post_id = $form_state['clicked_button']['#id'];
	
	$form_state['storage'] = array_merge($form_state['storage'], $post_values);
	_change_navigation_menu_form_order($form_state['storage']);
	
	_update_content_values(NULL, 'settings_columns', array(
			'type' => 'admin',
			'options' => serialize(array(
				'menu' => $post_values['menu_column'],
				'content' => $post_values['content_column'],
		))
		)
	);
	
	_update_content_values(NULL, 'settings_contents', array(
			'type' => 'admin',
			'url' => $post_values['contents_root'],
			'options' => $post_values['contents_domain'],
		)
	);
	
	_update_content_values(NULL, 'settings_stream', array(
			'type' => 'admin',
			'url' => $post_values['stream_path'],
			'options' => $post_values['stream'],
		)
	);
	
	_update_content_values(NULL, 'settings_menu', array(
			'type' => 'admin',
			'options' => serialize($form_state['storage']['ocw_navigation_menu'])
		)
	);
	
	drupal_set_message('設定を更新しました');
}

function _change_menu_list_order(&$storage) {
	if (count($storage['menu']) && $storage['list']) {
		$menu_list = array();
		foreach($storage['list'] as $key => $weight) {
			$menu_list[$key] = $storage['menu'][$key];
		}
		$storage['menu'] = $menu_list;
	}
}

