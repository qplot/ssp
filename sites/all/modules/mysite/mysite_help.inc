<?php
// $Id: mysite_help.inc,v 1.4 2008/04/06 23:08:25 agentken Exp $

/**
 * @file
 * Help text for MySite module.
 *
 * @ingroup mysite
 */
 
/**
 * The text output string for the help page
 */
function mysite_helptext() {
      $output = t("<ol><li><b>Installation</b>
      <p>MySite uses the standard Drupal 5 install system. Simply put the module directory into your modules directory and then navigate to http://example.com/admin/build/modules and activate the MySite module.  Installation should create three new database tables: {mysite}, {mysite_content} and {mysite_data}.</p></li><li><b>Access Control</b>
      <p>After installation, you should check the Access Control settings at Admin >> Users >> Access control.  MySite has the following access control permissions by default:  <ul><li>'administer mysite'<br />Allows a user to control the MySite settings at:  http://example.com/administer/settings/mysite.  Users with this permission also have the ability to edit other user's  MySite content and settings.  This is allow you to provide customer  support for users having trouble with MySite.</li><li>'edit mysite'<br />Grants a user the ability to create and update a MySite page.</li><li>'view all mysites'<br />Grants a user the ability to see public MySite pages created by other  users.</li></ul></p>
      <p>If the Aggregator module is present and enabled, then an additional permission exists:  <ul><li>'add mysite feeds'<br />Grants MySite users the ability to add RSS feeds to the site based on the  settings established under the MySite settings page. </li></ul></p></li><li><b>Initial Configuration</b>
      <p><em>By design, the MySite module is very flexible.  Administrators may turn off unwanted features.  There are numerous settings options that will affect the behavior of the module.  Please read this section carefully and refer to the README.txt for additional details.</em></p>
      <p>The MySite module is dependent on other modules, since all MySite does is present content in a new format. To use MySite correctly, at least one of the following modules needs to be active:</p><ul><li>Aggregator
      <p>If you wish to allow users to add new RSS feeds, set the appropriate Aggregator settings for MySite at http://example.com/admin/settings/mysite (the Feeds tab).</p>
      <p>Note that there are two types of feed handling in MySite.  'Web Feeds' refer to  individual RSS feeds.  'Web Headlines' refer to Aggregator categories containing multiple RSS feeds.</p></li><li>Blog
      <p>If the Blog module is active, MySite will allow users to add any active blogger to their MySite page.</p></li><li>Book
      <p>If there are any Books present on your site, they can be added to a MySite page.  MySite will show the newest additions to any book selected by a user.</p></li><li>Forum
      <p>If the Forum module is active, MySite will allow users to add Forum topics to their MySite page. For this feature to work, at least one forum container and one forum topic must be present.</p></li><li>Path
      <p>If the Path module is active and you turn on Path Aliasing, each MySite will be aliased to the path 'mysite/USERNAME' when the MySite is created or updated.</p></li><li>Taxonomy
      <p>If the taxonomy module is active, MySite will allow users to add tagged content to their MySite page. For this feature to work, you must have at least one Vocabulary and one Term created at http://example.com/admin/content/taxonomy. Administrators can limit the categories available to users by configuring the module at http://example.com/admin/settings/mysite/term. Note that Forum categories are not included in MySite's taxonomy-based lists.</p></li><li>Theme
      <p>MySite supports user-based theme settings.  When enabled, MySite users may choose from any active site theme.  Anyone viewing a MySite page will view the page in the user-selected theme.</p></li></li></ul>
      <p><b>Special Cases</b> <ul><li>Droplets, Blocks and Views
      <p>'Droplets' are the Drupal equivalent of Google Gadgets: they enable the site  administrators to create small widgets that add new features to the site.  Droplets can be created from HTML, JavaScript, or PHP code.  They can also be  generated from Blocks or from Views.</p>
      <p>MySite does not expose Blocks or Views directly to end users; the administrator  must select which elements to expose via the Droplet creation system.</p></li> <li>Node and CCK (Content)
      <p>The node module is a required core module.  MySite can handle requests for all  posts of a specific node type.  If you use the CCK (Content) module, CCK node  types are also supported.</p></li>
      <li>Posts (Content) <p>The node module is a required core module.  MySite can handle requests for individual posts.  If you use the CCK (Content) module, CCK node  types are also supported.</p></li
      <li>User <p>The user module is a required core module.  MySite can track posts based on  individual users.</p></li></ul></p></li><li><b>MySite Settings</b>
      <p>The following module settings are available.</p><ol type=A><li>Display Settings
      <p>Options that affect user interaction with MySite.</p><ul><li>MySite Sitename<p>This option allows you to change the default 'My Sitename' string used to display menu links and page titles for the MySite module.</p>
      <p>Note: If you do not change this value, it will update if you change the name of your site in your System Information settings.</p></li>
      <li>MySite Behavior
      <p>This option tells MySite how to behave when a visitor goes to http://example.com/mysite. Most installation will use the default setting 'Go to User MySite,' which directs the user to http://example.com/mysite/UID/view. The additional option mirrors the menu callback for http://example.com/mysite/all and is mainly used for debugging.</p></li><li>MySite Privacy
      <p>This option sets the default visibility of the owner's MySIte page with regard  to other users.  By default, all users with the 'view all mysites' privilege can see all other user personal pages.  Individual users can turn this setting to 'private.'</p></li><li>Page Setup
      <p>The administrator now has the option to turn off the left and right rail regions for MySite pages.  This allows the user to have complete control over the content  presented by MySite.</p>
      <p>If this feature is turned on, the 'footer' region of your site will still be  printed on all MySite pages.</p>
      <p>Note: The full screen display may have unexpected consequences for your theme, since it eliminates the default sidebars and most page regions.  Test before you deploy.</p></li>
      <li>Item Deletion<p>You may configure MySite to require users to confirm the deletion of content from their MySite collection.  Users have the option to disable this feature for their own collection.</p></li>
      <li>Node Links<p>By enabling this feature, MySite will place a link at the bottom of all content.  The link, when moused over, will display a box of options related to the content.  Typical options include adding the item to your collection or deleting an item from your collection.  <em>This feature has not been tested on all browsers and platforms.</em></p></li>
      </ul></li><li>Content Settings
      <p>The Content Settings option highlights the Content Types panel, which lets you configure which content options will be available to your users.  These settings control the options that MySite users will be presented.</p><ul><li>Content Types
      <p>On the MySite settings page (http://example.com/admin/settings/mysite), you have the option of enabling all content types that are available. However, you are not required to offer all content to users. Use these settings to determine which content types can be added to a MySite page. Note: when a new Type plug-in is added to your module, a new option should appear in this list.</p>
      <p>Note: Some type files have configuration settings -- these are visible as menu tabs.  Be sure to configure each active content type as needed</p></li><li>Disabled Content Types
      <p>You may also see a section labeled <b>Disabled Content Types</b> on the settings page.</p>
      <p>The MySite module checks to see if various options and permissions have been  defined before allowing you to activate a content type.  The module will  present you with a link to correct the configuration error.  After you make  the suggested change, the content type should be available for activation.</p></li></ul></li><li>Content Browser
      <p>The Content Browser settings affect how users will be presented the content options for adding elements to their MySite page.</p><ul><li>Content Browsing
      <p>This option will only be available if you have activated the MySite Icons  module. Versions prior to 5.x.2 presented users with a simple table of choices.  In newer versions, MySite presents options for displaying content choices to users.</p>
      <p>The default setting is to show a plain-text table of items, with 'Add' links. The optional behavior is to show a table of icons, with each icon representing a content option.</p></li><li>Content Layout
      <p><em>This options will only be available if you have activated the MySite Icons  module.</em> If you select the 'Display row of icons' option for content browsing, this  option is used to set the number of items to show per row.  You should test  this settings to optimize the presentation for your site theme(s).</p></li><li>Items per Page
      <p>This option sets the pagination limit for the content browser.  In either  table or panel mode, this setting will control the number of items to show per  page.</p></li> </ul></li><li>User Limits
      <p>These options determine how much content a user can add to their MySite page.</p><ul><li>Item Count
      <p>Since MySite collects content from a variety of Drupal content, it can be resource intensive. This setting puts a cap on the number of content groups a user can add to a MySite page. The default is 10.</p></li><li>Element Count
      <p>MySite is meant to give a quick overview, not to substitute for reading the original content in context. This setting limits the number of items (links) presented under each content group. The default is 5.</p></li></ul></li><li>Action Buttons
      <p>MySite ships with a small set of GPL icons, created by agentrickard expressly for MySite, which can be used to present the action buttons to users.  Select the option that you prefer.  Default is 'Icons.'  The icons are previewed on the settings page.  </p></li><li>Cache Settings
      <p>For high-traffic sites,  MySite might cause a high volume of database requests (especially if the Item and Element Count settings are very high).  If you have performance issues, you can cache the results of a MySite page view for  a set amount of time.</p></li><li>Default Page Settings
      <p>By setting s default user UID, the module can establish a default page for users to view, set at http://example.com/mysite/default.  This page may be used as an optional homepage.  Note: This menu item is a callback, so no link to it will be generated in your menu.  You must create a link manually.</p></li><li>Default Settings
      <p>As of MySite 5.2, the administrator can now setup the user default settings for MySite.  Clicking on the 'Default Settings' tab will take you to the MySite configuration page for user 0 (zero). The settings and content that are established for user zero will be used for all anonymous users who visit http://example.com/mysite and for all users who have not yet created a MySite page. Further, when a user creates a MySite page, the user's settings will initially match the Default Settings.</p></li><li>Default Content
      <p>As of MySite 5.2, the administrator can now select a default set of content to show to all anonymous users and to authenticated users who have not yet created a MySite page. When a user creates a new MySite page, the default content set will automatically be saved in their collection.  Once saved, the user can change his or her individual content settings to remove the default content. To define the default content set, click on the 'Default Content' tab of MySite's administration page.</p></li></ol>
      <p><b>Content Specific Settings</b></p><ol type=A> <li>Droplet Settings
      <p>Droplets are content elements similar to Drupal blocks, Google Gadgets and  Yahoo! Widgets.  The Droplet system allows the site administrator to define  custom content objects that users may place on a MySite page. </p>
      <p>The Droplet system enables the creation of Droplets from code that you supply,  from existing Drupal Blocks, or from existing Views (if the Views module is  installed and active). </p>
      <p>No Droplets are installed by default.  The following Droplet creation options  exist:<ul><li>Custom Droplets
      <p>Custom droplets allow you to place whatever code you wish into a Droplet item.  In this sense, Droplets are very similar to Blocks.  We do not use the Drupal  Block system because it doesn't make sense to require administrators to create  new Blocks every time they wish to add a new Droplet. </p>
      <p>Custom Droplets can contain any code that you desire, but are subject to  Drupal's normal input filter system.  If, for example, you wish to add the  JavaScript to use a Google Gadget, you must input that Droplet as 'Full HTML.' </p></li><li>Block Droplets
      <p>MySite will show a selection form that will allow you to convert any existing  Block into a MySite Droplet.  Simply select the Block to convert and submit  the form.  You will then have the option to preview and configure the Droplet. </p>
      <p>Note: Not all Blocks make good Droplets.  Also note that Block access rules  do not apply to Droplets.  Any user can access any Droplet. </p></li><li>Views Droplets
      <p>If you have installed and activated the Views module, you can create Droplets  based on existing Views.  (You cannot, however, create Droplets based on  default Views until you have activated them locally.) </p>
      <p>To create a Droplet based on a View, simply select the View from the select  list and submit the form.  You will then have the option to preview and  configure the Droplet. </p>
      <p>Note: Not all Views make good Droplets.  In particular, Views that require  arguments cannot be handled by the current Droplet system.  Also note that  Views access rules do not apply to Droplets.  Any user can access any Droplet. </p></li></ul></li><li>Aggregator Settings
      <p>If the Aggregator module is enabled and you have given users the permission to 'add mysite feeds,' you will need to configure the default settings for handling new Aggregator feeds. These settings help prevent users from overloading the Aggregator be setting throttles on update frequency.</p></li><ul><li>Aggregator Feeds
      <p>Only if you 'Allow users to add new feeds' will the access control settings appear. Then you can give specific roles the ability to add new feeds. Recommended use is to allow all registered users to add new feeds.</p></li><li>Allowed Feed Categories
      <p>When a user adds a new feed through MySite, they will only be given the  category options that you allow.  If only one option is selected, there is no  choice allowed and all user feeds will be filed under the default category. </p>
      <p>In MySite 5.x.2 Feed Categories are no longer required.</p></li><li>Default Feed Category
      <p>Establishes the default feed category for user-submitted feeds.</p></li><li>Update Interval
      <p>Sets the update interval for all user-submitted feeds.</p></li></ul></li><li>Node Settings
      <p>The Node module is a core Drupal module.  This MySite feature also works with  CCK (Content) module to allow users to select content based on content type. </p>
      <p>On the Nodes settings page, you should see a list of checkboxes for all  content types used by your site.  Simply select which elements you want users to be able to access through MySite. If you do not make any selections, then all node types will be available.</p>
      <p>The MySite module will display the X most recent posts by type, where X is set by the default Element Count setting. </p>
      <p>Note: MySite does not use PHPTemplate for node formatting.  Custom node theming do at the template layer will not appear to MySite users.  See the API  documentation for information about changing node templates in MySite. </p></li>
      <li>Post Settings <p>The Node module is a core Drupal module.  This MySite feature also works with  CCK (Content) module to allow users to select content based on content type. </p>
      <p>On the Posts settings page, you should see a list of checkboxes for all  content types used by your site.  Simply select which elements you want users to be able to access through MySite. If you do not make any selections, then all node types will be available.</p>
      <p>The MySite module will display the teaser element of the selected post. </p>
      <p>Note: MySite does not use PHPTemplate for node formatting.  Custom node theming do at the template layer will not appear to MySite users.  See the API  documentation for information about changing node templates in MySite. </p></li>
      <li>Profile Settings <p>The Profiles handler is new in MySite 5.x.2 and is a little tricky.  This  plugin works with the User module -- through hook_user() -- and the Profile  module to define the available profile elements that MySite users may display  on their personal page. </p>
      <p>The problem is that Profile elements may be different for users of different  roles or permissions, or for users who have done certain actions (like  blogging). </p>
      <p>The settings on this page establish the default elements for each user's  MySite profile item.  Users may change these settings after they activate  their profile item. </p>
      <p>Note: MySite users can only add their profile to their personal page.  By  design, only publicly-viewable profile fields are allowed.  The link to a  user's MySite page provided by mysite_user() will not be shown, since it  is redundant. </p></li><li>Taxonomy Settings
      <p>If the Taxonomy module is enabled, you mayt configure which taxonomy categories (Drupal vocabularies) that users may add to MySite pages.  At least one vocabulary must be selected, and that vocabulary must have active terms.</p><p>If you do not select any vocabularies, all vocabularies will be available to MySite users.</p></li><li>Theme Settings
      <p>On Mysite's Theme settings page, you will find a list of all active themes for your web site.  Check the themes that users are allowed to apply to their MySite page and save.  When users edit their MySite page, the available theme list will be presented. </p></li><li>User Settings
      <p>The Users plugin is new in MySite 5.x.2 and allows MySite users to track posts by individual site users. </p>
      <p>The settings for this plugin are based on the user Roles active for your site. Only selected roles can be tracked in MySite.  This feature let's the site  administrator have some selectivity over which users can be tracked. </p>
      <p>By default, all 'authenticated users' are available to all MySite users.  If  'authenticated users' are allowed, then _all_ roles will be allowed by MySite  except for 'anonymous user'. </p>
      <p>Note: The 'anonymous user' role is included in the list in cases where  anonymous users are allowed to post site content.  </p></li></ol><li><b>Plugins and User Options</b>
      <p>Part of the design of MySite is to reduce code overhead through the use of  includes (called Plugins).  For MySite to function, certain plugins are  required, but others are optional and can be removed. </p>
      <p>In the case of Format, Layout, and Style plugins, the 'default' file is always required.  If this file is the only plugin present, users will not be allowed to change their content settings.
      <p>For Type plugins, only the plugins you wish to use need to be present.  Since  the administrator can deactivate Content Types, there is no reason to remove  these files.  </p>
      <p>The MySite module directory includes a /plugins folder.  This folder and its subfolders must be present on your server for the module to function correctly.</p>
      <p>Within each subfolder are files that control the various presentation options available to end users.</p>
      <p>The following files are required: <ul><li>plugins/formats/default.theme</li> <li>plugins/layouts/default.php</li> <li>plugins/styles/default.css</li></ul></p>
      <p>If you do not like the additional presentation options provided (such as the sunset.css file), you may simply remove them from the directory (or change the file extension to .bak).</p>
      <p>If only one file is present, users will not be presented with choices during MySite customization.</p>
      <p><b>Note:</b> The folder  -- plugins/types/ -- must contain at least one type.inc file.  Without type includes, MySite cannot generate any content.</p>
      <p>See the README.txt in each plugins directory for more details.</li></ol>");
  return $output;
}
