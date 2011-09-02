#include "ewf/ewf.h"

#include "ewf/core/ewf_matrix.h"
#include "ewf/core/ewf_theme.h"
#include "ewf/core/ewf_config.h"
#include "ewf/ewf_utils.h"

#include "nbu/nbu.h"
#include "nbu/string.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <ezxml.h>

#define THEME_DEFAULT_URI "local://default"
#define THEME_REGEX_UUID "^[0-9A-Za-z-]{9,36}$"
#define THEME_REGEX_VERSION "^[0-9]+\\.[0-9]+$"
#define THEME_REGEX_TARGET "^[0-9]+\\.[0-9]+ ?, ?[0-9]+\\.[0-9]+$"

/**************************************************************/
/*                STATIC FUNCTION PROTOTYPES                  */
/**************************************************************/

static int ewf_theme_parse_threpo(const ewf_theme_repo_t *th_repo,
				  nbu_list_t *p_th_ipkg_list);
static int ewf_theme_parse_thdir(const char *th_repo_pwd, 
				 const char *th_dirname,
				 ewf_theme_t *theme);
static int ewf_theme_install_preview(const char *repopwd,
                                     const char *thname,
                                     const char *thuuid,
                                     const char *thversion);
static int ewf_theme_check_compatibility(const ewf_theme_t *theme);
static int ewf_theme_install(const char *pwd);
static int ewf_theme_load_by_uri(const char *uri);
static int ewf_theme_symlink_to_vdocroot(const char *themepwd,
					 const char *filename);

/**************************************************************/
/*                PUBLIC FUNCTION DEFINITIONS                 */
/**************************************************************/

/*
 * load the current selected theme based on these config variables:
 *       -> web_theme_uri
 * NB: if there an error on loading selected theme, we load the default theme 
 * defined by macro THEME_DEFAULT_URI
 *
 *   Example of uri:
 *  ----------------
 *       -   local://{theme name}
 *       -   file://{fullpwd}/{theme_name}
 *       -   usbkey://{part_uuid}/{theme_name}
 *       -   http://myfoo.com/{theme_name}
 *
 *   -> Only the 'local' scheme is supported by default in libewf core.
 *   -> Good news, you can add your alternative scheme !
 *
 * 1) get 'web_theme_uri' config value. Extract:
 *      
 *           {scheme} :// { hierarchical part }
 *
 *   -> if error                        => return EWF_THEME_LOAD_INVALID_URI
 *   -> if (scheme == 'local')          => used the local directory themes/
 *                                         in docroot/ and go to (3)
 *   -> otherwise                       => go to (2)
 *
 * 2) get 'preload_theme_alt_scheme' pointer function from ewf_matrix_ops
 *   -> if NULL                         => return EWF_THEME_LOAD_UNKNOWN_SCHEME
 *   -> if != NULL, call it with 'scheme' 
 *      and 'hierarchical part' in parameters. The function must give 
 *      local absolute directory name where the theme is.
 *     -> if return EWF_THEME_LOAD_SUCCESS 
 *                                      => go to (3)
 *     -> otherwise                     => return the func answer
 *     
 * 3) With the local absolute directory name, install the theme in htdocs/
 *    creating symbolic links
 *   -> if GOOD                         => return EWF_THEME_LOAD_SUCCESS
 *   -> if not GOOD                     => return EWF_THEME_LOAD_INSTALL_FAILED     
 */
int ewf_theme_load(void)
{
	char uri[256];
        int ret;
        
	if(ewf_config_get("web_theme_uri", uri, sizeof(uri)) != EWF_SUCCESS)
	{
		return EWF_THEME_LOAD_ERROR;
	}
        
        if(ewf_theme_load_by_uri(uri) == EWF_THEME_LOAD_SUCCESS)
        {
                return EWF_THEME_LOAD_SUCCESS;
        }

        /* try to load default theme */
        nbu_log_error("Unable to load theme uri '%s', try to load "
                      "default theme '%s'", 
                      uri, THEME_DEFAULT_URI);
        ret = ewf_theme_load_by_uri(THEME_DEFAULT_URI);
        
	return ret;
}

