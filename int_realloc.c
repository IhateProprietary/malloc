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
	printf("CHUNK GROWWWWWWWWWWWW next == %p\n", next);
	while ((void *)next < heapmax && !PREVINUSE(next))
	{
		prev = PREVCHUNK(next);
		printf("CHUNK %p AVAILABLE size 0x%lx next %p\n", prev, prev->size, next);
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
	arena = MEM2ARENA(mem);
	alignsize = REQ2SIZE(size, alignsize);
	printf("TRY REALLOC MEM %p SIZE 0x%lx for size 0x%lx %lu\n", chunk, chunk->size, alignsize, alignsize);
	pthread_mutex_lock(&arena->mutex);
	chunksize = CHUNKSIZE(chunk);
	if (chunksize < alignsize)
		grow_chunk(arena, chunk, alignsize);
	chunksize = CHUNKSIZE(chunk);
	printf("ASSERT REALLOC -> %d current chunk size 0x%lx\n", chunksize < size, chunksize);
	if (chunksize < size)
	{
		pthread_mutex_unlock(&arena->mutex);
		return ((void *)0);
	}
	alloc_partial_chunk(chunk, alignsize, &arena->unsortedbin);
	pthread_mutex_unlock(&arena->mutex);
	return (CHUNK2MEM(chunk));
}
