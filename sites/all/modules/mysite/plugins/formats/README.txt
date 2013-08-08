// $Id: README.txt,v 1.2 2008/04/06 23:08:26 agentken Exp $

/**
 * @file
 * Notes regarding the Format files used with MySite..
 */

MySite Formats
----------------
Format plugins are used by the MySite module to control the display of individual
elements with a user's content collection.

Format plugins are roughly analogous to node.tpl.php in PHPTemplate.

MySite ships with two Format files:

 -- default.theme
 -- teasers.theme

Of these, only 'default.theme' is required.  MySite will not work without it.


Options
---------
You may create new Format files by following the API documentation.  New format files
must be named *.theme and placed within the 'mysite/plugins/formats' directory.

If you remove all theme files except 'default.theme' your site users will not be presented
any format selection options.

If you wish to use a different default format, simply rename the appropriate file to 'default.theme' and
replace the original default file.


Contributing
--------------
If you have created a Format file and wish to share it, simply attach the file as a new issue at

http://drupal.org/project/issues/mysite

You may need to change the file extension to .txt to attach the file.
