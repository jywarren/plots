Module: Views PHP Filter
Author: George Nassar (george.nassar@gmail.com)


Description:
------------
Per a request in http://drupal.org/node/115947, Views PHP Filter effectively creates a Views Filter on criteria defined by a snippet of PHP.  This allows for much more powerful custom filtering on dynamically generated criteria.


Installation:
-------------
1. Copy the module directory into your 'modules' directory

2. Enable the module at 'administer >> modules'; it will appear under the Views category.


Usage:
------
A new filter, labeled "Node: Node ID", will appear as addablein your Views filter lists.  If you add that filter, you'll note the "Options" dropdown will have two choices: "PHP code" and "ID list".  "PHP code" will take a snippet of PHP as its value (without the <?php and ?> bracket tags), which should return an array of integers representing node IDs.  The filter will evaluate the PHP, and filter on the numbers in the return array.  "ID list" will simply take a list of numbers, separated by commas, and filter those node IDs.  This is currently used mainly for testing, but may survive to release if someone finds it useful.


NOTE:
-----

One should take caution in that the PHP code will be eval'd within the context of Drupal and Views.  This is a useful thing for power users, but can cause trouble if you're not expecting it.  For example, your code should not use variable names like $query unless you are actually intending to clobber the preexisting $query object.  Name your variables with this in mind.  A good safeguard if you aren't sure what you may or may not be clobbering is to give your all your variables some unique prefix -- say, start them all out with "$myvar_" or whatever you like.


Last updated: $Id: README.txt,v 1.3.2.1 2009/05/31 18:12:57 gnassar Exp $
