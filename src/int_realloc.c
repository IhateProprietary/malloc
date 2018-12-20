#include <stddef.h>
#include "malloc_private.h"

void	grow_chunk(marena_t *arena, mchunk_t *chunk, size_t size)
{
	mchunk_t	*next;
	mchunk_t	*prev;
	void		*heapmax;
	size_t		cursize;

	next = NEXTCHUNK(chunk);
	next = NEXTCHUNK(next);
	heapmax = (void *)((unsigned long)arena + HEAP_SIZE);
	cursize = CHUNKSIZE(chunk);
	while ((void *)next < heapmax && !PREVINUSE(next))
	{
		prev = PREVCHUNK(next);
		if (prev == arena->unsortedbin)
			unlink_chunk(prev, &arena->unsortedbin);
		else
			unlink_chunk(prev, &arena->bins[BIN_INDEX(prev->size)]);
		cursize += CHUNKSIZE(prev);
		SETOPT(next, SIZE_PREV_INUSE);
		if (cursize >= size)
			break ;
		next = NEXTCHUNK(next);
	}
	chunk->size = cursize + CHUNKFLAGS(chunk);
}

void	*int_realloc(void *mem, size_t size)
{
	mchunk_t	*chunk;
	marena_t	*arena;
	size_t		chunksize;
	size_t		alignsize;

	chunk = MEM2CHUNK(mem);
	if (CHUNKMAPPED(chunk))
		return ((void *)0);
	arena = MEM2ARENA(mem);
	alignsize = REQ2SIZE(size, alignsize);
	pthread_mutex_lock(&arena->mutex);
	chunksize = CHUNKSIZE(chunk);
	if (chunksize < alignsize)
		grow_chunk(arena, chunk, alignsize);
	chunksize = CHUNKSIZE(chunk);
	if (chunksize < size)
	{
		pthread_mutex_unlock(&arena->mutex);
		return ((void *)0);
	}
	alloc_partial_chunk(chunk, alignsize, &arena->unsortedbin);
	pthread_mutex_unlock(&arena->mutex);
	return (CHUNK2MEM(chunk));
}
