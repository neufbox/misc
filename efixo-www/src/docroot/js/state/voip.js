$(document).ready(function(){
    setInterval("sendRequest('/state/voip', updateStatus)", 1000);
});

function updateStatus(response)
{
    var voip_linestatus_val = getResponseAttrElement(response, 'linestatus', 'val');
    var voip_linestatus_text = getResponseAttrElement(response, 'linestatus', 'text');
    
    var voip_access_text = getResponseAttrElement(response, "access", 'text');
    var voip_access_val = getResponseAttrElement(response, "access", 'val');

    if (voip_linestatus_val == "up")
    {
	$('#voip_linestatus').attr('class', 'enabled');
    }
    else if (voip_linestatus_val == "down")
    {
	$('#voip_linestatus').attr('class', 'disabled');
    }
    
    if(voip_access_val == "gprs")
    {
        voip_linestatus_text = voip_linestatus_text + " (" + voip_access_text + ")";
    }

    $('#voip_linestatus').text(voip_linestatus_text);

    var voip_hookstatus_val = getResponseAttrElement(response, 'hookstatus', 'val');
    var voip_hookstatus_text = getResponseAttrElement(response, 'hookstatus', 'text');
    
    if (voip_hookstatus_val == "offhook")
    {
	$('#voip_hookstatus').attr('class', 'enabled');
    }
    else if (voip_hookstatus_val == "onhook")
    {
	$('#voip_hookstatus').attr('class', 'enabled');
    }
    else if (voip_hookstatus_val == "unknown")
    {
	$('#voip_hookstatus').attr('class', 'unused');
    }
    
    $('#voip_hookstatus').text(voip_hookstatus_text);
}