// $Id: dashboard.js,v 1.13 2010/05/22 15:02:20 techsoldaten Exp $

Drupal.behaviors.dashboard = function(context) {
  $dashboard = $('#dashboard:not(.dashboard-processed)', context).addClass('dashboard-processed');

  if ($dashboard.length > 0) {
    var dragTimeStamp;
    // Widget reordering.
    $columns = $dashboard.find('>div.column');
    $columns.sortable({
      items: '>div.widget',
      handle: '>h2',
      connectWith: $columns,
      placeholder: 'dashboard-placeholder ' + $('>div.widget', $columns).attr('class'),
      forcePlaceholderSize: true,
      //containment: 'window',
      distance: 5,
      opacity: 0.8,
      stop: function(event, ui) {
        widgets = [];
        $columns.each(function() {
          widgets.push($(this).sortable('toArray').join(',').replace(/widget-/g, ''));
        });
        jQuery.post(Drupal.settings.basePath + 'dashboard/' + Drupal.settings.dashboardPage + '/reorder-widgets', {
          token: Drupal.settings.dashboardToken,
          column_0: widgets[0],
          column_1: widgets[1],
          column_2: widgets[2]
        });
        dragTimeStamp = event.timeStamp;
      }
    }).find('>div.widget>h2>a').click(function(event) {
      // Do not process header links after dragging.
      if ((event.timeStamp - dragTimeStamp) <= 20) {
        return false;
      }
    });
  }
}

Drupal.behaviors.dashboardNavigation = function(context) {
  $navigation = $('.nav-content-dashboard:not(.dashboard-processed)', context).addClass('dashboard-processed');

  if ($navigation.length > 0) {
    // Add IDs for sortable('toArray'). theme('links') can not put IDs here.
    $navigation.find('>ul>li').each(function() {
      $this = $(this);
      $this.attr('id', $this.attr('class').replace(/ .*/, ''));
    });

    // Fake hovering for edit & delete icons.
    $dashboardActiveSpan = $navigation.find('>ul>li.active>span').hover(
      function () {
        $dashboardActiveSpan.addClass('hover');
      },
      function () {
        $dashboardActiveSpan.removeClass('hover');
      }
    );

    // Drag & drop tabs
    $navigation.sortable({
      items: '>ul>li:not(.dashboard-link-add, .dashboard-profile)',
      containment: $navigation,
      axis: 'x',
      distance: 5,
      start: function(event, ui) {
        ui.helper.find('>span.hover').removeClass('hover');
      },
      stop: function(event, ui) {
        jQuery.post(Drupal.settings.basePath + 'dashboard/' + Drupal.settings.dashboardPage + '/reorder-pages', {
          token: Drupal.settings.dashboardToken,
          pages: $navigation.sortable('toArray').join(',').replace(/dashboard-page-/g, '')
        });
        $navigation.find('>ul>li:not(.dashboard-link-add, .dashboard-profile)').each(function() {
          $this = $(this);
          path = $this.attr('class').replace(/.* dashboard-path-([^ ]*).*/, '$1');
          $this.find('>a').attr('href', Drupal.settings.basePath + 'dashboard/' + path);
        });
        $navigation.find('>ul>li:first>a').attr('href', Drupal.settings.basePath + 'dashboard');
      }
    });
    $navigation.find('>ul>li.dashboard-link-add>a').click(function() {
      // Hide add link, show add form.
      $this = $(this).hide().parent().append(Drupal.settings.dashboardPageAddForm);
      $dashboardAddForm = $('#dashboard-page-add-form');
      $dashboardAddForm.find('#edit-title').keyup(function(event) {
        if (event.which == 27) { // Esc pressed.
          dashboardRemoveAddPageForm();
        }
        else {
          // Enable Add button only when text is present.
          if ($(this).attr('value') == '') {
            $dashboardAddForm.find('#edit-submit').attr('disabled', 'disabled');
          }
          else {
            $dashboardAddForm.find('#edit-submit').removeAttr('disabled');
          }
        }
      }).focus();
      Drupal.settings.dashboardBodyClickParent = '#dashboard-page-add-form';
      Drupal.settings.dashboardBodyClickCallback = dashboardRemoveAddPageForm;
      $('body').bind('click', dashboardBodyClick);
      return false;
    });

    // Edit page link
    $dashboardActiveSpan.find('>a.edit').click(function() {
      // Hide edit link, show edit form.
      $dashboardActiveSpan.find('>a').hide().end().append(Drupal.settings.dashboardPageEditForm);
      $dashboardEditForm = $('#dashboard-page-edit-form').find('div.delete').hide().end();

      // Correct title in case it has already been edited.
      $dashboardEditForm.find('#edit-edit-title').attr('value', $dashboardActiveSpan.find('>a.edit').text());

      // Allow AHAH form submission, but make changes active instantly.
      Drupal.attachBehaviors($dashboardEditForm);
      $dashboardEditFormSubmit = $dashboardEditForm.find('#edit-edit-submit').click(function() {
        $dashboardActiveSpan.find('>a.edit').html(Drupal.checkPlain($dashboardEditForm.find('#edit-edit-title').attr('value')) + '<span class="edit-icon"></span>');
        dashboardRemoveEditPageForm();
      });
      $dashboardEditForm.find('#edit-edit-title').keyup(function(event) {
        if (event.which == 27) { // Esc pressed.
          dashboardRemoveEditPageForm();
        }
        else {
          // Enable Edit button only when text is present.
          if ($(this).attr('value') == '') {
            $dashboardEditFormSubmit.attr('disabled', 'disabled');
          }
          else {
            $dashboardEditFormSubmit.removeAttr('disabled');
          }
        }
      }).focus();
      Drupal.settings.dashboardBodyClickParent = '#dashboard-page-edit-form';
      Drupal.settings.dashboardBodyClickCallback = dashboardRemoveEditPageForm;
      $('body').bind('click', dashboardBodyClick);
      return false;
    });

    // Delete page link
    $dashboardActiveSpan.find('>a.delete').click(function() {
      // Hide edit link, show edit form.
      $dashboardActiveSpan.find('>a').hide().end().append(Drupal.settings.dashboardPageEditForm);
      $dashboardEditForm = $('#dashboard-page-edit-form').find('div.edit').hide().end();

      // Correct title in case it has already been edited.
      $dashboardEditForm.find('#edit-delete').attr('value', Drupal.t('Yes, delete !title', {'!title': $dashboardActiveSpan.find('>a.edit').text()}));
      $dashboardEditForm.find('#edit-delete').click(function() {
        // Set a non-dynamic title so Form API is not confused.
        $(this).attr('value', Drupal.t('Deleting…'));
      });

      Drupal.settings.dashboardBodyClickParent = '#dashboard-page-edit-form';
      Drupal.settings.dashboardBodyClickCallback = dashboardRemoveEditPageForm;
      $('body').bind('click', dashboardBodyClick);
      $dashboardEditForm.find('a.cancel').click(dashboardRemoveEditPageForm);
      return false;
    });
  }
}

