#include "nbl.h"
#include <ewf/core/ewf_fcgi.h>

#include "nbl_exim.h"

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#include <tropicssl/aes.h>
#include <tropicssl/md5.h>

#include "nbd.h"

#include "nbu/nbu.h"
#include "nbu/string.h"

#define AES_BLOCK_SIZE 16
#define MD5_DIGEST_SIZE 16

int nbl_exim_config_user_import(FILE *config)
{
	FILE *temp;
	char buf[256];
	char filename[64];
	struct timeval tv;
	int n, ret = NBU_ERROR;

	if (config == NULL)
	{
		nbu_log_error("config == NULL !");
		return NBU_ERROR;
	}

	gettimeofday(&tv, NULL);
	nbu_string_printf(filename, sizeof filename,
                          "/tmp/neufbox4_%ld.conf", (long int) tv.tv_sec);

	temp = fopen(filename, "w");
	if (temp == NULL)
	{
		nbu_log_error("fopen failed %m !");
		return NBU_ERROR;
	}

	while ((n = fread(buf, 1, sizeof(buf), config)) > 0)
	{
		fwrite(buf, 1, n, temp);
	}

	fclose(temp);

	if (nbd_user_config_import(filename) == 0)
	{
		ret = NBU_SUCCESS;
	}
	else
	{
		nbu_log_error("nbd_user_config_import failed !");
	}

	unlink(filename);

	return ret;
}

int nbl_exim_config_user_export(void)
{
	int fd;
	char buf[256];
	char filename[64];
	struct timeval tv;
	int n, ret = NBU_ERROR;

	gettimeofday(&tv, NULL);
	nbu_string_printf(filename, sizeof filename, "/tmp/" UBOX "_%ld.conf", (long int) tv.tv_sec);

	if (nbd_user_config_export (filename) == NBD_SUCCESS)
	{
		fd = open(filename, O_RDONLY);

		if ( fd > 0 )
		{
			ewf_fcgi_printf("Content-Type: application/octet-stream\r\n");
			ewf_fcgi_printf("Content-Disposition: attachment; filename=\"" UBOX "_%ld.conf\"\r\n\r\n", (long int) tv.tv_sec);

			while ((n = read(fd, buf, sizeof(buf))) > 0)
			{
				ewf_fcgi_fwrite(buf, 1, n);
			}

			close(fd);
			ret = NBU_SUCCESS;
		}

		unlink(filename);
	}

	return ret;
}

int nbl_exim_diagnostic_report_export(void)
{
	int fd;
	struct timeval tv;
	int n, ret = NBU_ERROR;

	aes_context enc1, enc2;
	char mac[256];
	unsigned char buf[AES_BLOCK_SIZE + 1];
	unsigned char msg[AES_BLOCK_SIZE];
	unsigned char key1[] = "a7e821fa91253b274a1fda399db62ffec08600cc";
	unsigned char iv1[]  = "9d37b43294574aedbf7fcffd4caab790";
	unsigned char key2[40];
	unsigned char iv2[32];

	md5_context md5_ctx;
	unsigned char digest[MD5_DIGEST_SIZE];

	nbd_sys_dump ("/tmp/diag");

	fd = open("/tmp/diag", O_RDONLY);
	if (fd > 0)
	{
		ewf_fcgi_printf("Content-Type: application/octet-stream\r\n");

		gettimeofday(&tv, NULL);

		ewf_fcgi_printf("Content-Disposition: attachment; filename=\"rapport_diag_" UBOX "_%ld\"\r\n\r\n", (long int) tv.tv_sec);

		aes_setkey_enc(&enc1, key1, 128);

		if (nbd_status_get("mac_addr", mac, sizeof(mac)) == NBD_SUCCESS)
		{
			memcpy(buf, mac, 8);
			memcpy(buf + 8, mac + 9, 8);
		}

		/* compute iv2 */
		md5_starts(&md5_ctx);
		md5_update(&md5_ctx, buf, AES_BLOCK_SIZE);
		md5_finish(&md5_ctx, digest);
		/* AES_BLOCK_SIZE == MD5_DIGEST_SIZE == 16 */
		for (n = 0; n < AES_BLOCK_SIZE; n++)
		{
			iv2[2*n] = digest[n] >> 4;
			iv2[2*n+1] = digest[n] & 0xF;
		}

		/* compute key2 */
		md5_starts(&md5_ctx);
		md5_update(&md5_ctx, buf, 8);
		md5_finish(&md5_ctx, digest);
		/* AES_BLOCK_SIZE == MD5_DIGEST_SIZE == 16 */
		for (n = 0; n < 10; n++)
		{
			key2[2*n] = digest[n] >> 4;
			key2[2*n+1] = digest[n] & 0xF;
		}

		md5_starts(&md5_ctx);
		md5_update(&md5_ctx, buf + 8, 8);
		md5_finish(&md5_ctx, digest);
		/* AES_BLOCK_SIZE == MD5_DIGEST_SIZE == 16 */
		for (n = 10; n < 16; n++)
		{
			key2[2*n] = digest[n] >> 4;
			key2[2*n+1] = digest[n] & 0xF;
		}

		aes_setkey_enc(&enc2, key2, 128);
		aes_crypt_cbc(&enc1, AES_ENCRYPT, AES_BLOCK_SIZE, iv1, buf, msg);
		ewf_fcgi_fwrite(msg, 1, AES_BLOCK_SIZE);

		md5_starts(&md5_ctx);

		while ((n = read(fd, msg, AES_BLOCK_SIZE)) > 0)
		{
			memset(buf, '\0', sizeof buf);
			memcpy(buf, msg, n);
			aes_crypt_cbc(&enc2, AES_ENCRYPT, AES_BLOCK_SIZE, iv2, buf, msg);
			aes_crypt_cbc(&enc1, AES_ENCRYPT, AES_BLOCK_SIZE, iv1, msg, buf);
			ewf_fcgi_fwrite(buf, 1, AES_BLOCK_SIZE);
			md5_update(&md5_ctx, buf, AES_BLOCK_SIZE);
		}

		/* AES_BLOCK_SIZE == MD5_DIGEST_SIZE == 16 */
		md5_finish(&md5_ctx, digest);		

		aes_crypt_cbc(&enc2, AES_ENCRYPT, AES_BLOCK_SIZE, iv2, buf, msg);
		aes_crypt_cbc(&enc1, AES_ENCRYPT, AES_BLOCK_SIZE, iv1, msg, buf);
		ewf_fcgi_fwrite(buf, 1, AES_BLOCK_SIZE);

		close(fd);

		ret = NBU_SUCCESS;
	}
	else
	{
		nbu_log_error("Unable to open /tmp/diag ! %s", strerror(errno));
	}

	unlink("/tmp/diag");

	return ret;
}
