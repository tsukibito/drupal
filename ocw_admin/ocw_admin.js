if (Drupal.jsEnabled) {
	var base_path = '';
	var ocw_admin_path = '';

	var contents_domain = '';
	var contents_root = '';
	
	var form_id = '';
	$(document).ready(function() {
		base_path = Drupal.settings.ocw_admin.base_path;
		ocw_admin_path = Drupal.settings.ocw_admin.path;
		
		contents_domain = Drupal.settings.ocw_admin.contents_domain;
		contents_root = Drupal.settings.ocw_admin.contents_root;
		
		if ($('#dummy_form_default_submit').val()) {
			form_id = document.getElementById('dummy_form_default_submit').form.id;
		}
		
		var contents_lists = $('.ocw-contents-list');
		for (var i = 0; i < contents_lists.length; i++) {
			var list = contents_lists[i];
			if ($(list).html() == '') {
				id = i;
				base = contents_domain;
				dir = $(list).attr('directory');
				click_function = $(list).attr('func');
				ext = $(list).attr('ext');
				
				if (!dir) dir = contents_root;
				if (!ext) ext = '';
				click_function = click_function.split("'").join("\\'");
				$(list).load(ocw_admin_path+'/include/get_contents.php?id='+id+'&base='+base+'&dir='+dir+'&func='+click_function+'&ext='+ext+'&'+Math.random());
			}
		}
		
		var contents_lists = $('.delete-content-button');
		for (var i = 0; i < contents_lists.length; i++) {
			var list = contents_lists[i];
			$(list).click(function() {
				answer = confirm('本当にこのコンテンツを削除しても良いですか？');
				if (answer){
					$('#delete_field_id').val($(this).attr('field_id'));
					$('#delete_contents_id').val($(this).attr('contents_id'));
					$('#delete_contents_submit').click();
				}
			});

			$(list).mouseover(function() {
				$(this).css('background-color', '#ddd');
				$(this).css("cursor","pointer");
			});
			
			$(list).mouseout(function() {
				$(this).css('background-color', '#eee');
				$(this).css("cursor","default");
			});
		}
		
		if ($('#ocw_admin-form') && $('#main-block').html() == null) {
			$('#ocw_admin-form').css('margin', '10px');
		}
		
		var input_list = $('input');
		for (var i = 0; i < input_list.length; i++) {
			var inout_form = input_list[i];
			if ($(inout_form).val() == '検索' && $(inout_form).attr('type') == 'submit') {
				$(inout_form).css('background', 'url('+ocw_admin_path+'/include/icons/search.png)');
				$(inout_form).css('height', '22px');
				$(inout_form).css('width', '40px');
				$(inout_form).css('border', '0px');
				$(inout_form).css('text-indent', '-9898px');
			}
		}
	});

	function set_form_id(form_name) {
		form_id = form_name;
	}

	function edit_news_item_value(key, content, year, month, day) {
		$('#news-item-content').val(content);
		$('#edit-news-date-month').val(month);
		$('#edit-news-date-day').val(day);
		$('#edit-news-date-year').val(year);
		$('#news-add-submit').val('更新');
		$('#edit-news-item').val(key);
	}
	
	function delete_news_item_value(id) {
		answer = confirm('本当にこのニュースを削除しても良いですか？');
		if (answer){
			$('#delete-news-item').val(id);
			$('#news-delete-submit').click();
		}
	}
	
	function edit_navigation_menu_item(key,label,url) {
		$('#menu-item-label').val(label);
		$('#menu-item-url').val(url);
		$('#menu-add-submit').val('更新');
		$('#edit-menu-item').val(key);
	}
	
	function delete_navigation_menu_item(id) {
		answer = confirm('本当にこのラベルを削除しても良いですか？');
		if (answer){
			$('#delete-menu-item').val(id);
			$('#menu-delete-submit').click();
		}
	}
	
	var profile_class = '講義名';
	var profile_professor = '教員名';
	var profile_faculty = '学部・研究科';
	
	function set_profile_preview(faculty_value, class_value, prof_value) {
		profile_class = class_value;
		profile_professor = prof_value;
		profile_faculty = faculty_value;
	}

	function change_profile_preview(tag, value) {
		switch(tag) {
			case 'class': profile_class = value; break;
			case 'professor': profile_professor = value; break;
			case 'faculty': profile_faculty = value; break;
		}
		
		var profile_tag = profile_class + '<br />' + profile_faculty + '&nbsp;&nbsp;' + profile_professor;
		$("#profile-preview").html(profile_tag);
	}

	function edit_class_contents(id) {
		document.getElementsByName('pre_edit_id').item(0).value = $('#edit_class_id').val();
		$('#edit_class_id').val(id);
		$('#edit_class_content_id').val(-1);

		$('#ocw-class-content-edit').submit();
	}

	function delete_class_contents(id) {		
		answer = confirm('本当にこのラベルを削除しても良いですか？');
		if (answer){
			var class_values = $.evalJSON($('#class_values_json').val());
			delete class_values[id];
			$('#class_values_json').val($.toJSON(class_values));

			$('#ocw-class-content-edit').submit();
		}
	}

	function edit_class_content(id) {
		$('#edit_class_content_id').val(id);
		$('#ocw-class-content-edit').submit();
	}

	function delete_class_content(id) {
		answer = confirm('本当にこのコンテンツを削除しても良いですか？');
		if (answer){
			var class_values = $.evalJSON($('#class_values_json').val());
			delete class_values[$('#edit_class_id').val()].contents[id];
			$('#class_values_json').val($.toJSON(class_values));

			$('#ocw-class-content-edit').submit();
		}
	}

	function set_contents_path(form_id, path) {
		$('#'+form_id).val(path);
	}
	
	function change_class_label(class_id, content_id, obj) {
		var class_values = $.evalJSON($('#class_values_json').val());
		if (content_id == -1) {
			class_values[class_id].label = obj.value;
		}
		else {
			class_values[class_id].contents[content_id].label = obj.value;
		}
		$('#class_values_json').val($.toJSON(class_values));
	}

	function order_class(order_key) {
		pre_order_key = $('#order_key').val();
		order_state = (!parseInt($('#order_state').val()))*1;
		search_word = $('#search_word').val();
		
		if (pre_order_key != order_key) {
			order_state = 0;
			$('#order_state').val(0);
		}
		else {
			$('#order_state').val(order_state);
		}
		
		$('#order_key').val(order_key);
		
		$.get(base_path+'/OCW/list',
				{
					'order_key': order_key,
					'pre_order_key': pre_order_key,
					'order_state': order_state,
					'search_word': search_word,
					'reload': 1
				 },
				 set_class_table);
	}
	
	function search_class(reset) {
		order_key = $('#order_key').val();
		order_state = $('#order_state').val();
		search_word = reset? '': $('#class-search-form').val();
		
		$('#search_word').val(search_word);
		
		$.get(base_path+'/OCW/list',
				{
					'order_key': order_key,
					'pre_order_key': '',
					'order_state': order_state,
					'search_word': search_word,
					'reload': 1
				 },
				 set_class_table);
	}
	
	function set_class_table(data, status) {
		$html = $(data);
		$('#class-table-block').html($($html).find('#class-table-block').html());
	}
	
	function get_contents_from_http(id, base, dir, click_function, ext) {
		click_function = click_function.split("'").join("\\'");
		$($('.ocw-contents-list')[parseInt(id)]).load(ocw_admin_path+'/include/get_contents.php?id='+id+'&base='+base+'&dir='+dir+'&func='+click_function+'&ext='+ext+'&'+Math.random());
	}
	
	function set_content_field(path, id) {
		if (id.indexOf(' ') != -1) {
			ids = id.split(' ');
		}
		else {
			ids = new Array(id);
		}
		
		$.get(base_path+path, null, function(data, status) {
			var $target = $(data);
			for (var i = 0; i < ids.length; i++) {
				$('#'+ids[i]).html($("#"+ids[i], $target));
			}
		}, 'html');
	}

	Drupal.theme.tableDragChangedWarning = function(){
		return '<div></div>';
	};
}