/*
 * return a nbu_list of ewf_theme_ipkg_t available
 *
 * 1) add 'local' themes found in docroot/themes/
 *
 * 2) get 'getlist_repo_theme_alt_scheme' pointer function 
 *    from ewf_matrix_ops
 *   -> if not NULL                     => call it
 *
 * @return EWF_SUCCESS if success, otherwise EWF_ERROR
 */
int ewf_theme_get_list(nbu_list_t **p_th_ipkg_list)
{
	int ret = EWF_SUCCESS;
	
	ewf_theme_repo_t th_repo, *p_th_repo = NULL;
	nbu_list_t *p_th_repo_list = NULL;

	*p_th_ipkg_list = nbu_list_new(sizeof(ewf_theme_ipkg_t*));
	if(*p_th_ipkg_list == NULL)
	{
		nbu_log_error("list allocation failed !");
		return EWF_ERROR;
	}

	/* local theme directory */
	nbu_string_copy(th_repo.uri, sizeof(th_repo.uri),
			"local://");
	nbu_string_printf(th_repo.pwd, sizeof(th_repo.pwd),
			  "%s/docroot/themes", 
			  ewf_config_env.dir_webapproot);

	if(ewf_theme_parse_threpo(&th_repo, *p_th_ipkg_list) != EWF_SUCCESS)
	{
		nbu_log_error("Failed to parse local repo themes directory !");
	}

	/* alt theme directories */
	if(ewf_matrix_ops.getlist_repo_theme_alt_scheme != NULL)
	{
		p_th_repo_list = nbu_list_new(sizeof(char*));
		
		if(ewf_matrix_ops.getlist_repo_theme_alt_scheme(p_th_repo_list)
		   == EWF_SUCCESS)
		{
			nbu_list_for_each_data(p_th_repo_list,
					       p_th_repo,
					       ewf_theme_repo_t)
			{
				if(ewf_theme_parse_threpo(p_th_repo, 
							  *p_th_ipkg_list)
				   != EWF_SUCCESS)
				{
					nbu_log_error("Failed to parse alt "
						      "theme directory '%s' !",
						      p_th_repo->pwd);
				}
			}
		}
		else
		{
			nbu_log_error("Error when parse alt local themes "
				      "directories");
		}

		nbu_list_destroy(p_th_repo_list);
	}
	
	return ret;
}

/*
 * set the current theme
 *
 * 1) try to load theme
 *    -> if success, set 'web_theme_uri' config value with this uri
 *    -> otherwise, return the error code
 *
 */
int ewf_theme_set(const char *uri)
{
	int ret;
	
        ret = ewf_theme_load_by_uri(uri);
	if(ret == EWF_THEME_LOAD_SUCCESS)
	{
		if(ewf_config_set("web_theme_uri", uri) != EWF_SUCCESS)
		{
			nbu_log_error("web_theme_uri set failed !");
			ret = EWF_THEME_LOAD_ERROR;
		}
	}
	
	return ret;
}

int ewf_theme_cut_uri(const char *uri,
                      char *scheme,
                      size_t scheme_size,
                      char *hlpart,
                      size_t hlpart_size)
{
	char *p = NULL;
	
	p = strstr( uri, "://" );
	if(p == NULL)
	{
		nbu_log_error("Invalid uri '%s' !\n", uri);
		return EWF_ERROR;
	}
	
	if(p == uri || p[3] == '\0')
	{
		nbu_log_error("Invalid uri '%s' ! no scheme and/or "
			      "hierarchical part !\n", uri);
		return EWF_ERROR;
	}

	snprintf(scheme, scheme_size, "%.*s", (int)(p - uri), uri);
	snprintf(hlpart, hlpart_size, "%s", p + 3);
	
	return EWF_SUCCESS;
}

const char* ewf_theme_uri_basename(const char *uri)
{
        const char *p = uri;
        int i = 0;
        
        i = strlen(uri) - 1;
        while(i >= 0)
        {
                if(*(uri + i) == '/')
                {
                        p = uri + i + 1;
                        break;
                }
                --i;
        }
        
        return p;
}
                           
/**************************************************************/
/*                STATIC FUNCTION DEFINITIONS                 */
/**************************************************************/

