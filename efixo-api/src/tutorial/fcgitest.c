#include <ClearSilver.h>
#include <fcgi_stdio.h>

/* $ mips-linux-gcc -I/home/rhk/workspace/dev/nb4/staging_dir/mips/include/ -I/home/rhk/workspace/dev/nb4/staging_dir/mips/include/clearsilver/ -L/home/rhk/workspace/dev/nb4/staging_dir/mips/lib/ fcgitest.c -lfcgi -lneo_cgi -lneo_cs -lneo_utl -lm -o fcgitest */

int fcgi_read_cb(void *ptr, char *data, int size)
{
    return FCGI_fread(data, sizeof(char), size, FCGI_stdin);
}

int fcgi_writef_cb(void *ptr, const char *format, va_list ap)
{
    return FCGI_vprintf(format, ap);
}

int fcgi_write_cb(void *ptr, const char *data, int size)
{
    return FCGI_fwrite((void *) data, sizeof(char), size, FCGI_stdout);
}


int main(int argc, char **argv, char **envp)
{
	CGI *cgi;
	NEOERR *err;

	while (FCGI_Accept() >= 0)
	{
		err = cgi_init(&cgi, NULL);
		nerr_ignore(&err);

	    cgiwrap_init_emu(NULL, &fcgi_read_cb, &fcgi_writef_cb, &fcgi_write_cb, NULL, NULL, NULL);

		err = cgi_display(cgi, "fcgitest.cst");
		nerr_ignore(&err);

		cgi_destroy(&cgi);
	}

	return 0;
}

