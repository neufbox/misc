#ifndef _EWF_SESSION_H_
#define _EWF_SESSION_H_

#include <sys/time.h>
#include <time.h>

#define SID_LEN 40
#define LOGIN_HASH_LEN 64
#define PASSW_HASH_LEN 64

#define SESSION_TIMEOUT 3600

typedef struct ewf_session_t {
	int activated:1;
	char id[SID_LEN + 1];
	struct timeval tv;
} ewf_session_t;

/*
 * ewf_session_list_add: add session to the session list
 *
 * @param ewf_session_t* session pointer
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_list_add(ewf_session_t* ewf_session);

/*
 * ewf_session_list_remove: remove session to the session list
 *
 * @param const char* session_id (session_id is challenge random number)
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_list_remove(const char* session_id);

/*
 * ewf_session_list_get: get a session in the session list
 *  with the hash
 *
 * @param const char* session_id (session_id is challenge random number)
 * @return A ewf_session_t* if success, NULL otherwise
 */
extern ewf_session_t* ewf_session_list_get(const char* session_id);

/*
 * ewf_session_list_size: return size of the session list
 *
 * @return size_t (size of list)
 */
extern size_t ewf_session_list_size(void);

/*
 * ewf_session_list_cleaner: remove old sessions 
 *  rules:
 *   - session unactivated => timeout=5 sec
 *   - session activated => timeout=macro SESSION_TIME sec
 *
 * @return number of destroyed session
 */
extern int ewf_session_list_cleaner(void);

/*
 * ewf_session_list_print: print session list to log
 *
 * @param void
 * @return void
 */
extern void ewf_session_list_print(void);

/*
 * ewf_session_create: create a session unactivated (zombie session)
 *  with a random id (challenge number). Think to free() it.
 *
 * @param void
 * @return new allocated ewf_session_t struct if success, NULL otherwise
 */
extern ewf_session_t* ewf_session_create(void);

/*
 *
 * check if session exist (activated or not)
 *
 * @param const char* id (session id)
 * @return EWF_SUCCESS if success, EWF_ERROR otherwise
 */
extern int ewf_session_check(const char *id);

#endif
