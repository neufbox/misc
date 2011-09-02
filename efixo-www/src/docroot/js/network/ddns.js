$(document).ready(function(){
    $('#ddns_active_on').click(function(){
	return hideDDNSItems();
    });
    
    $('#ddns_active_off').click(function() {
	return showDDNSItems();
    });
    
    $('a[rel=external]').attr('target', '_blank');
    
    $("input[name='ddns_active']:checked").click();
    
    setInterval("sendRequest('/network/ddns', updateStatus)", 1000);
});

function showDDNSItems()
{
    $('#row_ddns_service').css('display', 'none');
    $('#row_ddns_username').css('display', 'none');
    $('#row_ddns_password').css('display', 'none');
    $('#row_ddns_hostname').css('display', 'none');
}

function hideDDNSItems()
{
    $('#row_ddns_service').css('display', '');
    $('#row_ddns_username').css('display', '');
    $('#row_ddns_password').css('display', '');
    $('#row_ddns_hostname').css('display', '');
}

function updateStatus(response)
{
    var mtag = null;

    var status_text = null;
    var status_val = null;
    var active_text = null;
    var active_val = null;

    mtag = response.getElementsByTagName("ddns");
    if(mtag.length > 0)
    {
        active_text = getResponseAttrElement(mtag[0], 'active', 'text');
        active_val = getResponseAttrElement(mtag[0], 'active', 'val');

        status_text = getResponseAttrElement(mtag[0], 'status', 'text');
        status_val = getResponseAttrElement(mtag[0], 'status', 'val');
        
        $('#ddns_status').text(status_text);
    
        if(status_val == "up" || status_val == "updated" 
           || status_val == "starting")
        {
	    $('#ddns_status').attr('class', 'enabled');
        }
        else
        {
	    $('#ddns_status').attr('class', 'disabled');
        }
    }
}
