// $Id: node_images.js,v 1.2.2.1 2008/08/23 08:53:35 stefano73 Exp $

Drupal.behaviors.node_images_slideshow = function() {
  for (var node in Drupal.settings.node_images_slideshow) {
    var slideshow = Drupal.settings.node_images_slideshow[node], container = $('#slideshow-' + node);
    
    function preloadImages(i) {
      var n = slideshow.images[++i];
      if (slideshow.current != i) {
        if (n) {
          n.image = $('<img>').attr('src', n.src).load(function() { preloadImages(i); });
        } else preloadImages(0);
      }
    }
    
    function updateSlideshow(previous) {
	  if (slideshow.current != previous) {
	    $('#thumb-' + previous).removeClass('active');
	    $('#thumb-' + slideshow.current).addClass('active');
	  }
	  
	  var current = slideshow.images[slideshow.current];
      current.title = current.title || '';
      current.description = current.description || '';
      current.href = current.href || '#';

      container.
        find('.polaroid').attr('src', current.src).css({ opacity: 0.8 }).animate({ opacity: 1, width: current.width, height: current.height }).end().
        find('.title').html(current.title).end().find('.description').html(current.description).end().
        find('.link').attr('href', current.href).end().find('.current').html(slideshow.current).end();
      return false;
    }

    container.find('.previous').click(function() {
	  var current = slideshow.current;
	  if(!(slideshow.images[--slideshow.current])) slideshow.current = slideshow.total;
      return updateSlideshow(current);
    }).end().
    
    find('.next').click(function() {
	  var current = slideshow.current;
      if(!(slideshow.images[++slideshow.current])) slideshow.current = 1;
      return updateSlideshow(current);
    }).end();
    
    $('.slideshow-thumb').each(function () {
	  $(this).click(function() {
		var previous = slideshow.current;
		slideshow.current = this.id.substr(6, this.id.length-6);
		return updateSlideshow(previous);
      });
    });

    preloadImages(slideshow.current);	
	$('#thumb-' + slideshow.current).addClass('active');
  }
}