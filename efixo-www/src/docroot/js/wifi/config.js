$(document).ready(function(){
  $('#ap_active_on').click(function()
  {
    return showAPGeneralConfItems();
  });

  $('#ap_active_off').click(function()
  {
    return hideAPGeneralConfItems();
  });

#ifndef NB6
  $('#access_point_config tr:gt(0)').hide();
#endif

#ifndef NB4
  $('input[name="ap_mode"]').click(function()
  {
      updateAccessPointOption();
  });
#endif
  $("input[name='ap_active']:checked").click();
});

function hideAPGeneralConfItems()
{
  $('#row_ap_ssid').css('display', 'none');
  $('#row_ap_closed').css('display', 'none');
  $('#row_ap_channel').css('display', 'none');
  $('#row_ap_mode').css('display', 'none');
#ifndef NB4
  $('#row_access_point_options').css('display', 'none');
#endif
}

function showAPGeneralConfItems()
{
  $('#row_ap_ssid').css('display', '');
  $('#row_ap_closed').css('display', '');
  $('#row_ap_channel').css('display', '');
  $('#row_ap_mode').css('display', '');
#ifndef NB4
  updateAccessPointOption();
#endif
}

#ifndef NB4
function updateAccessPointOption()
{
#ifdef NB6
    var ap_active = $('input[name="ap_active"]').val();
#else
    var ap_active = $('input[name="ap_active"]:checked').val();
#endif

    if(ap_active == "on"
       && $('input[name="ap_mode"]:checked').val() == "11n")
    {
	$('#row_access_point_options').css('display', '');
    }
    else
    {
	$('#row_access_point_options').css('display', 'none');
    }
}
#endif
