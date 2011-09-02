$(document).ready(function(){
  $('#net_mode_router').click(function() {
    return showPPPItems();
  });
  
  $('#net_mode_bridge').click(function() {
    return hidePPPItems();
  });
});

function hidePPPItems()
{
  $('#row_ppp_login').css('display', 'none');
  $('#row_ppp_password').css('display', 'none');
}

function showPPPItems()
{
  $('#row_ppp_login').css('display', '');
  $('#row_ppp_password').css('display', '');
}
