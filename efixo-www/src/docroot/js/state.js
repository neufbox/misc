#ifndef CIBOX
var leds = new Array( "dsl", 
                      "traffic",
                      "tel",
                      "tv",
                      "wifi",
                      "alarm" );

function updateLed(ledid, state)
{
    if(state == "toggle") 
    {
        $(ledid).attr('class') == "on" ? selclass = "off" : selclass = "on";
    }
    else
    {
        selclass = state;
    }
    
    $(ledid).attr('class', selclass);
}
#endif /* #ifndef CIBOX */

var uts = new Array( "day", 
                     "hour",
                     "min",
                      "sec" );

function updateStatus(response)
{
    var a_var = new Array("internet", "voip", "tv");
    var n;
    
    var status_text;
    var status_val;
    var access_text;
    var access_val;
    var x;

    var mtag;
    
    for(n = 0; n < a_var.length; n++)
    {
        mtag = response.getElementsByTagName(a_var[n]);
        if(mtag.length > 0)
        {
	    status_text = getResponseAttrElement(mtag[0], "link", 'text');
	    status_val = getResponseAttrElement(mtag[0], "link", 'val');
            
	    access_text = getResponseAttrElement(mtag[0], "access", 'text');
	    access_val = getResponseAttrElement(mtag[0], "access", 'val');

            x = $('#' + a_var[n] + '_status');
	
	    if(status_val == "up")
	    {
	        x.attr('class', 'enabled');
	    }
	    else
	    {
	        x.attr('class', 'disabled');
	    }
	    
            if(access_val == "gprs")
            {
                status_text = status_text + " (" + access_text + ")";
            }

	    x.text(status_text);
        }
    }
    
#ifndef CIBOX
    mtag = response.getElementsByTagName("leds");
    if(mtag != null && mtag.length > 0)
    {
        for(led in leds)
        {
            updateLed('#led_' + leds[led], 
                      getResponseElement(mtag[0], leds[led]));
        }
    }
#endif /* #ifndef CIBOX */

    mtag = response.getElementsByTagName("uptime");
    if(mtag != null && mtag.length > 0)
    {
        var uptime = ""
        var utval = null
        var utposttext = null
        var uptime_is_completed = 1;

        for(ut in uts)
        {
            utval = getResponseAttrElement(mtag[0], uts[ut], "val");
            utposttext = getResponseAttrElement(mtag[0], uts[ut], "posttext");

            if(utval && utposttext)
            {
                uptime += utval + " " + utposttext + " ";
            }
            else
            {
                uptime_is_completed = 0;
                break;
            }
        }

        if(uptime_is_completed)
        {
            $('#row_uptime').text(uptime);
        }
#ifdef DEBUG
        else
        {
            console.error("uptime construction in javascript not complete !");
        }
#endif
    }
}

$(document).ready(function(){
    setInterval("sendRequest('/state', updateStatus)", 1100);
});
