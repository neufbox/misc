#ifndef _EWF_SESSION_H_
#define _EWF_SESSION_H_

#include <sys/time.h>
#include <time.h>

#define CHALLENGE_SIZE 40
#define LOGIN_HASH_SIZE 64
#define PASS_HASH_SIZE 64

#define MAXSESSION_COUNT 64
#define PRESESSION_TIMEOUT 300
#define PRESESSION_TIMEOUT_FAST 5 /* for password auth method */
#define SESSION_TIMEOUT 3600

typedef struct ewf_session_t {
	int activated;
	char id[CHALLENGE_SIZE + 1];
	struct timeval tv;
} ewf_session_t;

/*
 * ewf_session_get: get session with session_id if exist
 *
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_get(const char* session_id, ewf_session_t **session);

/*
 * ewf_session_cleaner: remove old sessions 
 *  rules:
 *   - session unactivated => timeout=PUSHBUTTON_TIMEOUT
 *   - session activated => timeout=SESSION_TIMEOUT sec
 *
 * @return number of destroyed session
 */
extern int ewf_session_cleaner(void);

/*
 * ewf_session_create: create a session unactivated with a random 
 *  id (challenge number)
 *
 * @param char* session_id (initialised char[CHALLENGE_SIZE + 1] array) 
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_create(char *buf_session_id, size_t buf_size);

/*
 * ewf_session_remove: remove session
 *
 * @param const char* session_id (session_id is challenge random number)
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_remove(const char* session_id);

/*
 * ewf_session_check: check that session is correct and update it.
 *   remove it if obsolete
 *
 * @param ewf_session_t session
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_check(ewf_session_t *session);

/*
 * Validate a session (ie. set activated and update time)
 *
 * @param ewf_session_t session to validate
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_validate(ewf_session_t *session);

/*
 * @return number to session activated of not
 */
extern int ewf_session_count(void);

#endif
