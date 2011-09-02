#!/usr/bin/awk -f

BEGIN{
	FS="[<>]"
	RS="/service|service"
}

{
	if ($0~/firmware/)
	{
		i=1
		while (i < NF)
		{
			if($i~/^name/)
			{
				split($i,TIMESTAMP,"=");
				printf("FIRMWARE_VERSION=%s\n", TIMESTAMP[2])
			}
			if($i~/^protocole/)
			{
				i++
				printf("FIRMWARE_PROTO=\"%s\"\n", $i)
			}
			if($i~/^url/)
			{
				i++
				printf("FIRMWARE_URL=\"%s\"\n", $i)
			}
			if($i~/^file/)
			{
				i++
				printf("FIRMWARE_FILE=\"%s\"\n", $i)
			}
			i++
		}
	}
}
