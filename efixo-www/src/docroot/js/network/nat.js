$(document).ready(function(){
    $('#nat_range').change(function() {
        return changeRange($('#nat_range'));
    });

    $('#nat_dstrange_p0').keyup(function() {
	autoAdaptPortRange();
    });
    $('#nat_extrange_p1').keyup(function() {
	autoAdaptPortRange();
    });
    $('#nat_extrange_p0').keyup(function() {
	autoAdaptPortRange();
    });

    $('#upnpd_active_on').click(function() {
	$('#row_upnp_nat_rules').css('display', '');
    });

    $('#upnpd_active_off').click(function() {
	$('#row_upnp_nat_rules').css('display', 'none');
    });

    $('#dmz_active_on').click(function() {
	$('#row_dmz_ip').css('display', '');
    });
    $('#dmz_active_off').click(function() {
	$('#row_dmz_ip').css('display', 'none');
    });

    $('.text.ip').keypress(onKeyPressIpAddress);

    $('input[type="text"]').attr('autocomplete', 'off');

    $('#button_network').click(function() {
        showLanPopup(completeAddIpDmz);
        return false;
    });

    changeRange($('#nat_range'));

    loaderNatMenu();

    $('td#nat_dstip').append('<button type="button" class="button_network" id="button_network_nat" style="vertical-align:middle;"></button>');
    $('#button_network_nat').click(function() {
        showLanPopup(completeAddIpNat);
        return false;
    });

    $('.button_network').css('visibility', 'visible');
});

function autoAdaptPortRange() {
    var lextp = parseInt($('#nat_extrange_p0').val());
    var rextp = parseInt($('#nat_extrange_p1').val());
    var ldstp = parseInt($('#nat_dstrange_p0').val());

    if(!isNaN(lextp) && !isNaN(rextp) && !isNaN(ldstp) && rextp >= lextp) {
	var new_value_dstrange_p1 = ldstp + (rextp - lextp);

	if(!isNaN(new_value_dstrange_p1)) {
	    $('#nat_dstrange_p1').val(new_value_dstrange_p1);
	}
    }
}

function changeRange(range)
{
    isRange = range.val();

    if (isRange == "true")
    {
	$('#col_nat_extport').css('display', 'none');
	$('#col_nat_extrange').css('display', '');
	$('#col_nat_dstport').css('display', 'none');
	$('#col_nat_dstrange').css('display', '');
    }
    else if (isRange == "false")
    {
	$('#col_nat_extport').css('display', '');
	$('#col_nat_extrange').css('display', 'none');
	$('#col_nat_dstport').css('display', '');
	$('#col_nat_dstrange').css('display', 'none');
    }
}

function completeAddIpDmz(ip, mac, name) {
    if(ip) {
        ip_ps = ip.split('.');
        $('#dmz_ip3').val(ip_ps[3]);
        $('#dmz_submit_btn').focus();
    }
}
function completeAddIpNat(ip, mac, name) {
    if(ip) {
        ip_ps = ip.split('.');
        $('#nat_dstip_p3').val(ip_ps[3]);
    }
}

function loaderNatMenu() {
        $('#nat_menu').css('display','block');
        if($('#show_form_nat_list:checked').length == 0) {
                $('#form_nat_list').css('display','none');
        }
        $('#show_form_nat_list').click( function() {
                $('#form_nat_list').css('display','block');
                $('#form_nat').css('display','none');
        } );
        $('#show_form_nat').click( function() {
                $('#form_nat_list').css('display','none');
                $('#form_nat').css('display','block');
        } );
}