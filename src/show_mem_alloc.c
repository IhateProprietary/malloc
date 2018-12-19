#include <stdio.h>
#include "malloc_private.h"

void	dump_arena_pool(marena_t *arena)
{
	static char	*human_size[3] = {"TINY", "SMALL", "LARGE"};
	mchunk_t	*chunk;
	size_t		size;
	int			idx[2];
	void		*maxheap;

	pthread_mutex_lock(&arena->mutex);
	maxheap = (void *)((unsigned long)arena + HEAP_SIZE);
	chunk = (mchunk_t *)arena->topmost;
	idx[1] = -1;
	while ((void *)chunk < maxheap)
	{
		size = CHUNKSIZE(chunk) - (SIZE_SZ * 3);
		idx[0] = size >= LARGEBIN_MINSIZE;
		idx[0] += size >= (MMAP_THRESHOLD >> 5);
		if (idx[0] != idx[1])
		{
			printf("%s: %p\n", human_size[idx[0]], chunk);
			idx[1] = idx[0];
		}
		printf("%p - %p : %lu bytes\n", CHUNK2MEM(chunk), NEXTCHUNK(chunk),
			   size);
		chunk = NEXTCHUNK(chunk);
	}
	pthread_mutex_unlock(&arena->mutex);
}

void	dump_large_pool()
{
	mchunk_t	*stop;
	mchunk_t	*chunk;
	size_t		size;

	chunk = mp.pool;
	stop = chunk->bk;
	while (1)
	{
		size = CHUNKSIZE(chunk) - SIZE_SZ * 4;
		mp.used += size;
		printf("LARGE: %p\n%p - %p : %lu bytes\n", chunk,
			   CHUNK2MEM(chunk), NEXTCHUNK(chunk), size);
		if (chunk == stop)
			break ;
		chunk = chunk->fd;
	}
}

void	show_alloc_mem(void)
{
	marena_t	*arena;
	size_t		pagemask;
	void		*stop;

	pthread_mutex_lock(&mp.global);
	pagemask = mp.pagesize - 1;
	mp.used =
		mp.narena * (HEAP_SIZE - ((sizeof(marena_t) + pagemask) & ~pagemask));
	if (mp.pool)
		dump_large_pool();
	if (mp.arena)
	{
		arena = mp.arena->prev;
		stop = (void *)mp.arena;
		while (1)
		{
			dump_arena_pool(arena);
			if ((void *)arena == stop)
				break ;
			arena = arena->prev;
		}
	}
	printf("Total : %lu\n", mp.used);
	pthread_mutex_unlock(&mp.global);
}
