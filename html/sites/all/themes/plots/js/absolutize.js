jQuery.fn.absolutize = function()
{
  return this.each(function()
  {
    var element = jQuery(this);
    if (element.css('position') == 'absolute')
    {
      return element;
    }
 
    var offsets = element.offset();
    var top = offsets.top;
    var left = offsets.left;
    var width = element[0].clientWidth;
    var height = element[0].clientHeight;
 
    element._originalLeft = left - parseFloat(element.css("left") || 0);
    element._originalTop = top - parseFloat(element.css("top") || 0);
    element._originalWidth = element.css("width");
    element._originalHeight = element.css("height");
 
    element.css("position", "absolute");
    element.css("top", top + 'px');
    element.css("left", left + 'px');
    element.css("width", width + 'px');
    element.css("height", height + 'px');
    return element;
 
  });
}
