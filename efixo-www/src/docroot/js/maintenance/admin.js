$(document).ready(function(){
  $('#web_auth').change(function()
  {
    $("#web_auth option:selected").each(function()
    {
      var value = $(this).val();
      if(value == "off" || value == "button")
      {
        hidePassProtectForm();
      }
      else if(value == "all" || value == "passwd")
      {
        showPassProtectForm();
      }
    });
  });
  
  $("#web_auth").change();
});

function showPassProtectForm()
{
  $('#loginconfig_login').css('display', '');
  $('#loginconfig_password').css('display', '');
  $('#loginconfig_password_verif').css('display', '');
}

function hidePassProtectForm()
{
  $('#loginconfig_login').css('display', 'none');
  $('#loginconfig_password').css('display', 'none');
  $('#loginconfig_password_verif').css('display', 'none');
}
