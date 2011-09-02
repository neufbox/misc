$(document).ready(function(){
    $('#upnpav_active_on').click(function()
				 {
				     return showUpnpavItems();
				 });
    
    $('#upnpav_active_off').click(function()
				  {
				      return hideUpnpavItems();
				  });
    
    $("input[name='upnpav_active']:checked").click();
    
    setInterval("sendRequest('/service/upnpav', updateStatus)", 1000);
});

function showUpnpavItems()
{
    $('#row_upnpav_name').css('display', '');
    $('#title_upnpav_share').css('display', '');
    $('#content_upnpav_share').css('display', '');
}

function hideUpnpavItems()
{
    $('#row_upnpav_name').css('display', 'none');
    $('#title_upnpav_share').css('display', 'none');
    $('#content_upnpav_share').css('display', 'none');
}

function updateStatus(response)
{
    var active_text = getResponseAttrElement(response, 'active', 'text');
    var active_val = getResponseAttrElement(response, 'active', 'val');

    var status_text = getResponseAttrElement(response, 'status', 'text');
    var status_val = getResponseAttrElement(response, 'status', 'val');

    var sharedcount_val = getResponseAttrElement(response, 'sharedcount', 'val');
    var sharedcount_posttext = getResponseAttrElement(response, 'sharedcount', 'posttext');
    
    var errno_val = getResponseAttrElement(response, 'errno', 'val');
    var errno_text = getResponseAttrElement(response, 'errno', 'text');
    
    $('#upnpav_status').text(status_text);
    
    if(status_val == "up" || status_val == "starting")
    {
	$('#col_upnpav_status').attr('class', 'enabled');
	$('#upnpav_sharedcount').text(" ( " + sharedcount_val + " " + sharedcount_posttext + " )");
    }
    else
    {
	$('#col_upnpav_status').attr('class', 'disabled');
	$('#upnpav_sharedcount').text("");
    }

    if(errno_val != "")
    {
	$('#upnpav_errno').text(" [ " + errno_text + " ] ");
    }
    else
    {
	$('#upnpav_errno').text("");
    }
}
