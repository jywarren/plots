			/*
			 #		jQuery Slinky Slider Plugin
			 #		---------------------------
			 #		Version:			1.0
			 #		---------------------------
			 #		Author:				samhs
			 # 		http://ohwrite.co.uk/jquery/jquery-plugin-slinky-slider/
			 #		http://docs.jquery.com/Plugins/SlinkySlider
			 #
			 # 		Copyright (c) 2009 Sam Hampton-Smith
			 #
			 #		Dual licensed under the MIT and GPL licenses:
			 #	 	http://www.opensource.org/licenses/mit-license.php
			 #		http://www.gnu.org/licenses/gpl.html
			 #
			 #		Please view files mit.txt and gpl.txt for full license terms
			 #		And include these two files if you redistribute this software
			 */


;(function($) {
		   
			$.fn.slinkySlider = function(settings) {	
				// Utility variables - do not alter
				var currentpanel;
				var panelwidth;
				var goforward = true;
				var t;
				settings = $.extend({}, $.fn.slinkySlider.defaults, settings);	
				return $(this).each(function(){
					panelwidth = $(this).width();
					$(this).css("overflow","hidden");
					settings.largesize = panelwidth-((settings.numberofpanels-1)*(settings.smallsize+settings.panelspacing));
					var container = $(this);
					var elheight = container.offsetHeight();
					for (var i=1;i<=settings.numberofpanels;i++) {
						$(this).append("<div class='panelwrappers'><div class='panel'></div></div>");
						$(".panelwrappers:last .panel").load(settings.panelname+i+".html").parents(".panelwrappers").data("number",i);
					}
					currentpanel = $(".panelwrappers:first");
					$(".panelwrappers").css({
							"width"		:	settings.smallsize+"px", 
							"float"		:	"left",
							"height"	:	elheight+"px"});
					$(".panels").css({
							"width"		:	settings.largesize+"px",
							"height"	:	"100%"}); 
					$(currentpanel).css("width",settings.largesize+"px");
					$(".panelwrappers").not(":last").css("margin-right",settings.panelspacing+"px");
					$(".panelwrappers").each(function(){
						$(this).mouseover(function(){switchpanel(this);});
					});
					if (settings.doauto) t = setTimeout(function(){switchpanel(null);},settings.autotimer);
				});

				function switchpanel(newpanel) {
					if (newpanel==currentpanel) {
						// do nothing because we're already on this panel
					} else {
						var auto = false;
						if (newpanel==null) {
							auto = true;
							if (goforward && $(currentpanel).data("number")==settings.numberofpanels) {
								goforward=false;
							}
							if (!goforward && $(currentpanel).data("number")==1) {
								goforward=true;
							}						
							if (goforward) {
								newpanel = $(currentpanel).next();
							} else {
								newpanel = $(currentpanel).prev();
							}							
						}
						else {
							$(".panelwrappers").stop();
							clearTimeout(t);
						}
						$(".panelwrappers").not(newpanel).animate({width: settings.smallsize+"px"},settings.transition, "swing");
						$(newpanel).animate({width: settings.largesize+"px"},settings.transition, "swing");				
						currentpanel = newpanel;
						if (auto) t = setTimeout(function(){switchpanel(null);},settings.autotimer); 
				}
			}
		}
			
		$.fn.slinkySlider.defaults = {
			autotimer:8000,
			transition:1000,
			panelspacing:3,
			smallsize:20,
			numberofpanels:5,
			largesize:0,
			doauto:true,
			panelname:"panel"
		}

})(jQuery);