#include "malloc.h"
#include "malloc_private.h"
#include <stdio.h>
#define N 900

extern mstate_t mp;

int		main()
{
	void *ptr[N];
	for (int i = 0;i < N;i++)
	{
		ptr[i] = malloc(1024);
		printf("%p\n", ptr[i]);
	}
	for (int i = 0; i < N;i++)
		free(ptr[i]);
	for (int i= 0; i < N; i++)
	{
		ptr[i] = malloc(1024);
		printf("%p\n", ptr[i]);
	}
	marena_t *a = mp.arena;
	mchunk_t *chunk = a->pool, *stop = a->pool->bk;
	dprintf(2, "test___ arena %p\n", mp.arena->pool);
	dprintf(2, "test___ arena %p %p,%p\n", chunk, stop, chunk->bk);
	for (; chunk != stop; chunk = chunk->fd)
		dprintf(2, "%p\n", CHUNK2MEM(chunk));
//	show_alloc_mem();
}
