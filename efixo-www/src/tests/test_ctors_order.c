#include <stdio.h>

static void __attribute__((constructor)) __init1()
{
	printf("init1\n");
}

static void __attribute__((constructor)) __init2b()
{
	printf("init2b\n");
}

static void __attribute__((constructor)) __init2a()
{
	printf("init2a\n");
}

static void __attribute__((destructor)) __fini1()
{
	printf("fini1\n");
}

static void __attribute__((destructor)) __fini2b()
{
	printf("fini2b\n");
}

int main()
{
	printf("main\n");

	return 0;
}

static void __attribute__((destructor)) __fini3()
{
	printf("fini3\n");
}

static void __attribute__((destructor)) __fini2a()
{
	printf("fini2a\n");
}

static void __attribute__((constructor)) __init3()
{
	printf("init3\n");
}
