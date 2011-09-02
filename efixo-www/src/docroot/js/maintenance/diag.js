$(document).ready(function() {
        #ifndef NB5
        $('#form_dsltest').submit(function() {
                startDsltest();
                return false;
        });
        #endif

        $('#form_voip_diag').submit(function() {
                sendRequest('/maintenance/diag',
		callBackVoipDiag,
		{
                        'action': $('#form_voip_diag input[name=action]').val(),
                        'step': $('#form_voip_diag input[name=step]').val()
                });

                $('#form_voip_diag input[name=step]').val('');
                $('#form_voip_diag .button_submit').hide();
                $('#loader').show();
        
                return false;
        });
});

/********** VOIP DIAG **************/
interHandler = null;
function callBackVoipDiag(xml)
{
        if(interHandler != null)  clearInterval(interHandler);
        interHandler = setInterval(function() {
                sendRequest(
                '/maintenance/diag',
                statusVoipDiag, {'action': 'vSStest_info'});
        },2000);
}
function setStatus(elm){
        var attrClass = 'unused';
        var attrText = '';

        if(elm.attr('val') == "success") {
                attrClass = 'enabled';
        } else if(elm.attr('val') == "error") {
                attrClass = 'disabled';
        }
        attrText = $.trim(elm.attr('text'));
        
        return { attrClass: attrClass, attrText: attrText};
}
function statusVoipDiag(xml)
{
        var status = [];
        status[0] = setStatus( $(xml).find('init')      );
        status[1] = setStatus( $(xml).find('gr909')     );
        status[2] = setStatus( $(xml).find('ring')      );
        status[3] = setStatus( $(xml).find('hookntone') );
        status[4] = setStatus( $(xml).find('dtmf')      );

        $.each(status, function(n, i){
                $('#voip_diag td:eq('+n+')').attr('class', i.attrClass);
                $('#voip_diag td:eq('+n+')').text(i.attrText);
        });

        $('#form_voip_diag input[name=step]').val($(xml).find('button').attr('nextstep'));
        $('#form_voip_diag button[name=submit]').text($(xml).find('button').text());

        //nextstep
        if($('#form_voip_diag input[name=step]').val() != '') {
                clearInterval(interHandler);
                $('#form_voip_diag #loader').hide();
                $('#form_voip_diag .button_submit').show();
                $('#form_voip_diag #info').text($(xml).find('text').text());
                 
        }

        //reboot
        $(xml).find('reboot').each(function(){
                clearInterval(interHandler);
                $('#loader, #info').hide();
                $('#form_voip_diag_reboot button[name=submit]').text($(this).attr('text'));
        });
}



#ifndef NB5
/********** DSL TEST **************/
function startDsltest()
{
        $('#button_dsltest').attr('disabled', true);
        $('#div_button_dsltest').attr('class', 'button_submit_disabled');
        $('#dsltest_status').css('display', '');
    
        /* send first ajax request */
        sendRequest('/maintenance/diag',
        ajaxCbDsltest,
        {'action' : 'dsltest'});
}

function stopDsltest()
{
        $('#button_dsltest').attr('disabled', false);
        $('#div_button_dsltest').attr('class', 'button_submit');
        $('#dsltest_status').css('display', 'none');
}

function ajaxCbDsltest(response)
{
        stopDsltest();

        var result_val = getResponseAttrElement(response, 'result', 'val');
        var result_text = getResponseAttrElement(response, 'result', 'text');

        if(result_val == "OK")
        {
                $('#td_dsltest_result').attr('class', 'enabled');
        }
        else
        {
                $('#td_dsltest_result').attr('class', 'disabled');
        }
    
        $('#td_dsltest_result').text(result_text);
}
#endif
