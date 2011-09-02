$(document).ready(function(){
  $('#filter_mode_on').click(function()
  {
    return showFilterSimple();
  });
  
  $('#filter_mode_off').click(function()
  {
    return hideFilterSimple();
  });
  
  $("input[name='filter_mode']:checked").click();
});
  
function showFilterSimple() {
  $('#filter_simple').css('display', '');
}

function hideFilterSimple() {
  $('#filter_simple').css('display', 'none');
}
