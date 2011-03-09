This module adds subscription features for Google Groups to Drupal.

The module works by sending an email to your Google Group's subscription address.
The user will still have to verify their address by clicking the link in Google's
confirmation email.

A fully themable block is provided with a subscription form that is easily
themed and modified by overriding a simple template file. This block is best
used for anonymous users.

The abiltiy to put a checkbox on the registration form is also included with a
setting for the default value. Also has the ability to hide the registration
checkbox to force user subscription.


Configuration
1. Enable the module.
2. Give appropriate roles the 'administer google groups' permission.
   (admin/user/permissions)
3. Add groups via the configuration page.
   (admin/settings/googlegroups)
4. Optionally place subscription blocks via the blocks admin.
   (admin/build/block)