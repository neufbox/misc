#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <ezxml.h>

#include <ewf/ewf_page.h>
#include <ewf/ewf_crypto.h>
#include <ewf/core/ewf_config.h>
#include <ewf/core/ewf_request.h>
#include <ewf/core/ewf_core.h>
#include <ewf/core/ewf_matrix.h>
#include <ewf/core/ewf_session.h>
#include <ewf/core/ewf_smith.h>
#include <ewf/core/ewf_theme.h>

#include <nbu/nbu.h>
#include <nbu/log.h>

#include <nbd/config.h>
#include <nbd/disk.h>

#include "nbl_utils.h"
#include "nbl_net.h"
#include "dbl.h"

#include "webapp.h"

#include "nbd.h"

#include <etk/net.h>

static int webapp_init(void)
{
        int ret;
        char buf[128];

        ret = nbd_connect();

        /*! \patch to change web_password to wpa hash
	  when his value doesn't contain 64 caracters
	  - 03/01/2008
	 */
	if ( nbd_user_config_get("web_password", buf, sizeof(buf) ) == NBD_SUCCESS
	     && strlen(buf) != 64
	     && nbd_status_get("wpa_key", buf, sizeof(buf) ) == NBD_SUCCESS)
	{
		char* hash = NULL;

		if(ewf_crypto_sha256_hash(buf, &hash) == EWF_SUCCESS)
		{
			if(nbd_user_config_set("web_password", hash) != NBD_SUCCESS)
			{
				nbu_log_error("Unable to set 'web_password' to '%s'", hash);
			}

			free(hash);
		}
		else
		{
			nbu_log_error("Unable to get sha256 hash of wpa_key = '%s'", buf);
		}
	}

        return ret;
}

static void webapp_clean(void)
{
	nbd_disconnect();
}

static int webapp_alt_login(ewf_session_t *session,
                            const char *method)
{
        NBU_UNUSED(session);

        if(nbu_string_matches(method, "button") == NBU_STRING_EQUAL)
        {
                if(nbd_status_matches("web_autologin", "on"))
                {
                        return EWF_SMITH_LOGIN_SUCCESS;
                }
                else
                {
                        return WEBAPP_SMITH_AUTOLOGIN_OFF;
                }
        }

        return EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED;
}

static void webapp_pre_dispatch(ewf_request_t *request)
{
        if(nbl_utils_box_upgrading() == 0)
        {
                ewf_hdf_set("old_uri", request->uri);

                request->uri = WEBAPP_URI_UPGRADING;
        }
}

static void webapp_pre_render(ewf_request_t *request, ewf_page_t *page)
{
	char net_data_access[16];
	char net_voip_access[16];
        char lang[8];
        char buf[32];
        char *nodename = NULL, *p = NULL;
        unsigned int i, j;
        const char *part_pgname[] = {
                "Page.Name.Parent",
                "Page.Name.Child",
        };

        if(request->type != ajax_request)
        {
		/* get lang and load menu */
		ewf_config_get("web_lang", lang, sizeof(lang));

                nbu_string_printf(buf, sizeof buf,
				  "lng/%s/menu.hdf",
				  lang);
		ewf_hdf_read_file(buf);

                /* register some extra variables */
                ewf_hdf_set_env("Page.Name", page->name);

                /* jump first '/' */
                j = (page->name[0] == '/' ? 1 : 0);
                nodename = page->name + j;
                p = nodename;

                for(i = 0;
                    i < NBU_ARRAY_SIZE(part_pgname) && *p != '\0';
                    i++)
                {
                        while(1)
                        {
                                p = page->name + j;
                                ++j;

                                if(*p == '/' || *p == '\0')
                                {
                                        nbu_string_printf(buf,
                                                          sizeof(buf),
                                                          "%.*s",
                                                          p - nodename,
                                                          nodename);

                                        ewf_hdf_set_env(part_pgname[i],
                                                        buf);

                                        nodename = p + 1;
                                        break;
                                }
                        }
                }

                status_to_hdf("net_data_ipaddr", "Sys.IPAddress");
                status_to_hdf("version_mainfirmware", "Sys.FirmwareVersion");
                status_to_hdf("mac_addr", "Sys.MACAddress");
                NV_TO_HDF("ipv6_enable");
                STATUS_TO_HDF("ipv6_lock");
                STATUS_TO_HDF("ipv6_status");

		nbd_status_get("net_data_access", net_data_access, sizeof(net_data_access));
		nbd_status_get("net_voip_access", net_voip_access, sizeof(net_voip_access));
		if (strcmp(net_data_access, net_voip_access) == 0) {
			snprintf(buf, sizeof(buf), "%s", net_data_access);
		} else {
			snprintf(buf, sizeof(buf), "%s%s", net_data_access,
					net_voip_access);
		}

		ewf_hdf_set("Sys.NetInfraExtra", buf);
        }
}

