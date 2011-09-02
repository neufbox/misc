#!/bin/sh

revdate=$(date +%Y%m%d)
revdatefr=$(date +"%d %b %Y")
revnumber="${revdate}"
docname="apirest-${revdate}"

opt="-a revnumber=${revnumber} -a toc -a toclevels=4 -a icons -n"

###

echo " >> generate html doc"
asciidoc -b xhtml11 ${opt} -a revdate="${revdatefr}" -o ${docname}.html apirest.doc
ln -s ${docname}.html apirest-latest.html

echo " >> generate pdf doc"
asciidoc -b docbook ${opt} -a revdate="${revdatefr}" -o ${docname}.xml apirest.doc
dblatex -t pdf ${docname}.xml
ln -s ${docname}.xml apirest-latest.xml
ln -s ${docname}.pdf apirest-latest.pdf
