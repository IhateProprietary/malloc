#include "malloc_private.h"

static void	insert_chunk_bin(marena_t *arena, mchunk_t *chunk)
{
	int			idx;

	unlink_chunk(chunk, &arena->unsortedbin);
	idx = BIN_INDEX(CHUNKSIZE(chunk));
	link_chunk(chunk, &arena->bins[idx]);
}

mchunk_t	*alloc_unsortedbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*stop;

	if ((chunk = arena->unsortedbin) == (mchunk_t *)0)
		return ((mchunk_t *)0);
	stop = chunk->bk;
	while (chunk != stop)
	{
		if (CHUNKSIZE(chunk) >= size)
			break ;
		next = chunk->fd;
		insert_chunk_bin(arena, chunk);
		chunk = next;
	}
	if (CHUNKSIZE(chunk) >= size)
	{
		unlink_chunk(chunk, &arena->unsortedbin);
		alloc_partial_chunk(chunk, size, &arena->unsortedbin);
		link_chunk(chunk, &arena->pool);
		SETNEXTOPT(chunk, PREV_INUSE);
		return (chunk);
	}
	insert_chunk_bin(arena, chunk);
	return ((mchunk_t *)0);
}