static int ewf_theme_parse_threpo(const ewf_theme_repo_t *th_repo,
				  nbu_list_t *p_th_ipkg_list)
{
	struct dirent *dirent;
	DIR *dir;
        int ret;

	ewf_theme_ipkg_t *p_th_ipkg = NULL;

	if((dir = opendir(th_repo->pwd)) == NULL)
	{
		return EWF_ERROR;
	}

	while ((dirent = readdir(dir)))
	{
		if(nbu_string_matches(dirent->d_name, 
				      ".") == NBU_STRING_EQUAL
		   || nbu_string_matches(dirent->d_name, 
					 "..") == NBU_STRING_EQUAL)
		{
			continue;
		}
		
		/* alloc ewf_theme_ipkg_t and try to parse xml file */
		p_th_ipkg = calloc(1, sizeof(ewf_theme_ipkg_t));
                ret = ewf_theme_parse_thdir(th_repo->pwd,
                                            dirent->d_name, 
                                            &(p_th_ipkg->theme));
		if(ret == EWF_THEME_PARSE_SUCCESS)
		{
                        /* install preview */
                        if(ewf_theme_install_preview(th_repo->pwd,
                                                     p_th_ipkg->theme.name,
                                                     p_th_ipkg->theme.uuid,
                                                     p_th_ipkg->theme.version)
                           == EWF_SUCCESS)
                        {
                                p_th_ipkg->has_preview = 1;
                        }

                        /* check target */
                        if(ewf_theme_check_compatibility(&p_th_ipkg->theme)
                           == EWF_SUCCESS)
                        {
                                p_th_ipkg->is_compatible = 1;
                        }
                                
			/* format theme uri */
			nbu_string_printf(p_th_ipkg->uri,
					  sizeof(p_th_ipkg->uri),
					  "%s%s", 
					  th_repo->uri, dirent->d_name);

			nbu_list_insert_end(p_th_ipkg_list,
					    p_th_ipkg);
		}
		else
		{
			nbu_log_error("Parse theme '%s/%s' failed (error %d)", 
				      th_repo->pwd, dirent->d_name, ret);
			free(p_th_ipkg);
			continue;
		}
	}
	
	closedir(dir);
	
	return EWF_SUCCESS;
}

