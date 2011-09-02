#ifndef _EWF_PAGE_H_
#define _EWF_PAGE_H_

#include "ewf/ewf.h"

#define DISPLAY_HANDLER_NAME display_handler
#define FORM_HANDLER_NAME form_handler
#define AJAX_HANDLER_NAME ajax_handler
#define DISPLAY_AJAX_HANDLER_NAME display_ajax_handler

#define DISPLAY_HANDLER static void DISPLAY_HANDLER_NAME(void)
#define FORM_HANDLER static void FORM_HANDLER_NAME(void)
#define AJAX_HANDLER static void AJAX_HANDLER_NAME(void)
#define DISPLAY_AJAX_HANDLER static void DISPLAY_AJAX_HANDLER_NAME(void)

#define PAGE(PAGENAME, ACCESS_POLICY, DHANDLER, FHANDLER, AHANDLER)	\
        static ewf_page_t page;                                         \
	static void __attribute__((constructor)) __init1(void)		\
	{								\
		ewf_page_register(&page);				\
	}                                                               \
	static ewf_page_t page = {					\
		.name = PAGENAME,					\
		.access_policy = ACCESS_POLICY,				\
		.display_handler = DHANDLER,				\
		.form_handler = FHANDLER,				\
		.ajax_handler = AHANDLER,				\
	}

/*
 * _u = DISPLAY and AJAX request are processed by one and same handler 
 *         DISPLAY_AJAX_HANDLER
 */

#define PAGE_DFA(PAGENAME, ACCESS_POLICY)				\
	DISPLAY_HANDLER; FORM_HANDLER; AJAX_HANDLER;			\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     DISPLAY_HANDLER_NAME, FORM_HANDLER_NAME, AJAX_HANDLER_NAME)

#define PAGE_DFA_u(PAGENAME, ACCESS_POLICY)				\
	DISPLAY_AJAX_HANDLER; FORM_HANDLER;                             \
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     DISPLAY_AJAX_HANDLER_NAME,                                 \
             FORM_HANDLER_NAME, DISPLAY_AJAX_HANDLER_NAME)

#define PAGE_DF(PAGENAME, ACCESS_POLICY)				\
	DISPLAY_HANDLER; FORM_HANDLER;					\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     DISPLAY_HANDLER_NAME, FORM_HANDLER_NAME, NULL)

#define PAGE_DA(PAGENAME, ACCESS_POLICY)				\
	DISPLAY_HANDLER; AJAX_HANDLER;					\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     DISPLAY_HANDLER_NAME, NULL, AJAX_HANDLER_NAME)

#define PAGE_DA_u(PAGENAME, ACCESS_POLICY)				\
	DISPLAY_AJAX_HANDLER;                                           \
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     DISPLAY_AJAX_HANDLER_NAME, NULL, DISPLAY_AJAX_HANDLER_NAME)

#define PAGE_D(PAGENAME, ACCESS_POLICY)					\
	DISPLAY_HANDLER;						\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     DISPLAY_HANDLER_NAME, NULL, NULL)

#define PAGE_FA(PAGENAME, ACCESS_POLICY)				\
	FORM_HANDLER; AJAX_HANDLER;					\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     NULL, FORM_HANDLER_NAME, AJAX_HANDLER_NAME)

#define PAGE_F(PAGENAME, ACCESS_POLICY)					\
	FORM_HANDLER;							\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     NULL, FORM_HANDLER_NAME, NULL)				

#define PAGE_A(PAGENAME, ACCESS_POLICY)					\
	AJAX_HANDLER;							\
	PAGE(PAGENAME, ACCESS_POLICY,					\
	     NULL, NULL, AJAX_HANDLER_NAME)

#define PAGE_ERROR PAGE_DA(URI_PAGE_ERROR, EWF_ACCESS_PUBLIC)
#define PAGE_LOGIN PAGE_DF(URI_PAGE_LOGIN, EWF_ACCESS_PUBLIC)
#define PAGE_LOGOUT PAGE_D(URI_PAGE_LOGOUT, EWF_ACCESS_PRIVATE)

/*! Enum to set access public or private
 */
typedef enum ewf_page_access {
	EWF_ACCESS_PRIVATE = 0,
	EWF_ACCESS_PUBLIC,
} ewf_page_access_t;

/*! Structure representing a page of the WebUI.
 */
typedef struct ewf_page {
	char *name;                    /*!< Name of the page */
	ewf_page_access_t access_policy;   /*!< This page is 
					    * public or private ? */
	void (*display_handler)(void);     /*!< Pointer to the function 
					    * handling displays for this page*/
	void (*form_handler)(void);        /*!< Pointer to the function 
					    * handling form submits for this 
					    * page */
	void (*ajax_handler)(void);        /*!< Pointer to the function 
					    * handling Ajax requests for this 
					    * page */
} ewf_page_t;

/*
 * Buffer for user page
 */
char buffer[256];

extern void ewf_page_register(ewf_page_t *page);

#endif
