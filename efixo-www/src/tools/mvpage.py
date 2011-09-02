#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os, sys, glob, re, commands

tpl_dir = "fcgiroot/tpl"
lng_fr_dir = "fcgiroot/lng/fr"
lng_en_dir = "fcgiroot/lng/en"
rgx_dir = "fcgiroot/rgx"

css_dir = "docroot/css"
js_dir = "docroot/js"

src_dir = "src/wui"

txt_dir_maps = [{"dir": tpl_dir, "ext": 'cst'}, 
                {"dir": lng_fr_dir, "ext": 'hdf'},
                {"dir": lng_en_dir, "ext": 'hdf'},
                {"dir": rgx_dir, "ext": 'hdf'},
                {"dir": css_dir, "ext": 'css'},
                {"dir": js_dir, "ext": 'js'}]
c_dir_maps = [{"dir": src_dir, "ext": 'c'}]

def mvpage(old, new):
    for txt_m in txt_dir_maps:
        if not os.path.exists("%s/%s.%s" % (txt_m["dir"],
                                            old,
                                            txt_m["ext"])):
            continue
        
        ret = commands.getstatusoutput("svn mkdir %s/%s" % (txt_m["dir"],
                                                            os.path.dirname(new)));
        
        ret = commands.getstatusoutput("svn mv %s/%s.%s %s/%s.%s" % (txt_m["dir"],
                                                                     old,
                                                                     txt_m["ext"],
                                                                     txt_m["dir"],
                                                                     new,
                                                                     txt_m["ext"]))
        if ret[0] != 0:
            print >> sys.stderr, "Error while move %s/%s.%s to %s/%s.%s" % (txt_m["dir"],
                                                                           old,
                                                                           txt_m["ext"],
                                                                           txt_m["dir"],
                                                                           new,
                                                                           txt_m["ext"])

    new_under = new[1:].replace('/','_')
    old_under = old[1:].replace('/','_')
    for c_m in c_dir_maps:
        if not os.path.exists("%s/%s.%s" % (c_m["dir"],
                                            old_under,
                                            c_m["ext"])):
            continue
        
        ret = commands.getstatusoutput("svn mv %s/%s.%s %s/%s.%s" % (c_m["dir"],
                                                                     old_under,
                                                                     c_m["ext"],
                                                                     c_m["dir"],
                                                                     new_under,
                                                                     c_m["ext"]))
        
        if ret[0] != 0:
            print >> sys.stderr, "Error while move %s/%s.%s to %s/%s.%s" % (c_m["dir"],
                                                                           old,
                                                                           c_m["ext"],
                                                                           c_m["dir"],
                                                                           new,
                                                                           c_m["ext"])
        
    print "OK"

def sedto(old, new):
    old_backslashed = old.replace('/', '\\/')
    new_backslashed = new.replace('/', '\\/')
    
    regex = "s/('|\")%s('|\")/\\1%s\\2/g" % (old_backslashed,
                                           new_backslashed)
    
    ret = commands.getstatusoutput("perl -pi -e %s ./" % (regex,))
    if ret[0] != 0:
        print >> sys.stderr, "Failed to regexp substitute %s" % (regex,)

# main
if len(sys.argv) != 3 :
    print "Usage: %s /<current page url> /<new page url>" % (sys.argv[0]);
    sys.exit(1)

cpage_url = sys.argv[1]
npage_url = sys.argv[2]

mvpage(cpage_url, npage_url)
sedto(cpage_url, npage_url)
