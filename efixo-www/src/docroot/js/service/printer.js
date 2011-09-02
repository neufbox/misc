$(document).ready(function(){
    setInterval("sendRequest('/service/printer', updateStatus)", 1000);
});

function updateStatus(response)
{
    var status_text = getResponseAttrElement(response, 'status', 'text');
    var status_val = getResponseAttrElement(response, 'status', 'val');
    
    $('#PrinterSharingStatus').text(status_text);
    
    if(status_val == "up" || status_val == "starting")
    {
	$('#PrinterSharingStatus').attr('class', 'enabled');
    }
    else
    {
	$('#PrinterSharingStatus').attr('class', 'disabled');
    }
}
