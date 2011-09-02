var ping_loop = null;
var ping_id = null;
var ping_ip = null;
var traceroute_loop = null;
var traceroute_id = null;
var traceroute_ip_subm = null;
var traceroute_ip = null;

$(window).unload(function() {
    if(traceroute_loop) 
    {
	clearInterval(traceroute_loop);
    }
		     
    if(traceroute_id) 
    {
	sendRequest('/maintenance/tests',
		    null,
		    {'action' : 'traceroute', 'run' : 'stop', 'id' : traceroute_id});
    }
		     
    if(ping_loop)
    {
	clearInterval(ping_loop);
    }
    
    if(ping_id) 
    {
	sendRequest('/maintenance/tests',
		    null,
		    {'action' : 'ping', 'run' : 'stop', 'id' : ping_id});
    }
});

$(document).ready(function() {
    $('#button_ping').click(function(event) {
	if(ping_loop)
	{
	    stopPing();
	}
	else
	{
	    startPing();
	}
    });
    
    $('#button_traceroute').click(function(event) {
	if(traceroute_loop)
	{
	    stopTraceroute();
	}
	else
	{
	    startTraceroute();
	}
    });
    
    /* when press <enter> on ping hostname input, launch ping */
    $('#ping_dest_hostname').keypress(function(event) {
	var key;
	if($.browser.mozilla) 
	{
	    key = event.which;
	} 
	else
	{
	    key = event.keyCode;
	}
	
	if(key == 13) 
	{
	    $('#button_ping').click();
	}
    });
    
    /* when press <enter> on traceroute hostname input, launch traceroute */
    $('#traceroute_dest_hostname').keypress(function(event) {
	var key;
	if($.browser.mozilla) 
	{
	    key = event.which;
	} 
	else 
	{
	    key = event.keyCode;
	}
	
	if(key == 13) 
	{
	    $('#button_traceroute').click();
	}
    });

    /* Unobtrusive js */
    $('#ping_nojs_content,#traceroute_nojs_content').css('display', 'none');
    $('#ping_content,#traceroute_content').css('display', '');
});

/********** PING PART *************/

function showStatusPing(elem)
{
    $('#ping_status > span:not(' + elem + ')').css('display', 'none');
    $(elem).css('display', '');
    $('#ping_status').css('display', '');
}

function endPing(msg)
{
    if(ping_loop) 
    {
	clearInterval(ping_loop);
	ping_loop = null;
    }
    
    /* show msg */
    showStatusPing('#ping_status_custom');
    $('#ping_status_custom').text(msg);
    $('#ping_status > span').attr('class', '');
    
    /* put start button active */
    $('#button_ping_launch').css('display', '');
    $('#button_ping_stop').css('display', 'none');
    
    /* enable form */
    $('#button_ping').attr('disabled', false);
    $('#div_submit_ping').attr('class', 'button_submit');
    $('#ping_dest_hostname').attr('disabled', false);
}

/*
 * Called when user pressed Start button in ping section
 */
function startPing()
{
    /* init vars */
    ping_id = null;
    ping_loop = null;
    ping_ip = null;
    
    /* get ip for the ping */
    ping_ip = $('#ping_dest_hostname').val();

    /* disable form */
    $('#ping_dest_hostname').attr('disabled', true);
    $('#button_ping').attr('disabled', true);
    $('#div_submit_ping').attr('class', 'button_submit_disabled');
    
    /* put status ping in correct way */
    $('#ping_status > span').attr('class', 'loading');
    $('#ping_status > b').text(ping_ip);
    showStatusPing('#ping_status_starting');
    
    /* init counters */
    $('#ping_sent').text('0');
    $('#ping_recv').text('0');
    $('#ping_avgrtt').text('0');
    
    /* send first ajax request */
    sendRequest('/maintenance/tests',
		ajaxCbInitPing,
		{'action' : 'ping', 'run' : 'start', 'ping_dest_hostname' : ping_ip});
}

/*
 * Called when user pressed "Stopper" button in ping section
 */
