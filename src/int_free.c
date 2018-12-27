#include <pthread.h>
#include "malloc_private.h"

mchunk_t	*consolidate_chunk(marena_t *arena, mchunk_t *chunk)
{
	mchunk_t	*prev;

	prev = PREVCHUNK(chunk);
	if (prev == arena->unsortedbin)
		unlink_chunk(prev, &arena->unsortedbin);
	else
		unlink_chunk(prev, &arena->bins[BIN_INDEX(CHUNKSIZE(prev))]);
	prev->size += CHUNKSIZE(chunk);
	return (prev);
}

void	insert_fastbin(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t *head;
	mchunk_t *next;

	head = *bin;
	*bin = chunk;
	chunk->fd = head;
	next = NEXTCHUNK(chunk);
	SETOPT(next, PREV_INUSE);
}

void	int_free(void *ptr)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	marena_t	*arena;
	size_t		size;

	chunk = MEM2CHUNK(ptr);
	arena = MEM2ARENA(ptr);
	//pthread_mutex___ BEGIN
	pthread_mutex_lock(&arena->mutex);
	unlink_chunk(chunk, &arena->pool);
	size = CHUNKSIZE(chunk);
	if (size <= FASTBIN_MAXSIZE)
	{
		insert_fastbin(chunk, &arena->fastbins[FASTBIN_INDEX(size)]);
		arena->fastbinsize += size;
		//pthread_mutex___ END
		pthread_mutex_unlock(&arena->mutex);
		return ;
	}
	next = NEXTCHUNK(chunk);
	if (!PREVINUSE(chunk))
		chunk = consolidate_chunk(arena, chunk);
	next->prevsize = CHUNKSIZE(chunk);
	UNSETOPT(next, PREV_INUSE);
	link_chunk(chunk, &arena->unsortedbin);
	//pthread_mutex____ END
	pthread_mutex_unlock(&arena->mutex);
}