static int ewf_theme_parse_thdir(const char *th_repo_pwd, 
				 const char *th_dirname,
				 ewf_theme_t *theme)
{
	struct stat buf_stat;
	char buf[256];
	FILE* xml_fd = NULL;
	ezxml_t xml_node;
	int ret = EWF_THEME_PARSE_ERROR;
        const char *val = NULL;
	
	/* format theme pwd */
	nbu_string_printf(buf,
			  sizeof(buf),
			  "%s/%s", th_repo_pwd, th_dirname);
	
	/* check if theme pwd is a directory */
	if(stat(buf, &buf_stat) != 0 || !S_ISDIR(buf_stat.st_mode))
	{
		nbu_log_error("Try to parse invalid directory");
		return EWF_THEME_PARSE_ERROR;
	}

	/* it's seems a directory theme */	
	/* try to open theme.xml */
	strncat(buf, "/theme.xml", sizeof(buf) - strlen(buf) - 1);
	if((xml_fd = fopen(buf, "r")) != NULL)
	{
		if((xml_node = ezxml_parse_fp(xml_fd)))
		{
			/* Fill ewf_theme_t structure */
			snprintf(theme->name, 
				 sizeof(theme->name),
				 "%s",
				 th_dirname);

#define GET_CHECK_AND_SET(name, stor, regex, error)   do {              \
                        val = ezxml_child_txt(xml_node, #name);         \
                        if((regex == NULL)                              \
                           || (val != NULL                              \
                               && ewf_utils_pcre_validate(val, regex) == 0)) \
                        {                                               \
                                snprintf(stor,                          \
                                         sizeof(stor),                  \
                                         "%s", val);                    \
                        }                                               \
                        else                                            \
                        {                                               \
                                nbu_log_error("Invalid value of %s "    \
                                              "xml node", #name);       \
                                ret = error;                            \
                                goto xml_error;                         \
                        }                                               \
                        } while(0)
                                  
                        GET_CHECK_AND_SET(uuid,
                                          theme->uuid,
                                          THEME_REGEX_UUID,
                                          EWF_THEME_PARSE_INVALID_UUID);
                        GET_CHECK_AND_SET(name,
                                          theme->longname,
                                          NULL,
                                          EWF_THEME_PARSE_MISSVALUE);
                        GET_CHECK_AND_SET(version,
                                          theme->version,
                                          THEME_REGEX_VERSION,
                                          EWF_THEME_PARSE_INVALID_VERSION);
                        GET_CHECK_AND_SET(target,
                                          theme->target,
                                          THEME_REGEX_TARGET,
                                          EWF_THEME_PARSE_INVALID_TARGET);
                        GET_CHECK_AND_SET(description,
                                          theme->description,
                                          NULL,
                                          EWF_THEME_PARSE_MISSVALUE);
                        GET_CHECK_AND_SET(creator,
                                          theme->author,
                                          NULL,
                                          EWF_THEME_PARSE_MISSVALUE);
                        GET_CHECK_AND_SET(homepage,
                                          theme->homepage,
                                          NULL,
                                          EWF_THEME_PARSE_MISSVALUE);
#undef GET_CHECK_AND_SET
                        
                        ret = EWF_THEME_PARSE_SUCCESS;
			
                xml_error:
			ezxml_free(xml_node);
		}
		else
		{
			nbu_log_error("Unable to parse '%s'",
				      buf);
                        ret = EWF_THEME_PARSE_INVALIDXML;
		}
		
		fclose(xml_fd);
	}
	else
	{
		nbu_log_error("Unable to open '%s' (%m)",
			      buf);
                ret = EWF_THEME_PARSE_NOXML;
	}
	
	return ret;
}

static int ewf_theme_install_preview(const char *repopwd,
                                     const char *thname,
                                     const char *thuuid,
                                     const char *thversion)
{
        char thpwdpreview[512];
        char thvdocpreview[512];
        struct stat buf_stat;
        int ret = EWF_ERROR;
        
        nbu_string_printf(thpwdpreview, sizeof(thpwdpreview),
                          "%s/%s/preview.png", repopwd, thname);
        
        nbu_log_debug("%s exist ?", thpwdpreview);
        
        /* verify existance */
        if(stat(thpwdpreview, &buf_stat) == -1)
        {
                nbu_log_error("No preview for theme '%s'", 
                              thname);
                return EWF_ERROR;
        }
        
        /* install preview */
        nbu_string_printf(thvdocpreview, sizeof(thvdocpreview),
                          "%s/preview/%s-%s-%s.png",
                          ewf_config_env.dir_vdocroot,
                          thname, 
                          thuuid, 
                          thversion);

        /* if symlink already exist, remove it */
        if(lstat(thvdocpreview, &buf_stat) == 0)
        {
                if(unlink(thvdocpreview) != 0)
                {
                        nbu_log_error("'%s' unlink failed ! (%m)",
                                      thvdocpreview);
                        return EWF_ERROR;
                }
        }
        
        /* make preview symlink ! */
        if(symlink(thpwdpreview, thvdocpreview) == 0)
        {
                ret = EWF_SUCCESS;
        }
        else
        {
                nbu_log_error("%m (%s -> %s)", 
                              thpwdpreview, thvdocpreview);
        }
        
        return ret;
}

static int ewf_theme_check_compatibility(const ewf_theme_t *theme)
{
        int ret = EWF_ERROR;
	nbu_list_t *list_ver = NULL, *list_min_ver = NULL, 
                *list_max_ver = NULL, *list_cur_ver = NULL;
	char min_ver[32], max_ver[32]; 
        const char *cur_ver = ewf_matrix_vars.version;
	long i_min_ver = 0, i_max_ver = 0, i_cur_ver = 0;
	
	int decalator = 0;
	char *ver = NULL;
	int num = 0;
	
	if(theme->target == NULL)
	{
		nbu_log_error("target value is null !");
		return ret;
	}

	if(nbu_string_split(theme->target, ",", &list_ver) != NBU_SUCCESS)
	{
		nbu_log_error("split target value with ',' failed !");
		return ret;
	}

	if(nbu_list_count(list_ver) != 2)
	{
		nbu_log_error("Invalid target value '%s' !", theme->target);
		goto v_clean;
	}

#define FIRST_GLUE_FOR_LAZY_DEV(NAME, IDX) \
	if(nbu_string_trim(nbu_list_get_at_index(list_ver, IDX), NAME, sizeof(NAME)) != NBU_SUCCESS) \
	{ \
		nbu_log_error("trim '%s' failed", #NAME); \
		goto v_clean; \
	}

	FIRST_GLUE_FOR_LAZY_DEV(min_ver, 0);
	FIRST_GLUE_FOR_LAZY_DEV(max_ver, 1);

#undef FIRST_GLUE_FOR_LAZY_DEV

#define SECOND_GLUE_FOR_LAZY_DEV(NAME) \
	if(ewf_utils_pcre_validate(NAME, THEME_REGEX_VERSION) != EWF_SUCCESS) \
	{ \
		nbu_log_error("invalid version format '%s' !", NAME); \
		goto v_clean; \
	} \
	 \
	if(nbu_string_split(NAME, ".", &list_ ## NAME) != NBU_SUCCESS) \
	{ \
		nbu_log_error("split '%s' value with '.' failed !", #NAME); \
		goto v_clean; \
	} \
	 \
	decalator = 24; \
	nbu_list_for_each_data(list_ ## NAME, ver, char) \
	{ \
		errno = 0; \
		num = strtol(ver, NULL, 10); \
		if(errno != 0) \
		{ \
			nbu_log_error("Unable to transform '%s' into usable number", #NAME); \
			goto v_clean; \
		} \
		\
		i_ ## NAME += (num << decalator); \
		decalator -= 8; \
	}
	
	SECOND_GLUE_FOR_LAZY_DEV(min_ver);
	SECOND_GLUE_FOR_LAZY_DEV(max_ver);
	SECOND_GLUE_FOR_LAZY_DEV(cur_ver);

#undef SECOND_GLUE_FOR_LAZY_DEV

	nbu_log_debug("%s - %s - %s", min_ver, cur_ver, max_ver);
	nbu_log_debug("%d - %d - %d", i_min_ver, i_cur_ver, i_max_ver);
	
	if(i_cur_ver >= i_min_ver && i_cur_ver <= i_max_ver)
	{
		ret = EWF_SUCCESS;
	}
	
v_clean:
	nbu_list_destroy(list_ver);
	nbu_list_destroy(list_min_ver);
	nbu_list_destroy(list_max_ver);
	nbu_list_destroy(list_cur_ver);
	
	return ret;
}

static int ewf_theme_install(const char *pwd)
{
	static char *th_files[] = {
		"/img",
		"/favicon.ico",
		"/css/common.css",
	};
	size_t i;

	for(i = 0; i < NBU_ARRAY_SIZE(th_files); i++)
	{
		if(ewf_theme_symlink_to_vdocroot(pwd, 
						 th_files[i]) != EWF_SUCCESS)
		{
			nbu_log_error("Unable to symlink '%s%s' to htdocs",
				      pwd, th_files[i]);
			
			return EWF_ERROR;
		}
	}
				
	return EWF_SUCCESS;
}

static int ewf_theme_basefile_install(void)
{
	char pwd_cmn[256], pwd[256];
	char rlt_filename[64];
	struct dirent *dirent;
	DIR *dir;
	int ret = EWF_SUCCESS;

	nbu_string_printf(pwd_cmn, sizeof(pwd_cmn),
			  "%s/docroot", 
			  ewf_config_env.dir_webapproot);
	
        /* create css directory */
        nbu_string_printf(pwd, sizeof(pwd),
			  "%s%s", ewf_config_env.dir_vdocroot, "/css");
        if(mkdir(pwd, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
        {
                nbu_log_notice("Warning: mkdir failed '%m'");
        }
        
	/* install common css */
	nbu_string_printf(pwd, sizeof(pwd),
			  "%s/css", pwd_cmn);

	if((dir = opendir(pwd)) != NULL)
	{
		while ((dirent = readdir(dir)))
		{
			if(nbu_string_matches(dirent->d_name, ".") 
			   == NBU_STRING_EQUAL
			   || nbu_string_matches(dirent->d_name, "..")
			   == NBU_STRING_EQUAL)
			{
				continue;
			}
			
			nbu_string_printf(rlt_filename,
					  sizeof(rlt_filename),
					  "/css/%s", dirent->d_name);
			
			if(ewf_theme_symlink_to_vdocroot(pwd_cmn, 
							 rlt_filename)
			   != EWF_SUCCESS)
			{
				nbu_log_error("Unable to symlink '%s%s' "
					      "to htdocs",
					      pwd_cmn, rlt_filename);
			
				ret = EWF_ERROR;
				continue;
			}
		}
		closedir(dir);
	}
	
	/* install common js */
	if(ewf_theme_symlink_to_vdocroot(pwd_cmn,
					 "/js") != EWF_SUCCESS)
	{
		nbu_log_error("Unable to symlink '%s/js' to htdocs",
			      pwd_cmn);

		ret = EWF_ERROR;
	}
	
	return ret;
}

static int ewf_theme_load_by_uri(const char *uri)
{
	char lthdir[256];
	char scheme[32];
	char hlpart[256];
	int ret = EWF_THEME_LOAD_ERROR;
	
	if(ewf_theme_cut_uri(uri, 
			     scheme, sizeof(scheme),
			     hlpart, sizeof(hlpart)) != EWF_SUCCESS)
	{
		return EWF_THEME_LOAD_INVALID_URI;
	}
	
	if(nbu_string_matches(scheme, "local") == NBU_STRING_EQUAL)
	{
		nbu_string_printf(lthdir,
				  sizeof(lthdir),
				  "%s/docroot/themes/%s",
                                  ewf_config_env.dir_webapproot,
				  hlpart);
	}
	else
	{
		if(ewf_matrix_ops.preload_theme_alt_scheme == NULL)
		{
			return EWF_THEME_LOAD_UNKNOWN_SCHEME;
		}
		
		ret = ewf_matrix_ops.preload_theme_alt_scheme(scheme,
							      hlpart,
							      lthdir,
							      sizeof(lthdir));
		if(ret != EWF_THEME_LOAD_SUCCESS)
		{
			nbu_log_error("ewf_matrix_ops.preload_theme_alt_scheme "
				      "failed !");
			return ret;
		}
		
	}

	if(ewf_theme_basefile_install() != EWF_SUCCESS)
	{
		nbu_log_error("basefile install failed !");
		
		return EWF_THEME_LOAD_ERROR;
	}

	if(ewf_theme_install(lthdir) == EWF_SUCCESS)
	{
		ret = EWF_THEME_LOAD_SUCCESS;
	}
	else
	{
		nbu_log_error("Installation of theme failed !");
		ret = EWF_THEME_LOAD_INSTALL_FAILED;
	}

	return ret;
}

static int ewf_theme_symlink_to_vdocroot(const char *themepwd,
					 const char *filename)
{
	struct stat vdoc_statfile;
	char thdoc_file[256];
	char vdoc_file[256];
	
	nbu_string_printf(thdoc_file, sizeof(thdoc_file),
			  "%s%s", themepwd, filename);
	nbu_string_printf(vdoc_file, sizeof(vdoc_file),
			  "%s%s", ewf_config_env.dir_vdocroot, filename);
	
	if(lstat(vdoc_file, &vdoc_statfile) == 0)
	{
		/* symlink already exist, remove it */
		if(remove(vdoc_file) != 0)
		{
			nbu_log_error("Unable to remove '%s': %s !", 
				      vdoc_file, strerror(errno));
			return EWF_ERROR;
		}
	}
        
	if(symlink(thdoc_file, vdoc_file) != 0)
	{
		nbu_log_error("symlink failed {");
		nbu_log_error("old='%s' ", thdoc_file);
		nbu_log_error("  -> new='%s'", vdoc_file);
		nbu_log_error("} ! : %s", strerror(errno)); 
		return EWF_ERROR;
	}

	return EWF_SUCCESS;
}
