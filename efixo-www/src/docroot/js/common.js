$(document).ready(function(){
    $('#help_title').click(function() {
            var help_width = $('#help').width();
            if(help_width == 224) {
                    $('#help_text').hide();
                    $('#help').animate({ width: "70px"}, {duration: 200, queue: false});
                    $('#help_title').css({ paddingLeft: "0px" });
                    $('#help_title span').css({ visibility: "hidden" });
                    $('#main').animate({marginRight: "70px"},{duration: 200, queue: false});

            }else if(help_width == 70) {
                    $('#help').animate(
                        { width: "224px"},
                        {duration: 200, complete: function(){
                                                $("#help_text").show();
                                        }
                        });
                    $('#help_title span').css({ visibility: "visible" });
                    $('#help_title').css({ paddingLeft: "72px" });
                    $('#main').animate({marginRight: "235px"},{duration: 200, queue: false});
            }
        }
    );
    
    if($('.success_notice'))
    {
	setTimeout(function() {$('.success_notice').slideUp("slow")}, 5000);
    }
});

function getPage(url, callback, getData) {
    if(!getData)
    {
	getData = {};
    }
    
    jQuery.ajax({
	type: "GET",
	dataType: "html",
	url: url,
	data: getData,
	cache: false,
	success: callback,
	beforeSend: function(xhrObj) {
            xhrObj.setRequestHeader('X-Requested-Handler', 'display');
	}
    });
}

function sendRequest(url, callback, postData)
{
    var method;
  
    if(postData) 
    {
	method = "POST";
    } else 
    {
	method = "GET";
	postData = {};
    }
  
    jQuery.ajax({
	type: method,
	url: url,
	cache: false,
	data: postData,
	success: function(data, textStatus) {
	    /* test return */
	    if(getResponseAttrElement(data, 'rsp', 'stat') == 'fail')
	    {
		if(getResponseAttrElement(data, 'err', 'code') == '115')
		{
		    /* need auth */
                    if(url != "/reboot")
                    {
		        page_red = '/login?page_ref=' + url;
                    }
                    else
                    {
                        page_red = '/login';
                    }
		    
		    window.location = page_red;
		    
		    return;
		}
	    }
            
	    callback(data, textStatus);
	},
	dataType: "xml",
	beforeSend: function(xhrObj) {
            xhrObj.setRequestHeader('X-Requested-Handler', 'ajax');
	}
    });
}

function existResponseAttrElement(r, e, a)
{
    return (r.getElementsByTagName(e).length > 0
            && r.getElementsByTagName(e)[0]);
}

function existResponseElement(r, e, a)
{
    return (r.getElementsByTagName(e).length > 0
            && r.getElementsByTagName(e)[0].firstChild);
}

function getResponseAttrElement(r, e, a)
{
    var ret;
    
    if(r.getElementsByTagName(e).length > 0)
    {
        ret = (r.getElementsByTagName(e)[0] ? 
	       r.getElementsByTagName(e)[0].getAttribute(a) : '');
    }
    else
    {
#ifdef DEBUG
        console.error("getResponseAttrElement(" + e + ", " + a + ") error");
#endif
        ret = null;
    }
        
    return ret;
}


function getResponseElement(r, e)
{
    var ret;
        
    if(r.getElementsByTagName(e).length > 0)
    {
        ret = (r.getElementsByTagName(e)[0].firstChild ? 
	       r.getElementsByTagName(e)[0].firstChild.nodeValue : '');
    }
    else
    {
#ifdef DEBUG
        console.error("getResponseElement(" + e + ") error");
#endif
        ret = null;
    }
        
    return ret;
}

var callbackLanPopup;

function showLanPopup(callback, portfilter) 
{
    $('body').append('<div class="popup">\
  <div class="popup_window">\
    <div class="clearfix">\
      <a href="#" class="jqmClose"><em>Fermer</em></a>\
    </div>\
    <div class="popup_content">\
      <div align="center">Chargement ...</div>\
	  </div>\
  </div>\
</div>');
    
    callbackLanPopup = callback;
  
    $('.popup').jqm({trigger: false, 
		     overlay: 25,
		     onShow: function(h) 
		     {
			 if(!portfilter)
			 {
			     portfilter = null;
			 } 
			 else
			 {
			     portfilter = {'portfilter' : portfilter};
			 }

			 getPage('/lanpopup', 
				 function(response) 
				 {
				     $('.popup_content').html(response);
				 }, 
				 portfilter);
                    
			 h.w.show();
		     },
		     onHide: function(h) 
		     {
                         h.w.hide();
                         if(h.o) {
                             h.o.css('background-color', '#fff');
                             h.o.css('opacity', '0');
                             h.o.css('z-index', '-1');
                             h.o.hide();
                             h.o.remove();
                         }
                         h.w.remove();
		     }});
    $('.popup').jqmShow(); 
}

function validLanPopup() {
    var num = $("input[name=computer_selector]:checked").val();
    var mac = $('input[id=mac_' + num + ']').val();
    var ip = $('input[id=ip_' + num + ']').val();
    var name = $('input[id=name_' + num + ']').val();
    
    callbackLanPopup(ip, mac, name);
    
    $('.popup').jqmHide();
}

function onKeyPressIpAddress(event)
{
        var key;
        if($.browser.mozilla) {
                key = event.which;
        } else {
                key = event.keyCode;
        }

        if(key == 190 || key == 46) {
                $(this).next().focus().select();
                return false;
        } else if(key == 8) {
                if($(this).val() == '') {
                        $(this).prev().focus();
                        return false;
                }
        }

        return true;
}
