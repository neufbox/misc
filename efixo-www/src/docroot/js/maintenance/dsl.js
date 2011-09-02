$(document).ready(function(){
  $('#adsl_profile_manual').click(function()
  {
    return showManualProfiles();
  });

  $('#adsl_profile_auto').click(function()
  {
    return hideManualProfiles();
  });

  $('#adsl_profile_rescue').click(function()
  {
    return hideManualProfiles();
  });
  
  $("input[name='adsl_profile']:checked").click();
});

function showManualProfiles()
{
  $('#adsl_configuration').css('display', '');
}

function hideManualProfiles()
{
  $('#adsl_configuration').css('display', 'none');
}
