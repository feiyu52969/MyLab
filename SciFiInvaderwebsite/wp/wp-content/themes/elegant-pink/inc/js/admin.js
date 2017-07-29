jQuery(document).ready(function($){
	
	$('#accordion-panel-wp_default_panel').prepend(
		'<div class="user_sticky_note">'+
		'<h3 class="sticky_title">Need help?</h3>'+
		'<span class="sticky_info_row"><label class="row-element">View demo: </label> <a href="http://raratheme.com/previews/?theme=elegant-pink" target="_blank">here</a>'+
		'<span class="sticky_info_row"><label class="row-element">View documentation: </label><a href="http://raratheme.com/documentation/elegant-pink/" target="_blank">here</a></span>'+
		'<span class="sticky_info_row"><label class="row-element">Support ticket: </label><a href="https://raratheme.com/support-ticket/" target="_blnak">here</a></span>'+		
		'<span class="sticky_info_row"><label class="more-detail row-element">More Details: </label><a href="http://raratheme.com/wordpress-themes/" target="_blank">here</a></span>'+
		'</div>'
		);

	var upgrade_notice = '<div class="notice-up"><a class="upgrade-pro" target="_blank" href="http://raratheme.com/wordpress-themes/elegant-pink-pro/"><img src="' + elegant_pink_data.promo_url + '" alt="UPGRADE TO ELEGANT PINK PRO" /></a>';
	upgrade_notice += '<a class="upgrade-pro-demo" target="_blank" href="http://raratheme.com/wordpress-themes/elegant-pink-pro/">ELEGANT PINK PRO FEATURES</a></div>';
	jQuery('#customize-info').append(upgrade_notice);

});