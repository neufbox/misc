$(document).ready(function(){
    $("div.blocklist p.selectable").click(function() {
        $("div.blocklist p.selectable").attr("class", "");        
        $(this).attr("class", "selected");
        $("input[name='theme_selected']").val($(this).children('input').val());
    });
});
