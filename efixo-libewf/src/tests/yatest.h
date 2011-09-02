#ifndef _YATEST_H_
#define _YATEST_H_

#include <string.h>
#include <stdio.h>

#define RET_SUCCESS 0
#define RET_ERROR 1

#define COLOR_ESCAPE            "\033"
#define COLOR_RESET             COLOR_ESCAPE "[0m"

#define COLOR_BLUE(txt)         COLOR_ESCAPE "[0;34m" txt COLOR_RESET
#define COLOR_GREEN(txt)        COLOR_ESCAPE "[0;32m" txt COLOR_RESET
#define COLOR_RED(txt)          COLOR_ESCAPE "[0;31m" txt COLOR_RESET
#define COLOR_LIGHT_RED(txt)	COLOR_ESCAPE "[1;31m" txt COLOR_RESET
#define COLOR_YELLOW(txt)	COLOR_ESCAPE "[1;33m" txt COLOR_RESET

typedef struct ret_t {
	int id;
	int line;
	char msg[256];
} ret_t;

typedef void (* func_test)(ret_t*);

#define PRINT_ERROR(m, ...) printf(COLOR_RED(m "\n"), ##__VA_ARGS__)
#define PRINT_RUNTIME_ERROR(m, ...) printf(COLOR_LIGHT_RED(m "\n"), ##__VA_ARGS__)
#define PRINT_WARNING(m, ...) printf(COLOR_YELLOW(m "\n"), ##__VA_ARGS__)
#define PRINT_OK(m, ...) printf(COLOR_GREEN(m "\n"), ##__VA_ARGS__)

#define TEST_RUN(func) do						\
	{								\
		ret_t ret_func;						\
		ret_func.id = RET_SUCCESS;				\
									\
		func(&ret_func);					\
		if(ret_func.id == RET_ERROR)				\
		{							\
			PRINT_ERROR("KO: %s (line:%d): %s",            \
                                    #func,                              \
                                    ret_func.line,                      \
                                    ret_func.msg);                      \
		}							\
		else							\
		{							\
			PRINT_OK("OK: %s",                              \
                                 #func);                                \
		}							\
		main_return |= ret_func.id;				\
	} while(0)

#define TEST_DEF(name) \
	static void name(ret_t* ret)

#define TEST_ASSERT(test, message, ...) do			\
	{							\
		if(!(test))					\
		{						\
			ret->id = RET_ERROR;			\
			ret->line = __LINE__;			\
			snprintf(ret->msg,			\
				 sizeof(ret->msg),		\
				 message, ##__VA_ARGS__);	\
			return;					\
		}						\
	} while(0)

#define TEST_INIT(name)					\
	int main_return = RET_SUCCESS;			\
	printf(COLOR_BLUE("@@ TEST " name " @@\n"))

#define TEST_RETURN main_return

#endif
