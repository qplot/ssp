README.txt
==========
This module allows you to create sections. Each section has an installed template, theme or style attached to it. Each section also contains a path setting similar to the new blocks admin. 
You can then assign themes to a list (regexped) paths. 

For example, if you want another style for your site admin, all you have to do, is create a section with:
name: Adminstration Section
path: admin*
and select (a customade) theme "admintheme" to that section.


Notes
=====
  This module is currently under development and I am still figuring out how to make it perform better.
  For example, the _init() hook is used, If we can find a method to incorporate this ins the menu, and thus using cahching etc, that would be very nice.
  Also, the logic inside sections_in_section($section = NULL) is suboptimal. Anyonw with good pregmatch knowledge is welcome to improve it, so that we can avoid to pull all sections out of the DB every call.
  Allthough it is built for 4.5, I will not release this module for 4.5 untill I have enough solid feedback, to call it stable.
  
Bèr Kessels [Drupal Services http://www.webschuur.com]

Feedback will be welcomed, but for support, please create an 'issue' of type 'support request' for the project -Internationalization-.