function stopPing()
{
    /* halt loop */
    if(ping_loop) 
    {
	clearInterval(ping_loop);
	ping_loop = null;
    }
    
    /* send stop ajax request */
    if(ping_id) 
    {
	/* put status ping in correct way */
	showStatusPing('#ping_status_stopping');
	
	sendRequest('/maintenance/tests',
		    ajaxCbStopPing,
		    {'action' : 'ping', 'run' : 'stop', 'id' : ping_id});
    }
}

/*
 * Called when ping finish
 */
function ajaxCbInitPing(response) 
{
    /* get init ajax packet */
    var status_val = getResponseAttrElement(response, 'status', 'val');
    var status_text = getResponseAttrElement(response, 'status', 'text');

    if(status_val != "living") 
    {
	endPing(status_text);
    } 
    else
    {
	ping_id = getResponseElement(response, 'id');
	
	/* put stop button active */
	$('#button_ping').attr('disabled', false);
	$('#button_ping_launch').css('display', 'none');
	$('#button_ping_stop').css('display', '');
	$('#div_submit_ping').attr('class', 'button_submit');
	
	/* change status msg */
	showStatusPing('#ping_status_custom');
	$('#ping_status_custom').text(status_text);
	
	ping_loop = setInterval("sendRequest('/maintenance/tests', ajaxCbUpdatePing, {'action' : 'ping', 'run' : 'status', 'id' : '" + ping_id + "'})", 1700);
    }
}

function ajaxCbUpdatePing(response)
{
    var ping_status_val = getResponseAttrElement(response, 'status', 'val');
    
    if(ping_status_val == 'halt') 
    {
	/* ping halted anormally ! */
	var ping_status_text = getResponseAttrElement(response, 'status', 'text');
	
	endPing(ping_status_text);
	
	return;
    }
    
    /* packet sent */
    var sent = getResponseElement(response, 'sent');
    
    var x = $('#ping_sent');
    x.text(sent);
    
    /* packet received */
    var received = getResponseElement(response, 'received');
    
    var x = $('#ping_recv');
    x.text(received);
    
    /* packet triptime */
    var avgrtt = getResponseElement(response, 'avgrtt');
    
    var x = $('#ping_avgrtt');
    x.text(avgrtt + " ms");
    
    if(ping_status_val == 'finished')
    {
	/* ping finished correctly */
	var ping_status_text = getResponseAttrElement(response, 'status', 'text');
	
	endPing(ping_status_text);
    }
}

function ajaxCbStopPing(response)
{
    endPing(getResponseAttrElement(response, 'status', 'text'));
}

/********** TRACEROUTE PART *************/

function showStatusTraceroute(elem)
{
    $('#traceroute_status > span:not(' + elem + ')').css('display', 'none');
    $(elem).css('display', '');
    $('#traceroute_status').css('display', '');
}

function endTraceroute(msg)
{
    if(traceroute_loop) 
    {
	clearInterval(traceroute_loop);
	traceroute_loop = null;
    }
    
    /* make null some vars */
    traceroute_ip = null;
    traceroute_id = null;
    
    /* show msg */
    showStatusTraceroute('#traceroute_status_custom');
    $('#traceroute_status_custom').text(msg);
    $('#traceroute_status > span').attr('class', '');
    
    /* put start button active */
    $('#button_traceroute_launch').css('display', '');
    $('#button_traceroute_stop').css('display', 'none');
    
    /* enable form */
    $('#button_traceroute').attr('disabled', false);
    $('#div_submit_traceroute').attr('class', 'button_submit');
    $('#traceroute_dest_hostname').attr('disabled', false);
}

function startTraceroute()
{
    /* init vars */
    traceroute_id = null;
    traceroute_loop = null;
    
    /* get target */
    traceroute_ip_subm = $('#traceroute_dest_hostname').val();
    
    /* disable form */
    $('#traceroute_dest_hostname').attr('disabled', true);
    $('#button_traceroute').attr('disabled', true);
    $('#div_submit_traceroute').attr('class', 'button_submit_disabled');

    /* put status_traceroute in good way */
    $('#traceroute_status > span').attr('class', 'loading');
    $('#traceroute_status > b').text(traceroute_ip_subm);
    showStatusTraceroute('#traceroute_status_starting');
    
    /* clean up result container */
    $('#traceroute_result tbody').remove();

    sendRequest('/maintenance/tests',
		ajaxCbInitTraceroute,
		{'action' : 'traceroute', 'run' : 'start', 'traceroute_dest_hostname' : traceroute_ip_subm});
}

