$(function() {

  var contentArea = '#content-area';
  // Look at all unordered lists and label them as allImages if appropriate
  $(contentArea + ' ul').each(function(index) {
    var list = this;
    $(this).children('li').each(function(index) {
      // @todo Someone could make this test more bullet proof.
      if($(this).html().substr(0, 4) != "<img") {
        list.isNotCarousel = true;
      }
    });
    if(list.isNotCarousel == null) {
      $(this).wrapAll("<div class='jCarouselLite-" + index + "' />");
      $(".jCarouselLite-" + index).wrapAll("<div class='jCarouselLite-container' />");
      $(this).before("<button class='prev-" + index + "'><<</button>");
      $(this).after("<button class='next-" + index + "'>>></button>");
      // Transform those lists into jCarousels
      $(".jCarouselLite-" + index).jCarouselLite({
        btnNext: ".next-" + index,
        btnPrev: ".prev-" + index
      });
    }
  });

});
