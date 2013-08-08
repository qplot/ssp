<?php
define('DS',DIRECTORY_SEPARATOR);

$concord_to_ssp = array(
	'Halifax-WAN'	=>	'hcs',
	'Vita-WAN'		=>	'vita',
	'Dillard-WAN'	=>	'dca',
	'Caswell-WAN'	=>	'ccs',
);

$ssp_to_concord = array_flip($concord_to_ssp);

$concord_graph_names = array(
	'avail' => 'Availability',
	'averageHealthIndex' => 'Average_Health_Index',
	'avgHealthIndex' => 'Hourly_Average_Health_Index',
	'avgNetVolume' => 'Average_Network_Volume',
	'bandwidthUtilizationLanWan' => 'Bandwidth_Utilization',
	'latency' => 'Latency',
	'netVolume' => 'Network_Volume',
	'reach' => 'Reachability',
	'volumeLeaders' => 'Volume_Leaders',
	'volumeVsBaseline' => 'Volume_vs_Baseline',
	'page0002' => 'Situations_to_Watch'
);

$ssp_graph_names = array_flip($concord_graph_names);

function tnt_make_key($client)
{
	return substr(sha1('craZyr3df0x'.sha1(strtolower($client))),0,20);
}

?>