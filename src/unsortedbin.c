#include "malloc_private.h"

static void	insert_chunk_bin(marena_t *arena, mchunk_t *chunk)
{
	int			idx;

	unlink_chunk(chunk);
	idx = BIN_INDEX(CHUNKSIZE(chunk));
	link_chunk(chunk, BIN_AT(arena, idx));
}

mchunk_t	*alloc_unsortedbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*stop;

	if ((chunk = UNSORTED(arena)) == UNSORTED(arena)->fd)
		return ((mchunk_t *)0);
	stop = chunk->bk;
	chunk = chunk->fd;
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
		unlink_chunk(chunk);
		alloc_partial_chunk(chunk, size, UNSORTED(arena));
		link_chunk(chunk, USED_POOL(arena));
		SETNEXTOPT(chunk, PREV_INUSE);
		return (chunk);
	}
	insert_chunk_bin(arena, chunk);
	return ((mchunk_t *)0);
}
