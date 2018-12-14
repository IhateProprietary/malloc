#include "malloc.h"
#include <stdio.h>

int		main()
{
	for (int i = 0;i < 20;i++)
	{
		void *ptr = malloc(24);
		void *abc = malloc(80);
		void *c = malloc(80);
		
		free(ptr);
		free(abc);
		free(c);
		printf("%p\n", ptr);
		printf("%p\n", abc);
		void *b = malloc(160);
		printf("%p\n", b);
		printf("%d----------------\n", i);
	}
}
