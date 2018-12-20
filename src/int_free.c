#include <pthread.h>
#include "malloc_private.h"

mchunk_t	*consolidate_chunk(marena_t *arena, mchunk_t *chunk)
{
	mchunk_t	*prev;

	prev = PREVCHUNK(chunk);
	if (chunk == arena->unsortedbin)
		unlink_chunk(chunk, &arena->unsortedbin);
	else
		unlink_chunk(chunk, &arena->bins[BIN_INDEX(prev->size)]);
	prev->size += chunk->size;
	return (prev);
}

void	insert_fastbin(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t *head;

	head = *bin;
	*bin = chunk;
	chunk->fd = head;
}

void	int_free(void *ptr)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	marena_t	*arena;
	size_t		size;

	chunk = MEM2CHUNK(ptr);
	arena = MEM2ARENA(ptr);
	pthread_mutex_lock(&arena->mutex);
	unlink_chunk(chunk, &arena->pool);
	size = CHUNKSIZE(chunk);
	if (size <= FASTBIN_MAXSIZE)
	{
		insert_fastbin(chunk, &arena->fastbins[FASTBIN_INDEX(size)]);
		arena->fastbinsize += size;
		pthread_mutex_unlock(&arena->mutex);
		return ;
	}
	next = NEXTCHUNK(chunk);
	if (!PREVINUSE(chunk))
		chunk = consolidate_chunk(arena, chunk);
	next->prevsize = CHUNKSIZE(chunk);
	UNSETOPT(next, SIZE_PREV_INUSE);
	link_chunk(chunk, &arena->unsortedbin);
	pthread_mutex_unlock(&arena->mutex);
}
