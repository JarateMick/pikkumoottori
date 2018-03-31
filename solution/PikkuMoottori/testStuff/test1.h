#pragma once

#define TEST2


#if defined(TEST1)

typedef struct
{
	void* data;
} Hahaa;

#elif defined(TEST2)
typedef struct
{
	void* data;
} Hahaa;
#endif

#if defined (TEST1)
#include "test2.h"
#elif defined(TEST2)
#include "test3.h"
#endif



static void initAll()
{
	Hahaa* h = 0;
	Internal* a = 0;

	 internal_initialize(a, h);


	// Turn off the rightmost bit
	getchar();
	return 0;
}
