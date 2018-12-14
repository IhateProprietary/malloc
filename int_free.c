#include <pthread.h>
#include "malloc_private.h"

mchunk_t	*consolidate_chunk(marena_t *arena, mchunk_t *chunk)
{
	mchunk_t	*prev;
	mchunk_t	*tmp;
	int			idx;

	prev = (mchunk_t *)((unsigned long)chunk - chunk->prevsize);
	printf("CONSOLIDATE chunk1 %p size 0x%lx prev_chunk %p size 0x%lx\n", chunk, chunk->size, prev, prev->size);
	idx = BIN_INDEX(prev->size);
	unlink_chunk(prev, &arena->bins[idx]);
	prev->size += chunk->size;
	return (prev);
}

void	insert_fastbin(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t *head;

	head = *bin;
	*bin = chunk;
	chunk->fd = head;
	printf("PUT FASTBIN %p\n", chunk);
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
	size = CHUNKSIZE(chunk);
	printf("free chunk %p size 0x%lx\n", chunk, size);
	if (size <= FASTBIN_MAXSIZE)
	{
		insert_fastbin(chunk, &arena->fastbins[FASTBIN_INDEX(size)]);
		pthread_mutex_unlock(&arena->mutex);
		return ;
	}
	next = (mchunk_t *)((unsigned long)chunk + size);
	if (!PREVINUSE(chunk))
		chunk = consolidate_chunk(arena, chunk);
	printf("next CLEAR BIT SIZE_PREV_INUSE %p\n", next);
	next->prevsize = CHUNKSIZE(chunk);
	UNSETOPT(next, SIZE_PREV_INUSE);
	link_chunk(chunk, &arena->unsortedbin);
	printf("next %p prevsize 0x%lx\n", next, next->prevsize);
	printf("MUTEX UNLOCK -- chunk %p fd %p bk %p\n", chunk, chunk->fd, chunk->bk);
	pthread_mutex_unlock(&arena->mutex);
}
