#!/usr/bin/awk -f

BEGIN {
	FS="[<>]"
	RS="/whitelist-smtp|whitelist-smtp"
	n=0
}

{
	if (n == 1) {
		i=1
		while (i < NF) {
			if ($i~/^url/) {
				i++
				 printf("%s\n", $i)
			}
			i++
		}
	}
	n++
}
