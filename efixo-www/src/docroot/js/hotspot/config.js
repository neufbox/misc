$(document).ready(function(){
    setInterval("sendRequest('/hotspot/config', updateStatus)", 1000);
    
    $('#hotspot_active_on').click(function()
				  {
				      return showHotspotItems();
				  });
    
    $('#hotspot_active_off').click(function()
				   {
				       return hideHotspotItems();
				   });
    
    $("input[name='hotspot_active']:checked").click();
});

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
    
    if($('#hotspot_active_on').is(':checked') && ssid_val.length > 0)
    {
        $('#row_hotspot_ssid').css('display', '');
        $('#hotspot_ssid').text(ssid_val);
    }
    else
    {
        $('#row_hotspot_ssid').css('display', 'none');
    }
    
    x.text(status_text);
    if(status_val == "up")
    {
        x.attr('class', 'enabled');
    }
    else if(status_val == "starting" || status_val == "stopping")
    {
	x.attr('class', 'unused');
    }
    else
    {
	x.attr('class', 'disabled');
    }
    
    /* Return to stable state: enable button and hide work in progress */
    if (active_val != "off" && status_val != "down" 
	&& status_val != "starting" && status_val != "stopping") 
    {
	$('#push_the_button').attr('disabled', false);
	$('#div_submit').attr('class', 'button_submit');
	$('#work_in_progress').css('display', 'none');
    }
    else if(active_val == "off" && status_val == "down")
    {
	$('#push_the_button').attr('disabled', false);
	$('#div_submit').attr('class', 'button_submit');
	$('#work_in_progress').css('display', 'none')
    }
}

function showHotspotItems()
{
    if($('#hotspot_active_on').is(':checked')
       && $('#hotspot_ssid').text().length > 0)
    {
        $('#row_hotspot_ssid').css('display', '');
    }
    
    $('#row_hotspot_mode').css('display', '');
    $('#row_hotspot_conditions').css('display', '');
    $('#row_hotspot_conditionserror').css('display', '');
    $('#row_hotspot_chosefon').css('display', '');
}

function hideHotspotItems()
{
    $('#row_hotspot_ssid').css('display', 'none');
    $('#row_hotspot_mode').css('display', 'none');
    $('#row_hotspot_conditions').css('display', 'none');
    $('#row_hotspot_conditionserror').css('display', 'none');
    $('#row_hotspot_chosefon').css('display', 'none');
}
