$(function(){
	$('table').attr('border','0')
	var non_zebra_tbls = $('td').parent().not('.odd,.even');
	non_zebra_tbls.filter(':even').addClass('odd')
	non_zebra_tbls.filter(':odd').addClass('even')
	$('table tr').hover(function(){ $(this).addClass('hover'); },function(){ $(this).removeClass('hover'); })
	
	$('#switch-me').click(function(){
		$('#group-switcher form').show();
		$(this).hide();
		return false;
	});
	$('#switch-cancel').click(function(){
		$('#switch-me').show();
		$('#group-switcher form').hide();
		return false;
	});
	
	$('.primary-links .last').before('<li><a id="tour-link" href="#tour">Tour</a></li>');
	
	$('#tour-link').click(function(){
		if( $('#tour-guide').length == 0 )
		{
			$.getScript(js_dir+'tour.js',function(){
				start_tour();
			});
		} else start_tour();
		
		return false;
	});
});
