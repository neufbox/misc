#!/usr/bin/env python

import os, sys

exclist=["/footer",
         "/menu",
         "/lanpopup",
         "/test",
         "/header",
         "/footer"]

urllist=[]

def collect_tpl(root_tpldir, parent = "."):
    global exclist

    dirpath = "%s/%s/" % (root_tpldir, parent)
    
    for file in os.listdir(dirpath):
        if file[0] == '.':
            continue

        fullpwd = "%s/%s" % (dirpath, file)
        file_noext, ext = os.path.splitext(file)

        if os.path.isdir(fullpwd):
            newparent = "%s/%s" % (parent, file)
            collect_tpl(root_tpldir, newparent)

        if ext != ".html":
            continue

        url = "%s/%s" % (parent[1:], file_noext)

        if url in exclist:
            continue

        urllist.append(url)

if len(sys.argv) != 2:
    print "Usage: %s <tpl dir>" % (sys.argv[0])
    sys.exit(1)

tpldir = sys.argv[1]

collect_tpl(tpldir)

urllist.sort()

for url in urllist:
    print "%s" % (url,)

