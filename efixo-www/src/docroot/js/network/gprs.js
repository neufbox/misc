$(document).ready(function(){
    setInterval("sendRequest('/network/gprs', updateStatus)", 1500);
});

function updateStatus(response)
{
    var active_val = getResponseAttrElement(response, "active", "val");
    var active_text = getResponseAttrElement(response, "active", "text");
    
    if(active_val == "on")
    {
        $("#active_data").attr("class", "enabled");
    }
    else
    {
        $("#active_data").attr("class", "disabled");
    }
    $("#active_data").text(active_text);

    var sim = response.getElementsByTagName("sim");
    if(sim.length > 0)
    {
        var status_val = getResponseAttrElement(sim[0], "status", "val");
        var status_text = getResponseAttrElement(sim[0], "status", "text");
        
        var pin_status_val = getResponseAttrElement(sim[0], "pinstatus", "val");

        var pin_remaining_val = getResponseAttrElement(sim[0], "pinremaining", "val");
        
        var puk_remaining_val = getResponseAttrElement(sim[0], "pukremaining", "val");

        /* error in pin or puk code */
        if(puk_remaining_val == 0)
        {
            var puk_remaining_text = getResponseAttrElement(sim[0], "pukremaining", "text");
            
            $("#error_simlock").text(puk_remaining_text);
            $("#error_simlock").css("display", "");
            
            $("#div_unlocksim").css("display", "none");
            $("#div_config").css("display", "none");
        }
        else if(pin_remaining_val == 0)
        {
            var pin_remaining_text = getResponseAttrElement(sim[0], "pinremaining", "text");

            $("#error_simlock").text(pin_remaining_text);
            $("#error_simlock").css("display", "");

            $("#div_unlocksim").css("display", "");
            
            $("#div_config").css("display", "none");
        }
        else
        {
            $("#error_simlock").css("display", "none");
            $("#div_unlocksim").css("display", "none");
            
            $("#div_config").css("display", "");
        }

        if(pin_status_val == "ko")
        {
            var pin_status_text = getResponseAttrElement(sim[0], "pinstatus", "text");
            var msg = "";

            msg += $("#active_data").text() + " ( " + pin_status_text;
            
            if(pin_remaining_val > 0 && pin_remaining_val <= 3)
            {
                var pin_remaining_text = getResponseAttrElement(sim[0], "pinremaining", "text");
                
                msg += " - " + pin_remaining_text;
            }

            msg += " )";
            
            $("#active_data").text(msg);
        }
        else if(status_val == "ko")
        {
            $("#active_data").text($("#active_data").text() + " ( " + status_text +" )");
        }
    }
    
    var device = response.getElementsByTagName("device");
    if(device.length > 0)
    {
        var dev_status_val = getResponseAttrElement(device[0], "status", "val");
        var dev_status_text = getResponseAttrElement(device[0], "status", "text");
        
        if(dev_status_val == "available")
        {
            $("#row_active_data").css("display", "");
            $("#device_state").attr("class", "enabled");
        }
        else
        {
            $("#row_active_data").css("display", "none");
            $("#device_state").attr("class", "disabled");
        }

        $("#device_state").text(dev_status_text);
    }
}
