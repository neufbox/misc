$(document).ready(function(){
    $('#form_auth_passwd').submit(function() {
	sendRequest('/login', getChallenge, {action: 'challenge'});
	
	return false;
    });
  
    $('#login').focus();

    $('#login').attr('autocomplete', 'off');
    $('#password').attr('autocomplete', 'off');

#ifndef CIBOX
    if($('#button_continue').val() != "on")
    {
        $('#div_button_continue').attr('class', 'button_submit_disabled');
    }
    setInterval("sendRequest('/login', updateContinueButton)", 1200);
#endif /* #ifndef CIBOX */
});

function getChallenge(response) {
    if(existResponseAttrElement(response, 'error', 'txt'))
    {
        var error = getResponseAttrElement(response, 'error', 'txt');

        $("div#main").prepend("<div class=\"error_notice quick_error\" >" + error + "</div>");
        $("div.quick_error").delay(5000).slideUp("slow", function() { $(this).remove(); });

        return;
    }
    
    var challenge = getResponseElement(response, 'challenge');

    if(challenge != null && challenge.length == 40)
    {
	var form = $('#form_auth_passwd');
	var input_login = $('#login');
	var input_password = $('#password');
	var input_zsid = $('#zsid');
	var input_hash = $('#hash');
	
        input_zsid.val(challenge);
	input_hash.val(HMAC_SHA256_MAC(challenge, SHA256_hash(input_login.val())) + HMAC_SHA256_MAC(challenge, SHA256_hash(input_password.val())));
	input_login.val("");
	input_password.val("");
        
        /* guedin traf */
        form.unbind('submit');

	/* continue */
	form.submit();
    }
}

#ifndef CIBOX
function updateContinueButton(response)
{
    var autologin = getResponseElement(response, 'autologin');
    var x = $('#div_button_continue');
    
    if(autologin == "on")
    {
	x.attr('class', 'button_submit');
    }
    else
    {
	x.attr('class', 'button_submit_disabled');
    }
}
#endif /* #ifndef CIBOX */
