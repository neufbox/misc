$(document).ready(function(){
    $('#wlan_encryptiontype').change(changeEncryption);
    
    $('button[id$="_generator"]').click(function()
					{
					    var id = $(this).attr('id');
					    setGenerateKey($('#'+ id.substr(0, id.lastIndexOf('_'))));
					    return false;
					});
    
    $('#wlan_encryptiontype').change();
});

function showOpenItems()
{
    $('#row_wlan_keytype').css('display', 'none');
    $('#row_wlan_wepkey').css('display', 'none');
    $('#row_wlan_wpakey').css('display', 'none');
    $('#row_wlan_wpaenctype').css('display','none');
}

function showWEPItems()
{
    $('#row_wlan_keytype').css('display', '');
    $('#row_wlan_wepkey').css('display', '');
    $('#row_wlan_wpakey').css('display', 'none');
    $('#row_wlan_wpaenctype').css('display','none');
}

function showWPAItems()
{
    $('#row_wlan_keytype').css('display', 'none');
    $('#row_wlan_wepkey').css('display', 'none');
    $('#row_wlan_wpakey').css('display', '');
    $('#row_wlan_wpaenctype').css('display','');
    
}

function changeEncryption()
{
    var encVal = $('#wlan_encryptiontype').val();
    
    if (encVal == "OPEN")
    {
	showOpenItems();
    }
    else if (encVal == "WEP")
    {
	showWEPItems();
    }
    else if (encVal == "WPA-PSK" || encVal == "WPA2-PSK" || encVal == "WPA-WPA2-PSK")
    {
	showWPAItems();
        if(encVal == 'WPA2-PSK') {
               var select = $('#wlan_wpaenctype');
               select.children('option[value=tkip], option[value=tkipaes]').attr('disabled','disabled');
               select.children('option[value=aes]').attr('selected', 'selected');
        } else {
               $('#wlan_wpaenctype option').removeAttr('disabled');
        }
    }
}

function setGenerateKey(inputObj)
{
    var encType = $('#wlan_encryptiontype').val();
    var key = '';

    if(encType == "WEP") {
        /* 104 bits WEP key */
	key = generateKey(104, $('#wlan_keytype').val());
    } else if(encType == "WPA-PSK" || encType == "WPA2-PSK" || encType == "WPA-WPA2-PSK") {
	/* 504 bits is the max lenght of WPA key, just use 160 bits */
	key = generateKey(160, 'alphanum');
    }

    inputObj.val(key);
}

var generateKey = function(){
    var alphanum_characters =  new Array('-','_','0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z');
    var key_characters = new Array(' ','!','#','$','%','&','(',')','*','+',',','.','/',':',';','<','=','>','?','@','[','\\',']','^','{','|','}','~').concat(alphanum_characters);

    return function(klen, ktype){
	/* generate "klen" bits length key */
    	var asciikey = '';
    	var key = '';
    	klen = klen / 8; /* convert to bytes */
				
	switch (ktype) {
	case "hexa":
	    for (var i = 0; i < klen; i++){
		asciikey = key_characters[Math.floor(Math.random() * key_characters.length)]; 
		key += asciikey.charCodeAt(0).toString(16);
	    }
	    break;
	case "alphanum":
	    for (var i = 0; i < klen; i++){
		key += alphanum_characters[Math.floor(Math.random() * alphanum_characters.length)]; 
	    }
	    break;
	default: /* ascii */
	    for (var i = 0; i < klen; i++){
		key += key_characters[Math.floor(Math.random() * key_characters.length)]; 
	    }
	}
    	return key;
    }
}();
