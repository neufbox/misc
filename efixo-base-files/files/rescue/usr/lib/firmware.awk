#!/usr/bin/awk -f

BEGIN{
	FS="[<>]"
}

{
	if ($2~/^proto/) {
		printf("PROTO=\"%s\"\n", $3)
	}
	if ($2~/^url/) {
		printf("URL=\"%s\"\n", $3)
	}

	if ($2~/^main/) {
		split($2,HASH,"=");
		printf("%s_HASH=%s\n", "MAIN", HASH[2])
		printf("%s_FILE=\"%s\"\n", "MAIN", $3)
	}
	if ($2~/^config/) {
		split($2,HASH,"=");
		printf("%s_HASH=%s\n", "CONFIG", HASH[2])
		printf("%s_FILE=\"%s\"\n", "CONFIG", $3)
	}
	if ($2~/^dsldriver/) {
		split($2,HASH,"=");
		printf("%s_HASH=%s\n", "ADSL", HASH[2])
		printf("%s_FILE=\"%s\"\n", "ADSL", $3)
	}
}
