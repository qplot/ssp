<?
global $user;
$group = current($user->og_groups);
if( is_numeric($group['nid']) )
{
$group = $group['nid'];
$group = node_load($group);
$wug_user = str_replace(' ','',strtolower($group->field_short_name[0]['value']));
$wug_pass = substr(sha1('craZyr3df0x'.sha1($wug_user)),0,20);
$wug_root_device_id = $group->field_wug_root_device_id[0]['value'];
?>
<a target="_blank" class="network-map" href="http://10.20.1.59:8080/NmConsole/Navigation.asp?bIsJavaScriptDisabled=true&amp;sUsername=<?= $wug_user ?>&amp;sPassword=<?= $wug_pass ?>&amp;nDeviceGroupID=<?= $wug_root_device_id ?>&amp;sGroupView=Map.asp&amp;sTab=Explorer&amp;coords=">
	<img border="0" ismap="ismap" src="http://10.20.1.59:8080/NmConsole/utility/RenderMap.asp?bIsJavaScriptDisabled=true&amp;sUsername=<?= $wug_user ?>&amp;sPassword=<?= $wug_pass ?>&amp;nDeviceGroupID=<?= $wug_root_device_id ?>" />
</a>
<?php } ?>