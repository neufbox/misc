function updateStatus(response)
{
    var enable_text = getResponseAttrElement(response, 'enable', 'text');
    var enable_val = getResponseAttrElement(response, 'enable', 'val');
    
    var active_text = getResponseAttrElement(response, 'active', 'text');
    var active_val = getResponseAttrElement(response, 'active', 'val');

    var status_text = getResponseAttrElement(response, 'status', 'text');
    var status_val = getResponseAttrElement(response, 'status', 'val');
    
    var ssid_val = getResponseAttrElement(response, 'ssid', 'val');
    
    var x = $('#hotspot_status');
    
    if(enable_val == "false")
    {
        $('#ctn_hotspot_disable').css('display', '');
        $('#ctn_hotspot_enable').css('display', 'none');
    }
    else
    {
        $('#ctn_hotspot_disable').css('display', 'none');
        $('#ctn_hotspot_enable').css('display', '');
    }

    if(active_val == "off")
    {
	x.attr('class', 'disabled');
	x.text(active_text);
    }
    else
    {
	if(status_val == "up")
	{
	    x.attr('class', 'enabled');

            $('#row_ssid').css('display', '');
            $('#col_ssid').text(ssid_val);
	}
        else
        {
            $('#row_ssid').css('display', 'none');
            
	    if(status_val == "starting" || status_val == "stopping")
	    {
	        x.attr('class', 'unused');
	    }
	    else
	    {
	        x.attr('class', 'disabled');
	    }
        }
	
	x.text(status_text);
    }
}

$(document).ready(function(){
    setInterval("sendRequest('/hotspot', updateStatus)", 1800);
});
