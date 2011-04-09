// $Id: to_do_block.js,v 1.2.2.1 2010/05/22 08:32:47 alexiswilke Exp $

var toDo = {
  urgentPageCount: 1,
  allPageCount: 1,
  curSelection: 'urgent',
  curPage: 0,
  pageCount: 1,

  buildLink:function(pageNumber, text, selection){
    return '<a href="' + Drupal.settings.toDo.basePath + 'to_do/json/' + selection + '/' + pageNumber + '">'
        + text
      + "</a>";
  },

  pagination:function(currentPage, tabPages, selection){
    currentPage = Number(currentPage)
    var output = '';
    if(tabPages > 1) {
      output += '<ul class="pager">';
      if(currentPage == 1){
        output += '<li class="page-current">«</li>\n';
      }
      else{
        output += '<li class="page-item">' + toDo.buildLink(currentPage - 1, "«", selection) + '</li>\n';
      }
      for(var i = 1; i <= tabPages; i++) {
        if(i == currentPage) {
          output += '<li class="pager-current">' + i + '</li>\n';
        }
        else if(i == 1  || i == (currentPage - 1) || i == (currentPage + 1) || i == tabPages) {
          output += '<li class="pager-item">' + toDo.buildLink(i, i, selection) + '</li>\n';
        }
        else if(i == currentPage + 2 || i == currentPage - 2) {
          output += '<li class="pager-current">...</li>\n';
        }
      }
      if(currentPage == tabPages){
        output += '<li class="page-current">»</li>\n';
      }
      else{
        output += '<li class="pager-item">' + toDo.buildLink(currentPage + 1, "»", selection) + '</li>\n';
      }
      output += '</ul>';
    }
    return output;
  },
  
  setTabHandlers:function(){
    // handle clicks on tabs
    $("#block-to_do_block-0 ul.primary li a").each(function(){
      var url=$(this).attr('jsref');
      $(this).attr('href',url);
      $(this).click(function(){
        $('#block-to_do_block-0 h2').append(' <span class="to-do-block-throbbing">&nbsp;</span>');
        var changeTabClass=($(this).attr("href")).search(toDo.curSelection)<0;
        if(changeTabClass){
          toDo.curSelection=toDo.curSelection=="urgent"?"all":"urgent";
        }
        toDo.pageCount=toDo.curSelection=="urgent"?toDo.urgentPageCount:toDo.allPageCount;
        toDo.action($(this),true,changeTabClass);
        return false;
      });
    });
  },
  
  setPaginationHandlers:function(){
    $("#to_do_pagination a").each(function() {
      $(this).click(function() {
        toDo.action($(this));
        return false;
      });
    });
  },
  
  action:function(e, tabs, changeTabClass){
    var targetHref = e.attr("href");
    var pieces = targetHref.split("/");
    toDo.curPage = pieces[pieces.length - 1];
    $.getJSON(targetHref, null, function(data){
      $("#block-to_do_block-0 div.content").fadeOut("fast", function(){
        if(tabs && changeTabClass){
          $("#block-to_do_block-0 ul.primary li").toggleClass("active");
        }
        $("#block-to_do_block-0 h2").html(data.title);
        $("#block-to_do_block-0 div table tbody").parent().replaceWith(data.table);
        $("#to_do_pagination").replaceWith(
          '<div id="to_do_pagination" class="item-list">'
            + toDo.pagination(toDo.curPage, toDo.pageCount, toDo.curSelection)
          + '</div>');
        toDo.setPaginationHandlers();
      }).fadeIn("normal");
    });
  }
};

Drupal.behaviors.toDo=function(context){
  $('body:not(.to-do-initialized)', context).addClass('to-do-initialized').each(function(){
    toDo.urgentPageCount = Drupal.settings.toDo.urgentPageCount;
    toDo.allPageCount = Drupal.settings.toDo.allPageCount;
    toDo.curSelection = Drupal.settings.toDo.selection ? 'all' : 'urgent';
    toDo.curPage = Drupal.settings.toDo.page;
    toDo.pageCount = toDo.curSelection == "urgent" ? toDo.urgentPageCount : toDo.allPageCount;
    toDo.setTabHandlers();
    $("#to_do_pagination").append(toDo.pagination(toDo.curPage, toDo.pageCount, toDo.curSelection));
    toDo.setPaginationHandlers();
  });
};

// vim: ts=2 sw=2 et
