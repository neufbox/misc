#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os, sys, glob, re

lngdircollect = {}
rec_symbol = re.compile("^([^ =]+) *= ?(.*)$")

######################################

def collectSymbol(rootdirname, lng, relativehdf):
    global lngdircollect, rec_symbol
    
    f = open("%s/%s/%s" % (rootdirname, lng, relativehdf), "r")

    for line in f.readlines():
        m = rec_symbol.match(line)
        
        if m:
            if lngdircollect[lng][relativehdf].has_key(m.group(1)):
                print " * Warning: in %s/%s/%s file, %s is duplicated !" % (rootdirname, lng, relativehdf, m.group(1))
                continue
            
            lngdircollect[lng][relativehdf][m.group(1)] = m.group(2)
        
    f.close()

def diffSymbol():
    global lngdircollect
    
    if len(lngdircollect.keys()) == 0:
        print "Not found lng directories"
        return
    
    lngref = lngdircollect.keys()[0]
    lngdiffs = lngdircollect.keys()[1:]
    
    for hdf in lngdircollect[lngref].keys():
        for symbol in lngdircollect[lngref][hdf].keys():
            
            for lngdiff in lngdiffs:
                if not lngdircollect[lngdiff].has_key(hdf):
                    print "- FILE MISSING !! (%s/%s)" % (lngdiff, hdf)
                    continue
                
                if not lngdircollect[lngdiff][hdf].has_key(symbol):
                    print "- %s (%s/%s)" % (symbol, lngdiff, hdf)
                else:
#                    if lngdircollect[lngref][hdf][symbol] == lngdircollect[lngdiff][hdf][symbol]:
#                        print " * Warning: %s/%s: %s and %s/%s: %s have same value (%s)" % (lngref, hdf, symbol, lngdiff, hdf, symbol, lngdircollect[lngref][hdf][symbol])
                        
                    # remove symbol from list
                    lngdircollect[lngdiff][hdf].pop(symbol)

            lngdircollect[lngref][hdf].pop(symbol)
            
    # reste
    for lng in lngdircollect.keys():
        for hdf in lngdircollect[lng].keys():
            for symbol in lngdircollect[lng][hdf].keys():
                print "+ %s (%s/%s)" % (symbol, lng, hdf)

def filePickup(rootdirname, lng, relativehdfdir):
    global lngdircollect
    
    flist = glob.glob('%s/%s/%s/*' % (rootdirname, lng, relativehdfdir))

    for filename in flist:
        relativefilename = filename.replace("%s/%s/" % (rootdirname, lng), "")
        
        if os.path.isdir(filename):
            filePickup(rootdirname, lng, relativefilename)
        else:
            lngdircollect[lng][relativefilename] = {}
                
            collectSymbol(rootdirname, lng, relativefilename)
                
# main
if len(sys.argv) != 2 :
    print "Usage: %s <lang dir>" % (sys.argv[0]);
    sys.exit(1)

rootdirname = os.path.dirname(sys.argv[1])

lngdirlist = glob.glob('%s/*' % (rootdirname,))
for lngdir in lngdirlist :
    if os.path.isdir(lngdir) :
        lng = os.path.basename(lngdir)
        
        print " -> adding", lng, "language in check"
    
        lngdircollect[lng] = {}
    
print "--------------------------------------"

for lng in lngdircollect.keys():
    print " -> parsing language %s" % (lng)
    filePickup(rootdirname, lng, ".")

print "-- DIFF ------------------------------"

diffSymbol()
