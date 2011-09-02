#ifndef _EWF_HDF_H_
#define _EWF_HDF_H_

#include <stdlib.h>
#include "ClearSilver.h"

struct ewf_hdf_iterator  {
    HDF * parentNode;
    HDF * node;
    char * name;
    char * value;
};



/*
 * Helper macro intented to use in handler code of a page.
 */
#define EWF_HDF_GET(varname)                                            \
        do {                                                            \
                varname = ewf_hdf_sget(#varname);                       \
                if(varname == NULL) {                                   \
                        nbu_log_notice("%s has invalid value", #varname); \
                        return;                                         \
                }                                                       \
        } while(0)

#define EWF_HDF_GET_DEFAULT(varname, dflval)                            \
        do {                                                            \
                varname = ewf_hdf_sget_default(#varname, dflval);       \
        } while(0)

#define EWF_HDF_GET_INT(varname)                                        \
        do {                                                            \
                varname = ewf_hdf_sget_int(#varname);                   \
                if(varname == -1) {                                     \
                        nbu_log_notice("%s has invalid value", #varname); \
                        return;                                         \
                }                                                       \
        } while(0)

#define EWF_HDF_GET_INT_DEFAULT(varname, dflval)                        \
        do {                                                            \
                varname = ewf_hdf_sget_int_default(#varname, dflval);   \
                if(varname == dflval) {                                 \
                        nbu_log_notice("%s has invalid value."          \
                                       " set to dflval = %d",           \
                                       #varname, dflval);               \
                }                                                       \
        } while(0)

#define EWF_HDF_FOREACH(varname, iterator)                              \
        for(ewf_hdf_each(varname, iterator ); (iterator)->node ; ewf_hdf_each(NULL, iterator))

/*********************************************************************
 * SET'TER accessor
 ********************************************************************/
/*
 * All ewf_hdf_set* functions prefix hdf name by 'Var.'
 * and don't overwrite an value which already exist !
 *
 * - the set_int and set functions add 'Value' suffix to hdf name
 * - the set_status function add 'Status' suffix to hdf name
 * - the set_error function register :
 *      - Var.$1.Error = $error_id
 *      - Var.$1.Status = -1
 *
 * The ewf_hdf_form_set_error function register :
 *
 * - Form.Error = $error_id
 *
 * The ewf_hdf_form_set_success function register :
 * 
 * - Form.Success = $notice_id
 *
 * The ewf_hdf_owset* functions can overwrite an value which
 * already exist
 * 
 */

/*
 * ewf_hdf_setf
 *
 * set formated value to an hdf variable (Var.${var_name}.Value)
 * @param var_name Var name
 * @param fmt Formated string like arg 1 printf
 */
extern int ewf_hdf_setf(const char *var_name, const char *fmt, ...);

/*
 * ewf_hdf_set
 *
 * set string value to an hdf variable (Var.${var_name}.Value)
 * @param var_name Var name
 * @param str_value String value
 */
extern int ewf_hdf_set(const char *var_name, const char *str_value);

/*
 * ewf_hdf_set_int
 *
 * set int value to an hdf variable (Var.${var_name}.Value)
 * @param var_name Var name
 * @param int_value Int value
 */
static inline int ewf_hdf_set_int(const char *var_name, int int_value)
{
	return ewf_hdf_setf(var_name, "%d", int_value);
}

/*
 * ewf_hdf_set_error
 *
 * set a error associated to an hdf variable:
 *   Var.${var_name}.Error = ${error_id})
 * @param var_name Var name
 * @param error_id String value
 */
extern int ewf_hdf_set_error(const char *var_name, const char *error_id);
#define ewf_hdf_query_var_error ewf_hdf_set_error

/*
 * ewf_hdf_form_set_error
 *
 * set a form error associated to a form (Form.Status = -1 and 
 *   Form.Error = ${error_id})
 * @param var_name Var name
 * @param error_id String value
 */
extern int ewf_hdf_form_set_error(const char *error_id);
#define ewf_hdf_query_error ewf_hdf_form_set_error

/*
 * ewf_hdf_form_set_success
 *
 * set a form success associated to a form (Form.Status = 0 and 
 *   Form.Success = ${success_id})
 * @param var_name Var name
 * @param success_id String value
 */
extern int ewf_hdf_form_set_success(const char *success_id);
#define ewf_hdf_query_success ewf_hdf_form_set_success

/*
 * ewf_hdf_owsetf
 *
 * set formated value to an hdf variable (Var.${var_name}.Value)
 * @param var_name Var name
 * @param fmt Formated string like arg 1 printf
 */
extern int ewf_hdf_owsetf(const char *var_name, const char *fmt, ...);

/*
 * ewf_hdf_owset
 *
 * set string value to an hdf variable (Var.${var_name}.Value)
 * @param var_name Var name
 * @param str_value String value
 */
extern int ewf_hdf_owset(const char *var_name, const char *str_value);

/*
 * ewf_hdf_set_owset_int
 *
 * set int value to an hdf variable (Var.${var_name}.Value)
 * @param var_name Var name
 * @param int_value Int value
 */
static inline int ewf_hdf_owset_int(const char *var_name, int int_value)
{
	return ewf_hdf_owsetf(var_name, "%d", int_value);
}

/*********************************************************************
 * GET'TER accessor
 ********************************************************************/
/* 
 * ewf_hdf_env_gettext_d explanation
 * ---------------------------------
 * 
 * 'var_name' is PREFIXED by 'Env.Var' and SUFFIXED by '.Value'
 *
 * Get text associated to 'value' for the 'var_name'
 *
 * example:
 *
 *  ewf_hdf_env_gettext_d("hotspot_status", value, defautvalue);
 *
 *    - return value of "Env.Var.hotspot_status.Value.{value}" if variable exist
 *    - or return value of "Env.Var.hotspot_status.Value.{defaultvalue}" (or if defaultvalue == NULL
 *        return value of "Env.Var.hotspot_status.Value.NULL") if variable exist
 *    - or return value of "Env.Text.UnknownValue" if variable exist
 *    - or return "Unknown value" const char*
 *
 *  ! NEVER RETURN NULL !
 *
 * Usefull for localization (fr, en, ...)
 */
extern const char* ewf_hdf_env_gettext(const char *var_name, const char *value);

extern const char* ewf_hdf_env_gettext_d(const char *var_name, const char *value, const char *def_value);

/* 
 * All next get is prefixed by 'Query.' 
 * 
 * The *sget* functions used Env.Var.$(var_name).Regex hdf value to check
 * $(var_name) value submitted. If regex check failed, Var.$1.Error is set 
 * to 'value'.
 */
const char* ewf_hdf_get_default(const char *var_name, const char *dfl_val);
const char* ewf_hdf_get(const char *var_name);
int ewf_hdf_get_copy(const char *var_name, char *buf, size_t buf_len);
int ewf_hdf_get_int_default(const char *var_name, int default_val_int);
int ewf_hdf_get_int(const char *var_name);

/*
 * const char* ewf_hdf_sget_default(const char *var_name, const char *dflval)
 *
 * @return dflval value if value of Query.$(var_name) is invalid or NULL,
 *         otherwise, return the value of Query.$(var_name)
 */
const char* ewf_hdf_sget_default(const char *var_name, const char *dflval);

/*
 * const char* ewf_hdf_sget(const char *var_name)
 *
 * if Query.$(var_name) value is valid, register:
 *   - Var.$(var_name).Error = 'value'
 *   - Form.Status = -1
 *
 * @return NULL if value of Query.$(var_name) is NULL or invalid,
 *         or value of var_name if valid.
 */
const char* ewf_hdf_sget(const char *var_name);

/*
 * int ewf_hdf_sget_copy(const char *var_name, char *buf, size_t buf_len)
 *
 * if Query.$(var_name) value is valid, register:
 *   - Var.$(var_name).Error = 'value'
 *   - Form.Status = -1
 *
 * @return EWF_SUCCESS if value of Query.$(var_name) is valid,
 *         EWF_ERROR if value is NULL and invalid or if value is invalid
 */
int ewf_hdf_sget_copy(const char *var_name, char *buf, size_t buf_len);

/*
 * int ewf_hdf_sget_int_default(const char *var_name, int dflt_val_int)
 *
 * @return integer value of Query.$(var_name) value if value is valid,
 *         or dflt_val_int if value is invalid or NULL.
 */
int ewf_hdf_sget_int_default(const char *var_name, int dflt_val_int);

/*
 * int ewf_hdf_sget_int(const char *var_name)
 *
 * @return integer value of Query.$(var_name) value if value is valid,
 *         or -1 if value is invalid or NULL.
 */
int ewf_hdf_sget_int(const char *var_name);

/*********************************************************************
 * Other
 ********************************************************************/

/*! Read an HDF data file.
 */
extern void ewf_hdf_read_file(const char *name);

/*! each sub HDF data var.
 */
extern struct ewf_hdf_iterator* ewf_hdf_each(const char *name, struct ewf_hdf_iterator *hdf_iterator);

/*! Verify exist on node. Return 1 if exist, 0 otherwise
 */
extern int ewf_hdf_exist(const char *name);

/*
 * Translate all Query variable to hdf variable
 */
extern int ewf_hdf_copy_cgi_query_tree(void);
#define ewf_hdf_query_replay ewf_hdf_copy_cgi_query_tree

/*
 * Register an variable in Env. hdf node
 */
extern int ewf_hdf_set_env(const char *name, const char *value);

/*
 * Get an variable in Env. hdf node
 */
extern const char* ewf_hdf_get_env(const char *name);

/*
 * call cgi_parse
 */
extern void ewf_hdf_cgi_parse(void);

#endif
