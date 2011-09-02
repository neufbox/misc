#ifndef _API_COMMON_H_
#define _API_COMMON_H_

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#include <assert.h>

#include "fcgios.h"
#include "fcgi_stdio.h"
#include "ClearSilver.h"
#include "nbd.h"
#include "ewf.h"
#include "api_signal.h"/*!
 * \file api_common.h
 * \brief Common functions for the API.
 * \author Raphael HUCK
 */

/*! Enum to set access public or private.
 */
typedef enum _api_access_policy {
	API_ACCESS_POLICY_PUBLIC = 0,
	API_ACCESS_POLICY_PRIVATE,
} api_access_policy_t;

/*! Enum for errors.
 */
typedef enum _api_error {
	API_ERROR_UNKNOWN = 0,
	API_ERROR_FORMAT_NOT_FOUND,
	API_ERROR_METHOD_NOT_FOUND,
	API_ERROR_NEED_ARG,
	API_ERROR_INVALID_ARG,
	API_ERROR_NEED_AUTH,
	API_ERROR_BOX_UPGRADING
} api_error_t;

struct api_error_entry {
	char *err_id;
	char *err_msg;
};

/*! Structure representing a method.
 */
typedef struct _api_method {
	ewf_http_request_method_t request_method;	/*!< HTTP request method */
	api_access_policy_t access_policy;  	 	/*!< Needs authentication? */
	void (*action_handler)(void);					/*!< Handler that will be called */
} api_method_t;

/*! Structure representing the list of methods of the API.
 */
typedef struct _api_method_list {
	ewf_hashtable_t *list;						/*!< Hash table to store the methods */
	int initialized;							/*!< Set to 1 if the hash table has been initialized */
} api_method_list_t;

#define API_BUFFER_SIZE 256

#define API_GET_METHOD_NAME(CNAME, ANAME) \
	#CNAME "." #ANAME

/*! Macro to register a method.
 */
#define API_REGISTER_METHOD(CONTROLLER_NAME, ACTION_NAME, REQUEST_METHOD, ACCESS_POLICY) \
	static void ACTION_NAME(void); \
	static void __attribute__((constructor)) __init_##ACTION_NAME(void) \
	{ \
		api_register_method(API_GET_METHOD_NAME(CONTROLLER_NAME, ACTION_NAME), REQUEST_METHOD, ACCESS_POLICY, &ACTION_NAME); \
	} \
	static void ACTION_NAME(void)


/*! Global array containing the different methods. 
 */
api_method_list_t methods;

/*! Global buffer of size \c API_BUFFER_SIZE to avoid allocating and freeing memory all the time.
 */
char buffer[API_BUFFER_SIZE];

/*! Register a method by putting its struct in the global hash table.
 */
void api_register_method(const char *method_name, int request_method, int access_policy, void (*action_handler)(void));

/*! Initialize stuff on daemon startup.
 */
void api_init(void);

/*! Clean up stuff before shutting down the daemon.
 */
void api_cleanup(void);

/*! Initialize stuff before processing the new request.
 */
void api_begin(void);

/*! Clean up stuff after processing the request.
 */
void api_end(void);

/*! Dispatch the request to the appropriate action hanler. 
 */
void api_dispatch(void);

/*! Handle the request.
 */
void api_handle_request(api_method_t *m, const char *name);

/*! Set the content type of the output.
 */
void api_set_content_type(const char *type);

/*! Get the value of a URI parameter.
 */
char *api_get_parameter(const char *name);

/*! Set an HDF value.
 */
void api_set_value(const char *name, const char *value);

/*! Set the error type.
 */
void api_set_error(api_error_t error);

/*! Set custom error.
 */
void api_set_custom_error(const char *err_id, const char *err_msg);

#endif /* _API_COMMON_H_ */

