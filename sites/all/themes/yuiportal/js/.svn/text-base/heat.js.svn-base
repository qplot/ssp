$(function(){
	
	function dateNormalized(date)
	{
		var d  = date.getDate();
		var day = (d < 10) ? '0' + d : d;
		var m = date.getMonth() + 1;
		var month = (m < 10) ? '0' + m : m;
		var yy = date.getYear();
		var year = ((yy < 1000) ? yy + 1900 : yy)+'';
		return month + "/" + day + "/" + year.substring(2,4);
	}
	
	function update_date_fields()
	{
		$('.date').each(function(){
			var $t = $(this);
			var date = Date.parse($t.val())
			if( date )
			{
				$t.val(dateNormalized(date));
			}
		});	
	}
	
	$('.date').change(update_date_fields);
	$('#heat_filter').submit(function(){
		var before = Date.parse($('input[@name=before]').val());
		var after = Date.parse($('input[@name=after]').val());
		if( before < after )
		{
			alert('Please check your dates, ' + dateNormalized(before) + ' comes before ' + dateNormalized(after) +'.' )
			return false;
		}
		return true;
	});
	
	
});