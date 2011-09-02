$(document).ready(function(){
    $('#samba_active_on').click(function()
				{
				    return showSambaItems();
				});
    
    $('#samba_active_off').click(function()
				 {
				     return hideSambaItems();
				 });
    
    $("input[name='samba_active']:checked").click();
    
    setInterval("sendRequest('/service/samba', updateStatus)", 1000);
});


function hideSambaItems()
{
    $('#row_samba_workgroup').css('display', 'none');
    $('#row_samba_name').css('display', 'none');
    $('#title_samba_share').css('display', 'none');
    $('#content_samba_share').css('display', 'none');
}

function showSambaItems()
{
    $('#row_samba_workgroup').css('display', '');
    $('#row_samba_name').css('display', '');
    $('#title_samba_share').css('display', '');
    $('#content_samba_share').css('display', '');
}

function updateStatus(response)
{
    var active_text = getResponseAttrElement(response, 'active', 'text');
    var active_val = getResponseAttrElement(response, 'active', 'val');

    var state_text = getResponseAttrElement(response, 'state', 'text');
    var state_val = getResponseAttrElement(response, 'state', 'val');

    var codeno_text = getResponseAttrElement(response, 'codeno', 'text');
    var codeno_val = getResponseAttrElement(response, 'codeno', 'val');
    
    if(state_val == "down")
    {
	$('#col_samba_status').attr('class', 'disabled');
    }
    else if(state_val == "up")
    {
	$('#col_samba_status').attr('class', 'enabled');
    }
    
    if(active_val == "off")
    {
	msg = active_text;
    }
    else
    {
	msg = state_text;

	if(codeno_val != "")
	{
		msg += " [ " + codeno_text + " ] ";
	}
    }
    
    $('#col_samba_status').text(msg);
}
