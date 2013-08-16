<?php
/*
client: hcs
key: d0504fd1d4b9e6ec9f04
*/

$params = array('client','key','date','report');
if( !isset($_GET['date']) )
{
  $_GET['date'] = date('Y_m_d',strtotime('-1 day'));//yesterday is the latest
}

foreach($params as $param)
{
  if( !isset($_GET[$param]) || empty($_GET[$param]) )
  {
    die('Missing '.$param);
  }

}
extract($_GET);

// require_once('../drupal/sites/all/modules/tnt/ehealth.php');
define('DS',DIRECTORY_SEPARATOR);

$concord_to_ssp = array(
  'Halifax-WAN' =>  'hcs',
  'Vita-WAN'    =>  'vita',
  'Dillard-WAN' =>  'dca',
  'Caswell-WAN' =>  'ccs',
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

// back to this file
global $ssp_to_concord;

if( isset($client) )
  $client = strtolower($client);

if( tnt_make_key($client) != $key )
{
  die('Invalid Key');
}

$client_dir = $ssp_to_concord[$client];
$root_dir = 'C:'.DS.'xampp'.DS.'htdocs'.DS.'ssp'.DS.'ehealth'.DS;
$temp = $root_dir.'groups'.DS.$client_dir.DS.'health'.DS.'lanWan'.DS.$client_dir.'_'.$_GET['date'].'_*';
//die($temp);
//$report_dir = glob('groups'.DS.$client_dir.DS.'health'.DS.'lanWan'.DS.$client_dir.'_'.$_GET['date'].'_*',GLOB_ONLYDIR);
$report_dir = glob($temp,GLOB_ONLYDIR);
//print_r($report_dir);
if( empty($report_dir) ) die('This report does not exist.');
$report_dir = current($report_dir);

$concord_graph = $ssp_graph_names[$_GET['report']];
$temp = $report_dir.DS.'*'.$concord_graph.'*';
//die($temp);
$the_report = glob($report_dir.DS.'*'.$concord_graph.'*');
//print_r($the_report);
if( empty($the_report) ) die('This report does not exist.');
$the_report = current($the_report);

$report_path_info = pathinfo($the_report);

if( $report_path_info['extension'] == 'gif' )
{
  header('Content-type: image/gif');
  echo file_get_contents($the_report,FILE_BINARY);

}
else if( $report_path_info['extension'] == 'jpeg' )
{
  header('Content-type: image/jpeg');
  echo file_get_contents($the_report,FILE_BINARY);
}
else if( $report_path_info['extension'] == 'html' )
{
  if( $report_path_info['filename'] == 'page0002' )//situations to watch
  {
    $situations_to_watch = trim(file_get_contents($the_report));
    list($junk,$situations_to_watch) = explode('<A NAME="sitToWatch"></A>',$situations_to_watch,2);
    list($situations_to_watch,$junk) = explode('<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0 BORDER=0><TR>',$situations_to_watch,2);
    $situations_to_watch = strip_tags($situations_to_watch,'<TABLE><TD><TR>');
    $situations_to_watch = explode('<TABLE ',$situations_to_watch);
    $situations_to_watch = $situations_to_watch[2];
    if( !empty($situations_to_watch) ) echo '<TABLE '.$situations_to_watch;
    echo ' ';//inserts a bullet if nothing is echoed

  }
}
else
{
  echo file_get_contents($the_report);
}

?>