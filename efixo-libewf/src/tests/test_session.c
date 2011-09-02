#include "ewf/ewf.h"
#include "ewf/core/ewf_session.h"

#include "yatest.h"

TEST_DEF(test_session_add)
{
#define NUM_INSERT 100

	ewf_session_t *session = NULL;
	char buf[256][NUM_INSERT];
	size_t size;
	
	int i = 0;
	
	for(i = 0; i < NUM_INSERT; i++)
	{
		TEST_ASSERT((session = ewf_session_create()) != NULL,
			    "session create failed !");
		TEST_ASSERT(ewf_session_list_add(session) == EWF_SUCCESS,
			    "session_list_add failed !");
		
		snprintf(buf[i], sizeof(buf[i]),
			 "%s", session->id);
	}

	for(i = 0; i < NUM_INSERT; i++)
	{
		TEST_ASSERT((session = ewf_session_list_get(buf[i])) != NULL,
			    "session_list_get failed ! unable to get session id "
			    "'%s' !", buf[i]);
	}

	/* session timeout = 5 seconds when session isn't activated */
	sleep(6);
	
	TEST_ASSERT((size = ewf_session_list_cleaner()) == NUM_INSERT,
		    "session_list_cleaner failed ! "
		    "Expected value '%d', but got '%zu' ",
		    NUM_INSERT, size);

#undef NUM_INSERT
}

TEST_DEF(test_session_remove)
{
#define NUM_INSERT 100

	ewf_session_t *session = NULL;
	char buf[256][NUM_INSERT];
	size_t size;
	
	unsigned int i = 0;
	
	for(i = 0; i < NUM_INSERT; i++)
	{
		TEST_ASSERT((session = ewf_session_create()) != NULL,
			    "session create failed !");
		TEST_ASSERT(ewf_session_list_add(session) == EWF_SUCCESS,
			    "session_list_add failed !");
		
		snprintf(buf[i], sizeof(buf[i]),
			 "%s", session->id);
	}

	for(i = 0; i < NUM_INSERT; i++)
	{
		TEST_ASSERT(ewf_session_list_remove(buf[i]) == EWF_SUCCESS,
			    "session_list_remove failed ! unable to remove session "
			    "'%s' !", buf[i]);
		
		TEST_ASSERT((size = ewf_session_list_size()) == (NUM_INSERT - i - 1),
			    "session_list_size failed ! "
			    "Expected value '%d', but got '%zu' ",
			    NUM_INSERT - i - 1, size);
	}

#undef NUM_INSERT
}

int main(void)
{
	TEST_INIT("ewf_session");
	
	TEST_RUN(test_session_add);
	TEST_RUN(test_session_remove);
	
	return TEST_RETURN;
}
