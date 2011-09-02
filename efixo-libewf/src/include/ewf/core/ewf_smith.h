#ifndef _EWF_SMITH_H
#define _EWF_SMITH_H

#include "ewf/ewf_page.h"

enum ewf_smith_ctl_return {
	EWF_SMITH_CTL_NOT_ALLOWED = -1000,
	EWF_SMITH_CTL_SUCCESS = 0,
	EWF_SMITH_CTL_AUTH_OFF,
	EWF_SMITH_CTL_PUBLICPAGE,
	EWF_SMITH_CTL_ALREADY_AUTH,
};

enum ewf_smith_login_return {
	EWF_SMITH_LOGIN_INVALID_SESSION = -1000,
	EWF_SMITH_LOGIN_INVALID_ARGS,
	EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED,
	EWF_SMITH_LOGIN_ERROR = -1,
	EWF_SMITH_LOGIN_SUCCESS = 0,
};

/*
 * Control user access
 *
 * \param page ewf_page_t *
 * \return integer >= EWF_SMITH_CTL_SUCCESS if success, < otherwise
 *
 *        * access granted :
 *            EWF_SMITH_CTL_SUCCESS if user has a good session
 *            EWF_SMITH_CTL_AUTH_OFF if access control is disabled
 *	      EWF_SMITH_CTL_PUBLICPAGE if the page requested is public
 *
 *        * access failed :
 *            EWF_SMITH_CTL_NOT_ALLOWED if user isn't allowed to view this page
 *
 */
extern int ewf_smith_access_control(const ewf_page_t *page);

/*
 * User login function
 *
 * \return EWF_SMITH_LOGIN_SUCCESS if success, otherwise:
 *
 *        * error :
 *            EWF_SMITH_LOGIN_INVALID_SESSION if zombie session is invalid
 *            EWF_SMITH_LOGIN_INVALID_ARGS if arguments required are wrong
 *            EWF_SMITH_LOGIN_METHOD_NOT_ALLOWED if auth method not allowed
 *            EWF_SMITH_LOGIN_ERROR a dramatic error occur
 *            ... (user defined with matrix)
 *
 */
extern int ewf_smith_login(void);

/*
 * User logout function
 *
 * \return EWF_SUCCESS if success, otherwise EWF_ERROR:
 *
 */
extern int ewf_smith_logout(void);


#endif
