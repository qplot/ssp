$Id: README.txt,v 1.3.2.2.2.2 2008/04/17 04:32:12 scafmac Exp $

Basic Info
==========

The ldap_integration module is actually a set of modules. 
Currently, they include
	ldapauth
	ldapgroups
	ldapdata
	

ldapauth.module is the basic module upon which every other ldapXXX module 
depends on. It implements LDAP/AD authentication for Drupal. This module has 
been tested with OpenLDAP, OpenDirectory (Mac OS X) and Active Directory 2003.

ldapgroups.module depends upon ldapauth.module and extends the functionality 
to  integrate LDAP Groups with Drupal Roles

ldapdata.module depends upon the profile and ldapauth module  and extends the 
functionality to enable mapping of other LDAP attributes to Drupal attributes.
Also allows management of ldap attributes via the Drupal profile module.


History
=======
This module was initially developed by pablom (http://drupal.org/user/6936) 
for drupal. Its current maintainers are kreaper (http://drupal.org/user/57158)
and scafmac (http://drupal.org/user/90087)


Limitations
===========

The module group went through a re-design overhaul as of Drupal 5.x when it 
was transferred to the new maintainers. As such, support for previous modules
is extremely limited. Before upgrading this module from < 4.7.x to 5.x, you
must read http://drupal.org/node/92407#comment-178886


Documentation
=============

Documentation is forthcoming. For LDAP administrators, it should be intuitive.
(well, at least that was the thought..)


