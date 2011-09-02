var zombie_status = false;
var no_response = false;

$(document).ready(function(){
    if($('#rebootingstatus').length > 0)
    {
	$.ajaxSetup({timeout : 150, error: boxHasZombified});
	
	setInterval("sendRequest('/reboot', updateStatus)", 1500);
    }
});

function boxHasZombified(XMLHttpRequest, textStatus, errorThrown)
{
    zombie_status = true;
}

function updateStatus(response)
{
    var status = getResponseElement(response, 'status');
    
    if(status)
    {
	if(zombie_status)
	{
	    /* alive ! redirect it to home page */
	    zombie_status = false;
	    window.location = '/index';
	}
    }   
}
