$(document).ready(function(){
  $('#mac_address_list tr:last').append('<td class="col_action">\
	  <button type="button" class="button_network" id="button_network"></button>\
	</td>');
  
  $('#button_network').click(function() {
    showLanPopup(completeAddAuthorizedMac, 'wlan0');
    return false;
  });
});

function completeAddAuthorizedMac(ip, mac, name) {
  if(mac) {
    var mac_splited = mac.split(':');
    $('#mac_address_p0').val(mac_splited[0]);
    $('#mac_address_p1').val(mac_splited[1]);
    $('#mac_address_p2').val(mac_splited[2]);
    $('#mac_address_p3').val(mac_splited[3]);
    $('#mac_address_p4').val(mac_splited[4]);
    $('#mac_address_p5').val(mac_splited[5]);
  }
}
