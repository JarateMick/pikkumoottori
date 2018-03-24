#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "test1.h"

typedef struct 
{
	void* bigmem;
} Internal;

static int internal_initialize(Internal* i, Hahaa* h)
{
	printf("test3.h\n");
	// h->data = malloc(4);
}