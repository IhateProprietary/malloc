#include "malloc_private.h"

static void	insert_chunk_bin(marena_t *arena, mchunk_t *chunk)
{
	int			idx;

	idx = BIN_INDEX(chunk->size);
	printf("INSERT %p to bin %d, size 0x%lx\n", chunk, idx, chunk->size);
	link_chunk(chunk, &arena->bins[idx]);
}

mchunk_t	*alloc_unsortedbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*stop;

	if ((chunk = arena->unsortedbin))
	{
		stop = chunk->bk;
		while (chunk != stop)
		{
			printf("try %p size %lu for %lu size\n", chunk, chunk->size, size);
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
			SETNEXTOPT(chunk, SIZE_PREV_INUSE);
			printf("UNSORTEDBIN chunk %p size %lu, unsortedbin %p\n",
				   chunk, chunk->size, arena->unsortedbin);
			return (chunk);
		}
		insert_chunk_bin(arena, chunk);
		arena->unsortedbin = (bin_t)0;
	}
	return ((mchunk_t *)0);
}
