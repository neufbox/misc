#ifndef _KIT_HELPER_H_
#define _KIT_HELPER_H_

#include <string.h>

/* get value in nv/status/dsl with policy control
 *
 * @return -1 if doesn't exist or unavailable, 0 if success
 */
extern int kit_helper_get(const char *name, char *buf, size_t buf_size);

/* set value in nv/status/dsl with policy control
 *
 * @return -1 if fail (var doesn't exist or unavailable), otherwise NBD_*
 *   code return
 */
extern int kit_helper_set(const char *name, const char *value);

/* do a action with policy control
 *
 * @return -1 if fail, 0 if success
 */
extern int kit_helper_action(const char *scriptname, const char *action);

#ifdef DEBUG
extern int kit_helper_test(void);
#endif

#endif
