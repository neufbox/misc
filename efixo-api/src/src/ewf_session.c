#include "ewf_session.h"
#include "ewf.h"

#include <assert.h>
#include <etk/io.h>

#include "nbu/nbu.h"
#include "nbu/list.h"
#include "nbu/log.h"

static nbu_list_t* ewf_session_list;

static void __attribute__((constructor)) __init(void)
{
	ewf_session_list = nbu_list_new(sizeof(ewf_session_t));
}

static void __attribute__((destructor)) __fini(void)
{
	nbu_list_destroy(ewf_session_list);
}

int ewf_session_get(const char* session_id, ewf_session_t **session)
{
	*session = NULL;
	
	if(session_id == NULL)
	{
		return EWF_ERROR;
	}
	
	nbu_list_for_each_data(ewf_session_list, *session, ewf_session_t)
	{
		if(nbu_string_ncompare((*session)->id, sizeof((*session)->id), session_id) == NBU_STRING_EQUAL)
		{
			/* session found */
			return EWF_SUCCESS;
		}
	}
	
	return EWF_ERROR;
}

int ewf_session_cleaner(void)
{
	ewf_session_t* ewf_session = NULL;
	struct timeval tv;
	int count = 0;
  
	gettimeofday(&tv, NULL);

	nbu_list_for_each_data(ewf_session_list, ewf_session, ewf_session_t)
	{
		if(ewf_session->activated == 0 && (ewf_session->tv.tv_sec + PRESESSION_TIMEOUT < tv.tv_sec)) 
		{
			nbu_list_delete_backward(ewf_session_list);
			count++;
		} 
		else if(ewf_session->tv.tv_sec + SESSION_TIMEOUT < tv.tv_sec)
		{
			nbu_list_delete_backward(ewf_session_list);
			count++;
		}
	}
  
	return count;
}
int ewf_session_create(char *buf_token, size_t buf_token_size) 
{
	unsigned char rdm[CHALLENGE_SIZE / 2];

	struct timeval tv;
	int n, ret = EWF_ERROR;

	ewf_session_t* ewf_session = NULL;
  
	assert(buf_token);
	
	if(buf_token_size > CHALLENGE_SIZE && gettimeofday(&tv, NULL) == 0)
	{
		ewf_session = calloc(1, sizeof(ewf_session_t));
		
		/* create the random CHALLENGE_SIZE chars */
		file_open_read_close("/dev/random", rdm, CHALLENGE_SIZE / 2);

		for (n = 0; n < CHALLENGE_SIZE / 2; n++)
		{
			nbu_string_printf(buf_token + 2 * n, 3, "%02x", rdm[n]);
		}
		*(buf_token + CHALLENGE_SIZE) = '\0';

		/* add the session to the list */
		ewf_session->activated = 0;
		nbu_string_copy(ewf_session->id, sizeof(ewf_session->id), buf_token);
		ewf_session->tv = tv;
    
		ret = (nbu_list_insert_end(ewf_session_list, ewf_session) == NBU_SUCCESS ? EWF_SUCCESS : EWF_ERROR);
	}

	return ret;
}

int ewf_session_remove(const char* session_id) 
{
	int found = 0;
	ewf_session_t* ewf_session = NULL;
	
	if(session_id == NULL) 
	{
		return EWF_ERROR;
	}
  
	nbu_list_for_each_data(ewf_session_list, ewf_session, ewf_session_t)
	{
		if(nbu_string_ncompare(ewf_session->id, sizeof(ewf_session->id), session_id) == NBU_STRING_EQUAL)
		{
			/* session found */
			nbu_list_delete_backward(ewf_session_list);
			found = 1;
			
			break;
		}
	}
  
	return (found ? EWF_SUCCESS : EWF_ERROR);
}

int ewf_session_check(ewf_session_t *session)
{
	int ret = EWF_ERROR;
	struct timeval tv;
	
	if(session == NULL)
	{
		return ret;
	}

	if(gettimeofday(&tv, NULL) != 0)
	{
		nbu_log_error("Unable to gettimeofday !");
		return ret;
	}

	if(session->activated == 0)
	{
		/* no activated ! */
		nbu_log_debug("session not activated !");
		return ret;
	}

	if(session->tv.tv_sec + SESSION_TIMEOUT >= tv.tv_sec)
	{
		session->tv = tv;
		
		ret = EWF_SUCCESS;
	}
	else
	{
		/* remove this session ! */
		nbu_log_debug("session timeout !");
		
		ewf_session_remove(session->id);
	}

	return ret;
}

int ewf_session_validate(ewf_session_t *session)
{
	struct timeval tv;
	
	assert(session != NULL);
	
	if(gettimeofday(&tv, NULL) != 0)
	{
		nbu_log_error("Unable to gettimeofday !");
		return EWF_ERROR;
	}
	
	session->activated = 1;
	session->tv = tv;

	return EWF_SUCCESS;
}

int ewf_session_count(void)
{
	return nbu_list_count(ewf_session_list);
}
