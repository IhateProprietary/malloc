#include "malloc.h"
#include <stdlib.h>
#include <stdio.h>

int		main()
{
	size_t alloced = 0;
	for (int i = 0;i < 1024;i++)
	{
		void *ptr = malloc(24);
		alloced += 32;
		void *abc = malloc(80);
		alloced += 96;
		void *c = malloc(160);
		alloced += 176;
		
		free(ptr);
		alloced -= 32;
		free(c);
		alloced -= 176;
		realloc(abc, 240);
		alloced += 176;
		printf("%p\n", ptr);
		printf("%p\n", abc);
		void *b = malloc(800);
		alloced += 816;
		printf("%p\n", b);
		printf("%d----------------\n", i);
	}
	printf("0x%lx\n", alloced);
}
