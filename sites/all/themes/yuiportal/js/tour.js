var TOUR_PAUSE = 5000;
var TOUR_ANIM_SPEED = 2000;

function get_primary_link( url )
{
	return $("#nav a[href='"+ url +"']");
}

var $droplet_header = $('.view .mysite-header:first');
var $tour_link = $('#tour-link');

var tour_stops = [
		{ el: $tour_link, cls:'up', msg: "Hi there, I'm your Tour Guide!<br />I'm going to show you around." },
		{ el: $tour_link, cls:'up', msg: "I can give help on <b>ANY</b> page." },
		{ el: $('#heat_filter'), cls:'left', msg:"You can use this form to filter through tickets."},
		{ el: $droplet_header, cls:'left', msg: "These droplets contain information about your network." },
		{ el: $droplet_header, cls:'left', msg: "You can customize your homepage by <b>dragging and dropping</b> these headers." },
		{ el: $('.mysite-footer-right:first a:first'), cls:'up', msg: "These links will move them as well." },
		{ el: $('.network-map:first'),cls:'left',msg:"This network map will show you the status of each of your monitored network devices." },
		{ el: $('.network-map:first'),cls:'left',msg:"You can <b>click on a devices</b> to get more detail or <b>open a ticket</b> on that device." },
		{ el: $('select.reports:first'),cls:'left',msg:"Here you can view network reports on the previous day." },
		{ el: $('.mysite-mainmenu .mysite-current .mysite-table-1:first'), cls:'left',msg:'Here you can change the order items appear on your homepage.' },
		{ el: $('.mysite-mainmenu .mysite-options a:last'), cls:'up',msg:'If you mess up, you can always go back to defaults.' },
		{ el: get_primary_link('/mysite/me/content/droplet'), cls:'up', msg: "This link will allow you to add additional content to your homepage" },
		{ el: get_primary_link('/network-summary'), cls:'up', msg: "This link provides you a quick summary of your network, easy to print too." },
		{ el: get_primary_link('/heat/tickets'), cls:'up', msg: "This link will allow you to view Surazal Tickets for your network." },
		{ el: $('#switch-me'), cls:'up', msg: "You can switch between your different networks here." },
		{ el: $('#sitename'), cls:'left', msg: "<b>Lost?</b> Click our Logo to return to the homepage." },
		{ el: get_primary_link('/contact'), cls:'up', msg: "This concludes our tour, if you have any questions please contact us." }
];

var tour_current_stop = 0;
var $tour_guide;
var $tour_tip;

function init_tour()
{
	if ($('#tour-guide').length == 0)
		$('body').append('<div id="tour"><div id="tour-guide"></div><div id="tour-tip"></div></div>');
	$tour_guide = $('#tour-guide').show();
	$tour_tip = $('#tour-tip');
}

function start_tour()
{
	init_tour();
	tour_current_stop = 0;
	next_stop();
}
	
function next_stop()
{
	if( tour_current_stop >= tour_stops.length ) { $tour_guide.fadeOut('slow'); $tour_tip.fadeOut('slow'); return; }
	var stop = tour_stops[tour_current_stop];	
	var $stop = stop['el'];
	
	if( $stop.length == 0 )//cant find current stop, go to next
	{
		if( ++tour_current_stop < tour_stops.length ) next_stop();
		return
	}
	
	$tour_tip.fadeOut();
	var offset = $stop.offset();
	var top  = offset.top + $stop.height();
	var left = offset.left + ($stop.width()/2);
	var speed = TOUR_ANIM_SPEED;
	if( tour_current_stop > 0 && $stop == tour_stops[tour_current_stop-1]['el'])
	{
		speed = 0;//instant
	}
	$tour_guide.animate({
		top:top +'px',
		left:left + 'px'
	},speed,'swing',function(){
		$('#tour').attr('class','tour-state-' + stop['cls']);
		$tour_tip.html(stop['msg'])
		$tour_tip.css({
			top:	top  + $tour_guide.height() + 'px',
			left:	left + 'px',
			right:  'auto'
		});
		if( left + $tour_tip.width() + 26 > document.body.offsetWidth )
		{
			$tour_tip.css({
				right:'1em',
				left:'auto'
			});
		}
		$tour_tip.fadeIn();
		if( ++tour_current_stop < tour_stops.length ) setTimeout('next_stop()',TOUR_PAUSE);
		else setTimeout('hide_tour()',TOUR_PAUSE)
	});
}

function hide_tour()
{
	$tour_tip.hide('slow');
	$tour_guide.hide('slow');
}
