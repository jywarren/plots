 /\  __|_.  ._|_
/~~\(_ | |\/| |\/
               /
This module allows you to record various activities on your website and display
them in a 'feed', such as other popular websites do.

Example activity messages:
  * You wrote a comment in response to "example comment title"
  * Jim wants to be your friend. Approve this friendship here.
  * Nancy is Phil's latest fan.

Technically:
  * Activity 6.x-2.x takes a new approach from previous versions.
  * Activity now provides a configurable action that can be used with any
    Triggers.
  * All activity is displayed through Views 2.
  * Core modules such as Node, Comment, and User are provided as includes as a
    way to provide example of how this can be integrated with any module.
  * Permissions can be provided for display by any sort of user relationship
    type modules, such as Flag friend, User Relationships, Friendlist, etc.