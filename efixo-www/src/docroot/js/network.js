$(document).ready(function(){
#ifdef CIBOX
    $('#stbconfig_dynamic').click(function() 
				  {
				      $('#stbconfig_iface').css('display', 'none');
				  });
    
    $('#stbconfig_config').click(function() 
				 {
				     $('#stbconfig_iface').css('display', '');
				 });
#endif
    
    setInterval("sendRequest('/network', updateStatus)", 1000);
});

function updateStatus(response)
{
    var ifaces = response.getElementsByTagName('iface');
    var iface_info_text = null;

    for(var i = 0; i < ifaces.length; i++)
    {
        var iface = ifaces[i];
	
        var iface_status_val = getResponseAttrElement(iface, "status", "val");
        var iface_type_val = getResponseAttrElement(iface, "type", "val");
	
        var name = getResponseElement(iface, "name");
	
        var x = $('#' + name + '_status');
        if(iface_status_val == "up")
        {
	    if(iface_type_val == "stb")
	    {
		iface_info_text = getResponseAttrElement(iface, "type", "text");
	    }
	    else
	    {
		iface_info_text = getResponseElement(iface, "speed");
	    }

            x.attr('class', 'enabled');
            x.text(iface_info_text);
        }
        else
        {
	    var iface_status_text = getResponseAttrElement(iface, "status", "text");
	    
            x.attr('class', 'unused');
            x.text(iface_status_text);
        }
    }
        
    var tvs = response.getElementsByTagName('tv');
        
    if(tvs.length > 0)
    {
        var tv = tvs[0];
        
        var tv_status_val = getResponseAttrElement(tv, 'status', 'val');
        var tv_status_text = getResponseAttrElement(tv, 'status', 'text');
	
        var x = $('#tv_status');
	
        if(tv_status_val == "up")
        {
            x.attr('class', 'enabled');
        }
        else
        {
            x.attr('class', 'unused');
        }
	
        x.text(tv_status_text);
    }

    var usbs = response.getElementsByTagName('usb');

    if(usbs.length > 0)
    {
	var usb = usbs[0];
	
	var usb_status_val = getResponseAttrElement(usb, 'status', 'val');
	var usb_status_text = getResponseAttrElement(usb, 'status', 'text');
	
        var x = $('#usb_status');
	
        if (usb_status_val == "up")
        {
            x.attr('class', 'enabled');
        }
        else if (usb_status_val == "down")
        {
            x.attr('class', 'unused');
        }
	
        x.text(usb_status_text);
    }
    
    var wifis = response.getElementsByTagName('wifi');
    
    if(wifis.length > 0)
    {
	var wifi = wifis[0];
	
        var wifi_status_val = getResponseAttrElement(wifi, 'status', 'val');
	var wifi_status_text = getResponseAttrElement(wifi, 'status', 'text');
        
        var x = $('#wifi_status');
        if (wifi_status_val == "on")
        {
            var wifi_clientcount_val = getResponseAttrElement(wifi, 'clientcount', 'val');
            var wifi_clientcount_text = getResponseAttrElement(wifi, 'clientcount', 'text');
	    
            x.attr('class', 'enabled');
	    
	    infotxt = "";
	    if(parseInt(wifi_clientcount_val) >= 2)
	    {
		infotxt += wifi_clientcount_val + ' ';
	    }
	    infotxt += wifi_clientcount_text;
	    
            x.text(wifi_status_text + ' (' + infotxt + ') ');
        }
        else if (wifi_status_val == "off")
        {
            x.attr('class', 'disabled');
            x.text(wifi_status_text);
        }
    }
}