static int webapp_preload_theme_alt(const char *scheme,
                                    const char *hlpart,
                                    char *lthdir,
                                    size_t lthdir_size)
{
        char buf[64]; /* uuid len = 9 -> 36 */
        char mnt[256];
        unsigned int i = 0;

        /*
         * carry on 'ehd' (external hard drive) scheme
         */
        if(nbu_string_matches(scheme, "ehd") != NBU_STRING_EQUAL)
        {
                nbu_log_notice("Unknown scheme '%s' !", scheme);
                return EWF_THEME_LOAD_UNKNOWN_SCHEME;
        }

        /* We need to transpose uuid value to an absolute directory
         * where hard drive partition is mounted
         */
        while(hlpart[i] != '\0')
        {
                if(hlpart[i] == '/')
                {
                        nbu_string_printf(buf, sizeof(buf),
                                          "%.*s",
                                          i,
                                          hlpart);
                        break;
                }

                i++;
        }

        if(hlpart[i] != '/')
        {
                nbu_log_error("Invalid uri '%s://%s' (uuid no terminated by /)",
                              scheme, hlpart);
                return EWF_THEME_LOAD_INVALID_URI;
        }

        if(nbd_disk_get_mntpath_from_uuid(buf, mnt, sizeof(mnt)) != NBD_SUCCESS)
        {
                nbu_log_error("Unable to get mount point of uuid '%s'",
                              buf);
                return EWF_THEME_LOAD_INVALID_URI;
        }

        nbu_string_printf(lthdir, lthdir_size,
                          "%s/%s", mnt, hlpart + i + 1);

        return EWF_THEME_LOAD_SUCCESS;
}

static int webapp_getlist_theme_alt(nbu_list_t *th_repo_list)
{
	char *xml = NULL;
	size_t xml_size;
        ezxml_t xmln_root, xmln_uuid;

        char *uuid = NULL;
        char mnt[256];
        ewf_theme_repo_t* threpo = NULL;

        if(nbd_disk_get_mount_partition_uuid_list(&xml, &xml_size) != NBD_SUCCESS)
	{
                nbu_log_error("Failed to get uuid list !");
                return EWF_ERROR;
        }

        xmln_root = ezxml_parse_str(xml, xml_size);
        if(xmln_root != NULL)
        {
                for(xmln_uuid = ezxml_child(xmln_root, "uuid");
                    xmln_uuid != NULL;
                    xmln_uuid = ezxml_next(xmln_uuid))
                {
                        uuid = ezxml_txt(xmln_uuid);
                        if(uuid == NULL)
                        {
                                nbu_log_error("uuid = NULL !");
                                continue;
                        }

                        threpo = calloc(1, sizeof(ewf_theme_repo_t));
                        if(threpo == NULL)
                        {
                                nbu_log_error("threpo = NULL !");
                                continue;
                        }

                        if(nbd_disk_get_mntpath_from_uuid(uuid,
                                                          mnt,
                                                          sizeof(mnt))
                           != NBD_SUCCESS)
                        {
                                nbu_log_error("get mnt path from uuid failed");
				free(threpo);
                                continue;
                        }

                        nbu_string_printf(threpo->uri,
                                          sizeof(threpo->uri),
                                          "ehd://%s/themes/",
                                          uuid);

                        nbu_string_printf(threpo->pwd,
                                          sizeof(threpo->pwd),
                                          "%s/themes",
                                          mnt);

                        nbu_list_insert_end(th_repo_list,
                                            threpo);

                }

                ezxml_free(xmln_root);
        }

        free(xml);

        return EWF_SUCCESS;
}

static int webapp_config_set(const char *name, const char *value)
{
        return (nbd_user_config_set(name, value) < NBD_SUCCESS ? EWF_ERROR : EWF_SUCCESS);
}

int main(int argc, char **argv, char **envp)
{
        int ret;

        /* init matrix */
        ewf_matrix_ops.conf_get = nbd_user_config_get;
	ewf_matrix_ops.conf_set = webapp_config_set;
	ewf_matrix_ops.core_ext_init = webapp_init;
	ewf_matrix_ops.core_ext_cleanup = webapp_clean;
        ewf_matrix_ops.login_alt_method = webapp_alt_login;
        ewf_matrix_ops.pre_dispatch = webapp_pre_dispatch;
        ewf_matrix_ops.pre_render = webapp_pre_render;
        ewf_matrix_ops.preload_theme_alt_scheme = webapp_preload_theme_alt;
        ewf_matrix_ops.getlist_repo_theme_alt_scheme = webapp_getlist_theme_alt;

        ewf_matrix_vars.version = WEBUI_VERSION;

        /* init ewf */
        ewf_core_init();

        /* start ewf core */
	ret = ewf_core_start(argc, argv, envp);

        /* cleanup ewf */
        ewf_core_cleanup();

        return ret;
}
