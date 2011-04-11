Bakery module, for single sign-on between Drupal sites on the same domain.

Regarding registration/sign-in on subsites:
===========================================
This feature is improtant for usability. It's also really easy to configure your
site so that this feature is horrible for usability. A few examples:

Registration:
This feature does not support saving any data other than the username, e-mail 
address, and fields created with the core profile module. If you have other 
modules that modify the registration process to add fields or make the form
behave differently they are unlikely to work properly from the subsite. 

You should keep the "allow registration" and "Require e-mail verification when 
a visitor creates an account" settings the same on all sites. If your
master site disallows registration then no subsites will be allowed to create
accounts either and users will be confused why they see a form but it doesn't
work.

Known issues:
===========================================
  * Values in profile fields exposed on the subsite that are not set on the
    master will not be saved.
  * Bakery is currently incompatible with a configuration that requires
    administrator approval of accounts. An account registered on the slave will
    not be set to blocked on the master.
    
// $Id: README.txt,v 1.1.4.2 2010/10/27 19:15:57 greggles Exp $