/**
 * Widget behaviors - delete & configure.
 */
Drupal.behaviors.dashboardWidget = function(context) {
  $("#dashboard>div.column>div.widget:not(.dashboard-processed)", context).addClass('dashboard-processed').each(function() {
    var $this = $(this);
    $('a.remove-widget', $this).data('widget', $this).click(function () {
      var $widget = $(this).data('widget').slideUp('fast');
      jQuery.post(Drupal.settings.basePath + 'dashboard/' + Drupal.settings.dashboardPage + '/remove-widget', {
        token: Drupal.settings.dashboardToken,
        widget_id: $widget.attr('id').replace(/^widget-/, '')
      });
      return false;
    });
  });
}

/**
 * Widget behaviors - add widget to dashboard.
 */
Drupal.behaviors.dashboardWidgetAdd = function(context) {
  $('.dashboard-widget a.add-widget').click(function() {

    // var type is the key from hook_dashboard()
	// Default type is 'user', defined in dashboard_dashboard()
	// TODO: get this value properly
    var type = 'user';
    var widget_id_value = $(this).attr('id').replace(/^add-widget-/, '');
    var link_element = this;

    // Save the data into database
    $.post(Drupal.settings.basePath + 'dashboard/' + type + '/add-widget',
      {
        token: Drupal.settings.dashboardToken,
        widget_id: widget_id_value
      },
      function(data, textStatus, XMLHttpRequest) {
        $(link_element).replaceWith(data.label);
      },
      "json"
    );

    return false;
  });
}

/**
 * Hide the Add page form if click is outside the form.
 */
function dashboardBodyClick(event) {
  if ($(event.target).parents(Drupal.settings.dashboardBodyClickParent).length == 0) {
    Drupal.settings.dashboardBodyClickCallback();
  }
}

/**
 * Remove the Add page form, show the link, remove event.
 */
function dashboardRemoveAddPageForm() {
  $dashboardAddForm.remove();
  $navigation.find('>ul>li.dashboard-link-add>a').show();
  $('body').unbind('click', dashboardBodyClick);
}

/**
 * Remove the Edit page form, show the link, remove event.
 */
function dashboardRemoveEditPageForm() {
  $dashboardEditForm.remove();
  $dashboardActiveSpan.removeClass('hover').find('>a').show();
  $('body').unbind('click', dashboardBodyClick);
}

function dashboardRemoveAddWidget() {
  $dashboardAdd.removeClass('selected').parent().find('>ul').remove();
}

/**
 * Function : dump()
 * Arguments: The data - array,hash(associative array),object
 *    The level - OPTIONAL
 * Returns  : The textual representation of the array.
 * This function was inspired by the print_r function of PHP.
 * This will accept some data as the argument and return a
 * text that will be a more readable version of the
 * array/hash/object that is given.
 * Docs: http://www.openjs.com/scripts/others/dump_function_php_print_r.php
 */
function dump(arr,level) {
	var dumped_text = "";
	if (!level) level = 0;

	//The padding given at the beginning of the line.
	var level_padding = "";
	for(var j=0;j<level+1;j++) level_padding += "    ";

	if (typeof(arr) == 'object') { //Array/Hashes/Objects
		for(var item in arr) {
			var value = arr[item];

			if (typeof(value) == 'object') { //If it is an array,
				dumped_text += level_padding + "'" + item + "' ...\n";
				dumped_text += dump(value,level+1);
			} else {
				dumped_text += level_padding + "'" + item + "' => \"" + value + "\"\n";
			}
		}
	} else { //Stings/Chars/Numbers etc.
		dumped_text = "===>"+arr+"<===("+typeof(arr)+")";
	}
	return dumped_text;
}
