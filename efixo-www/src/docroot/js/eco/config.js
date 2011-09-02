$(document).ready(function(){
        if($('#mode_eco_default:checked').length == 1) {
                $('.custom').css('display','none');
        } else {
                $('.custom').css('display','');
        }
        $('#mode_eco_custom').click(function () {
                $('.custom').css('display','');
        });
        $('#mode_eco_default').click(function () {
                $('.custom').css('display','none');
        });
        $('.eco_link_lan').change(function(){
                if($(this).attr('value') == "off") {
                        $(this).siblings('input').attr('disabled','disabled');
                } else {
                        $(this).siblings('input').removeAttr('disabled');
                }
        });
});