function stopTraceroute()
{
    /* halt loop */
    if(traceroute_loop) 
    {
	clearInterval(traceroute_loop);
	traceroute_loop = null;
    }
    
    /* send stop ajax request */
    if(traceroute_id) 
    {
	/* put status traceroute in correct way */
	showStatusTraceroute('#traceroute_status_stopping');
	
	sendRequest('/maintenance/tests',
		    ajaxCbStopTraceroute,
		    {'action' : 'traceroute', 'run' : 'stop', 'id' : traceroute_id});
    }
}

function ajaxCbInitTraceroute(response) 
{
    /* get init ajax packet */
    var status_val = getResponseAttrElement(response, 'status', 'val');
    var status_text = getResponseAttrElement(response, 'status', 'text');
    
    if(status_val != "living") 
    {
	endTraceroute(status_text);
    } 
    else 
    {
	traceroute_id = getResponseElement(response, 'id');
	    
	/* put stop button active */
	$('#button_traceroute').attr('disabled', false);
	$('#button_traceroute_launch').css('display', 'none');
	$('#button_traceroute_stop').css('display', '');
	$('#div_submit_traceroute').attr('class', 'button_submit');
	
	/* change status msg */
	showStatusTraceroute('#traceroute_status_custom');
	$('#traceroute_status_custom').text(status_text);
	    
	/* show result table */
	$('#traceroute_result').css('display', '');
	$('#traceroute_result table').append('<tbody></tbody>');
	
	traceroute_loop = setInterval("sendRequest('/maintenance/tests', ajaxCbUpdateTraceroute, {'action' : 'traceroute', 'run' : 'status', 'id' : '" + traceroute_id + "'})", 1700);
    }
}

function ajaxCbUpdateTraceroute(response) 
{
    var traceroute_status_val = getResponseAttrElement(response, 'status', 'val');
    
    if(traceroute_status_val == 'halt')
    {
	/* traceroute halted anormally ! */
	var traceroute_status_text = getResponseAttrElement(response, 'status', 'text');
	
	endTraceroute(traceroute_status_text);
	
	return;
    }
    
    /* formated output */
    var hop;
    var index;
    var hostname;
    var rtt;
    var ip;
    
    
    if(traceroute_ip == null)
    {
	ip = getResponseElement(response, 'target');
	if(ip != null
	   && ip != traceroute_ip_subm)
	{
	    traceroute_ip = ip;
	    $('#traceroute_status > b').text($('#traceroute_status > b').text() + ' - ' + traceroute_ip);
	}
    }

    var hops = response.getElementsByTagName('hop');
    $('#traceroute_result tbody').text('');
    for(var i = 0; i < hops.length; i++) 
    {
	hop = hops[i];
	index = hop.getElementsByTagName("index")[0].firstChild.nodeValue;
	if(hop.getElementsByTagName("hostname")[0].firstChild) 
	{
	    hostname = hop.getElementsByTagName("hostname")[0].firstChild.nodeValue;
	} 
	else 
	{
	    hostname = 'unknown';
	}
	
	if(hop.getElementsByTagName("ip")[0].firstChild) 
	{
	    ip = hop.getElementsByTagName("ip")[0].firstChild.nodeValue;
	} 
	else 
	{
	    ip = 'unknown';
	}

	if(ip != "*") 
	{
	    rtt = hop.getElementsByTagName("rtt")[0].firstChild.nodeValue + "ms";
	} 
	else 
	{
	    rtt = '';
	}
	
	$('#traceroute_result tbody').append("<tr><td>" + index + "</td><td>" + hostname + " (" + ip + ") </td><td>" + rtt + "</td></tr>");  
    }
    
    if(traceroute_status_val == 'finished') 
    {
	/* traceroute finished */
	var traceroute_status_text = getResponseAttrElement(response, 'status', 'text');
	
	endTraceroute(traceroute_status_text);
    }
}

function ajaxCbStopTraceroute(response)
{
    endTraceroute(getResponseAttrElement(response, 'status', 'text'));
}
