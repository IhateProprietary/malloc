#include "malloc_private.h"

static void	insert_chunk_bin(marena_t *arena, mchunk_t *chunk)
{
	int			idx;

	idx = BIN_INDEX(chunk->size);
	set_link_chunk(chunk, &arena->bins[idx]);
}

mchunk_t	*alloc_unsortedbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*stop;

	chunk = arena->unsortedbin;
	stop = chunk->bk;
	while (chunk != stop)
	{
		if (chunk->size == size)
			break ;
		next = chunk->fd;
		insert_chunk_bin(arena, chunk);
		chunk = next;
	}
	if (chunk->size == size)
	{
		next = chunk->fd;
		next->bk = stop;
		stop->fd = next;
		return (chunk);
	}
	insert_chunk_bin(arena, chunk);
	arena->unsortedbin = (bin_t)0;
	return ((mchunk_t *)0);
}
