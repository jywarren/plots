Custom Tokens
----------------------
Author - Ariel Barreiro (hanoii) http://drupal.org/user/23157
abarrei@gmail.com

Ported to Drupal 6 - Arnold Chang (arnoldc) http://drupal.org/user/357761

Overview:
--------
It gives the user the ability to create custom tokens using PHP code for specific
replacements that can improve other modules relying on
<a href="http://drupal.org/project/token">Token</a>.

<h3>Advanced module, PHP code evaluation and execution</h3>
This module is designed for developers with a little PHP knowledge. Custom tokens
will evaluate and execute PHP snippets in order to provide the replacements for
the tokes you define.

Dependencies:
 <a href="http://drupal.org/project/token">Token</a>

Sponsored by:
<a href="http://www.infomagnet.com">Infomagnet</a>

Installation:
------------

1. Copy the token_custom directory to the Drupal sites/<...>/modules/ directory.

2. Go to "Administer" -> "Modules" and enable the module.

3. Go to "Administer" -> "Build" -> "Tokens" and start creating the tokens you
need.

4. You need to assign proper permissions to any role you would like to be able
to create additional PHP tokens.

Last updated:
------------
$Id: README.txt,v 1.2.2.1 2008/11/21 13:59:35 hanoii Exp $