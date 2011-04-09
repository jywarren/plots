// Global killswitch
if (Drupal.jsEnabled) {
  $(document).ready(eventAutoAttach);
}

/**
 * Attaches the block update behaviour to links tagged with 'updateblock' class.
 */
function eventAutoAttach() {
  $("#block-event-0 div.content a.updateblock").click(function() {
    this.blockUpdater = new blockUpdater( $(this).parents(".content"), $(this).attr("href").replace("month", "block"), eventAutoAttach); 
    return false; 
    });

}

/**
 * create an instance of this object in the onClick handler for block update links.
 * 
 * could be separated into misc/blockupdater.js
 */

function blockUpdater(element,url,callback) {
  var blockUpdate = this;
  element.blockUpdate = this; 

  this.element = element;
  this.callback = callback;

  this.oldHTML = this.element.html();

  // Keep block at it's current width/height to make the update less disruptive
  this.styleHeight = $(element).height();
  this.styleWidth  = $(element).width();
  $(element).height(element[0].offsetHeight+"px");
  $(element).width(element[0].offsetWidth+"px");

  // Clear block contents
  $(element).html("");

  // Insert progressbar
  this.progress = new Drupal.progressBar('updateprogress');
  $(this.element).prepend(this.progress.element);

  var rel = this;
  var cancel = document.createElement("a");
  $(cancel).html("cancel").attr("alt","cancel").addClass("cancel-update")
    .attr("href", "#").bind("click", function() {
    rel.update("abort",undefined,blockUpdate);
    return false;
  });

  this.element.prepend($(cancel)); 

  this.dontUpdate = false;
 $.get(url,function(data){
     blockUpdate.update(data)});
}
blockUpdater.prototype.update = function (xmlHttp) {
  var blockUpdate=this;
  if(!blockUpdate.dontUpdate) {
   if(xmlHttp == 'abort') {
     blockUpdate.element.html(this.oldHTML);
     blockUpdate.element.append("<p class='calendar-log'>Update aborted.</p>");
    } else {
     blockUpdate.element.height(blockUpdate.styleHeight); 
     blockUpdate.element.width(blockUpdate.styleWidth);
     blockUpdate.element.html(xmlHttp);
     blockUpdate.dontUpdate = true;
   }
    blockUpdate.dontUpdate = true;
    if(blockUpdate.callback != undefined)
      blockUpdate.callback();
  }
}
