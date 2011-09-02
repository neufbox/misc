$(document).ready(function(){
  $('#dnshosts_config .col_action').css('display', '');

  $('#dnshosts_ip_p0').focus();

  $('input[id^=dnshosts_ip_]').keypress(function(event)
  {
    var key;
    if($.browser.mozilla) {
      key = event.which;
    } else {
      key = event.keyCode;
    }
    
    if(key == 190 || key == 46) {
      $(this).next().focus().select();
      return false;
    } else if(key == 8) {
      if($(this).val() == '') {
        $(this).prev().focus();
        return false;
      }
    }

    return true;
  });
  
  $('#dnshosts_config tr:last').append('<td class="col_action">\
	  <button type="button" class="button_network" id="button_network"></button>\
	</td>');
  
  $('#button_network').click(function() {
    showLanPopup(completeAddIp);
    return false;
  });
   
});

function completeAddIp(ip, mac, name) {
  if(ip) {
    ip_ps = ip.split('.');
    $('#dnshosts_ip_p0').val(ip_ps[0]);
    $('#dnshosts_ip_p1').val(ip_ps[1]);
    $('#dnshosts_ip_p2').val(ip_ps[2]);
    $('#dnshosts_ip_p3').val(ip_ps[3]);

    $('#dnshosts_hostname').focus();
  }
}
