function updateStatus(response) {
    var ecos = response.getElementsByTagName('eco');

    if(ecos.length > 0)
    {

	var eco = ecos[0];

        var eco_status_val = getResponseAttrElement(eco, 'link', 'val');
	var eco_status_text = getResponseAttrElement(eco, 'link', 'text');
        var x = $('#eco_status');
        if (eco_status_val == "up")
        {
            x.attr('class', 'enabled');
	    x.text(eco_status_text);
        }
        else if (eco_status_val == "down")
        {
            x.attr('class', 'disabled');
            x.text(eco_status_text);
        }
    }
}

$(document).ready(function(){
        setInterval("sendRequest('/eco', updateStatus)", 1100);
});