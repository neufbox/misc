$(document).ready(function(){
    $('input.ip').keypress(function(event)
			   {
			       var key;
			       if($.browser.mozilla) 
			       {
				   key = event.which;
			       } 
			       else
			       {
				   key = event.keyCode;
			       }
			       
			       if(key == 190 || key == 46) 
			       {
				   $(this).next().focus().select();
				   return false;
			       } 
			       else if(key == 8) 
			       {
				   if($(this).val() == '') 
				   {
				       $(this).prev().focus();
				       return false;
				   }
			       }

			       return true;
			   });
});
