#ifndef _EWF_HTTP_H_
#define _EWF_HTTP_H_

/*! Enum for HTTP request method.
 *  */
typedef enum _ewf_http_request_method {
	EWF_HTTP_REQUEST_METHOD_UNKNOWN = -1,
	EWF_HTTP_REQUEST_METHOD_GET = 0,
	EWF_HTTP_REQUEST_METHOD_POST,
	EWF_HTTP_REQUEST_METHOD_PUT,
	EWF_HTTP_REQUEST_METHOD_DELETE
} ewf_http_request_method_t;

/*! Determine the request method
 * \return the HTTP request method (GET, POST, PUT, DELETE)
 */
ewf_http_request_method_t ewf_http_request_method(void);

/*! Determine the request URI
 * \return the request URI
 */
char *ewf_http_request_uri(void);

#endif /* _EWF_HTTP_H_ */
