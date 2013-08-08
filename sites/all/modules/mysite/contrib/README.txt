// $Id: README.txt,v 1.3 2008/04/06 23:08:26 agentken Exp $

/**
 * @file
 * Notes regarding the use of contributed Plugins for MySite..
 */

Contributions
--------------
The items in this folder were developed by MySite users or are used with contributed Drupal modules.

All of these files are optional.  To install them, place the file in the appropriate plugin directory, as noted below.

These files have been tested for correctness against the MySite API and should work as expected.

MySite Plugins
----------------
The plugins directory stores files used by the MySite module.  This directory and its
five subdirectories are required.  You should find the following subdirectories:

 -- formats
 -- icons
 -- layouts
 -- styles
 -- types

If any of these directories are not within your 'mysite/plugins' directory, then the MySite
module will not work correctly.


Adding Plugins
----------------
Plugins can be identified by the file extension.  The following extensions go in the corresponding folders.

  -- *.inc files are Types
  -- *.theme files are Formats
  -- *.php files are Layouts
  -- *.css files are Styles
  -- *.png files are normally Layouts, but may be Icons


Current Contributions
-----------------------
The following 4 contributions are included with the MySite download:

 -- bilbio.inc
 -- refine.inc
 -- storylink.inc
 -- weblink.inc


Contributing
--------------
If you have created a new MySite plugin and wish to share it, simply attach the file as a new issue at

http://drupal.org/project/issues/mysite

You may need to change the file extension to .txt to attach the file.
