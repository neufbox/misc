$(document).ready(function(){
    $('#dhcp_active_on').click(function() {
	$('#row_dhcp_start').css('display', '');
	$('#row_dhcp_end').css('display', '');
	$('#row_dhcp_lease').css('display', '');
	$('#section_dhcp_static').css('display', '');
        $('#dhcp_off_warning').css('display','none');
    });
    
    $('#dhcp_active_off').click(function() {
	$('#row_dhcp_start').css('display', 'none');
	$('#row_dhcp_end').css('display', 'none');
	$('#row_dhcp_lease').css('display', 'none');
	$('#section_dhcp_static').css('display', 'none');
        $('#dhcp_off_warning').css('display','');
    });
    
    $('#dhcp_static tr:last').append('<td class="col_action">\
	  <button type="button" class="button_network" id="button_network"></button>\
	</td>');
    
    $('#button_network').click(function() {
	showLanPopup(completeAddStaticIp);
	return false;
    });
    
    $("input[name='dhcp_active']:checked").click();
    
    $('input[type=text]').attr('autocomplete', 'off');
});

function completeAddStaticIp(ip, mac, name) 
{
    if(ip) 
    {
	var ip_splited = ip.split('.');
	$('#dhcp_static_ipaddr_p3').val(ip_splited[3]);
    }

    if(mac) 
    {
	var mac_splited = mac.split(':');
	$('#dhcp_static_macaddr_p0').val(mac_splited[0]);
	$('#dhcp_static_macaddr_p1').val(mac_splited[1]);
	$('#dhcp_static_macaddr_p2').val(mac_splited[2]);
	$('#dhcp_static_macaddr_p3').val(mac_splited[3]);
	$('#dhcp_static_macaddr_p4').val(mac_splited[4]);
	$('#dhcp_static_macaddr_p5').val(mac_splited[5]);
    }
}
