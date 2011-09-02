#include "ewf/ewf.h"

#include "ewf/core/ewf_session.h"

#include <etk/io.h>

#include "nbu/nbu.h"
#include "nbu/list.h"
#include "nbu/log.h"

nbu_list_t* ewf_session_list;

static void __attribute__((constructor)) __init(void)
{
	ewf_session_list = nbu_list_new(sizeof(ewf_session_t));
}

static void __attribute__((destructor)) __fini(void)
{
	nbu_list_destroy(ewf_session_list);
}

int ewf_session_list_add(ewf_session_t* ewf_session) 
{
	return (nbu_list_insert_end(ewf_session_list, ewf_session) == NBU_SUCCESS 
		? EWF_SUCCESS : EWF_ERROR);
}

int ewf_session_list_remove(const char* session_id) 
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

ewf_session_t* ewf_session_list_get(const char* session_id) 
{
	ewf_session_t* ewf_session = NULL;
	
	if(session_id == NULL) 
	{
		return NULL;
	}
  
	nbu_list_for_each_data(ewf_session_list, ewf_session, ewf_session_t)
	{
		if(nbu_string_ncompare(ewf_session->id, sizeof(ewf_session->id), session_id) == NBU_STRING_EQUAL)
		{
			/* session found */
			return ewf_session;
		}
	}
	
	return NULL;
}

size_t ewf_session_list_size(void) 
{
	return nbu_list_count(ewf_session_list);
}

int ewf_session_list_cleaner(void) 
{
	ewf_session_t* ewf_session = NULL;
	struct timeval tv;
	int count = 0;
  
	gettimeofday(&tv, NULL);

	nbu_list_for_each_data(ewf_session_list, ewf_session, ewf_session_t)
	{
		if(ewf_session->activated == 0 && (ewf_session->tv.tv_sec + 5 < tv.tv_sec)) 
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

void ewf_session_list_print(void) 
{
	ewf_session_t* ewf_session = NULL;
  
	nbu_log_info("----- START - SESSION LIST -----");
	nbu_list_for_each_data(ewf_session_list, ewf_session, ewf_session_t)
	{
		nbu_log_info("-- ITEM(p=%x, a=%i, s=%i)", 
			     ewf_session, 
			     ewf_session->activated,
			     ewf_session->tv.tv_sec);
	}
	nbu_log_info("-----  END  - SESSION LIST -----");
}

ewf_session_t* ewf_session_create(void)
{
	unsigned char rdm[SID_LEN / 2];
	FILE *f;
	size_t s;

	struct timeval tv;
	int n;

	ewf_session_t *session = NULL;
  
	if(gettimeofday(&tv, NULL) == 0)
	{
		session = calloc(1, sizeof(ewf_session_t));
		
		/* create the random SID_LEN chars */
		f = fopen("/dev/random", "r");
		if(f == NULL) {
			nbu_log_error("cannot open /dev/random");
			return NULL;
		}

		s = fread(rdm, 1, SID_LEN / 2, f);
		fclose(f);

		if(s != SID_LEN / 2) {
			nbu_log_error("/dev/random: read error %d bytes instead of %d",
				      s, SID_LEN / 2);
			return NULL;
		}

		for (n = 0; n < SID_LEN / 2; n++)
		{
			nbu_string_printf(session->id + 2 * n, 3, 
					  "%02x", (unsigned char) rdm[n]);
		}
		
		*(session->id + SID_LEN) = '\0';
		session->activated = 0;
		session->tv = tv;
	}

	return session;
}

/*
 *
 * check if session exist (activated or not)
 *
 * @param const char* id (session id)
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
int ewf_session_check(const char *id) 
{
	return (ewf_session_list_get(id) != NULL ? EWF_SUCCESS : EWF_ERROR);
}
