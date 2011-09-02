function updateStatus(response)
{
        var a_var = new Array("internet", "voip", "tv");
        var n;
    
        var status_text;
        var status_val;
        var access_text;
        var access_val;
        var x;

        var mtag;
    
        for(n = 0; n < a_var.length; n++)
        {
                mtag = response.getElementsByTagName(a_var[n]);
                if(mtag.length > 0)
                {
                        status_text = getResponseAttrElement(mtag[0], "link", 'text');
                        status_val = getResponseAttrElement(mtag[0], "link", 'val');
            
                        access_text = getResponseAttrElement(mtag[0], "access", 'text');
                        access_val = getResponseAttrElement(mtag[0], "access", 'val');

                        x = $('#' + a_var[n] + '_status');
	
                        if(status_val == "up")
                        {
                                x.attr('class', 'enabled');
                        }
                        else
                        {
                                x.attr('class', 'disabled');
                        }
	    
                        if(access_val == "gprs")
                        {
                                status_text = status_text + " (" + access_text + ")";
                        }

                        x.text(status_text);
                }
        }
}

(function($){
    $.fn.konami             = function( fn, params ) {
        params              = $.extend( {}, $.fn.konami.params, params );
        this.each(function(){
            var tgt         = $(this);
            tgt.bind( 'konami', fn )
               .bind( 'keyup', function(event) { $.fn.konami.checkCode( event, params, tgt ); } );
        });
        return this;
    };

    $.fn.konami.params      = {
        'code'      : [38, 38, 40, 40, 37, 39, 37, 39, 66, 65],
        'step'      : 0
    };

    $.fn.konami.checkCode   = function( event, params, tgt ) {
        if(event.keyCode == params.code[params.step]) {
            params.step++;
        } else {
            params.step     = 0;
        }

        if(params.step == params.code.length) {
            tgt.trigger('konami');
            params.step     = 0;
        }
    };
})(jQuery);


var firstname = new Array('Vincent', 'Pierre-Lucas', 'Miguel', 'Anthony',
'Raphaël', 'Arnaud', 'Tanguy', 'Severin', 'Evangelina','François','Olivier',
'Guillaume','Frédéric', 'Sébastien', 'Gilles', 'Rachid', 'Christophe',
'Sothear', 'Les ninjas', 'Salomon', 'Francis', 'Nicolas', 'Pierre',
'Antoine', 'Gaël', 'Rudy', 'Cédric', 'Julien');
var dx, xp, yp, am, stx, sty ;
var i, doc_width = 800, doc_height = 600;
function snow() {
        for (i = 0; i < firstname.length; ++ i) {
                yp[i] += sty[i];
                if (yp[i] >= doc_height-53) {
                        xp[i] = Math.random()*(doc_width-am[i]-53);
                        yp[i] = 0;
                        stx[i] = 0.02 + Math.random()/10;
                        sty[i] = 0.7 + Math.random();
                        doc_width = $(window).width();
                        doc_height = $(window).height();
                }
                dx[i] += stx[i];
                $('#credit-'+i).css('top', yp[i]);
                $('#credit-'+i).css('left', xp[i] + am[i]*Math.sin(dx[i]));
        }
        setTimeout('snow()',10);
}


function credits() {

        $('html, body').css('overflow','hidden');
        doc_width = $(window).width();
        doc_height = $(window).height();

        dx = new Array();
        xp = new Array();
        yp = new Array();
        am = new Array();
        stx = new Array();
        sty = new Array();

        for (i = 0; i < firstname.length; ++ i) {
                dx[i] = 0;
                xp[i] = Math.random()*(doc_width-(53));
                yp[i] = Math.random()*doc_height;
                am[i] = Math.random()*20;
                stx[i] = 0.02 + Math.random()/10;
                sty[i] = 0.7 + Math.random();

                $("body").append('<div id="credit-'+ i +'" class="credits"><small>'+firstname[i]+'</small></div>');
                $('#credit-'+i).css('z-index',i+10);
        }
        snow();
}

$(document).ready(function(){
        setInterval("sendRequest('/index', updateStatus)", 860);
        $(this).konami(function(){
            credits();
        });
});
