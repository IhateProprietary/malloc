#include "malloc_private.h"

void	forsake_fastbins(marena_t *arena)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*fd;
	int			idx;

	idx = 0;
	while (idx < NFASTBINS)
	{
		chunk = arena->fastbins[idx];
		while (chunk)
		{
			next = NEXTCHUNK(chunk);
			next = NEXTCHUNK(next);
			fd = chunk->fd;
			if (!PREVINUSE(chunk))
				chunk = consolidate_chunk(arena, chunk);
			next = NEXTCHUNK(chunk);
			UNSETOPT(next, PREV_INUSE);
			next->prevsize = CHUNKSIZE(chunk);
			link_chunk(chunk, &arena->unsortedbin);
			chunk = fd;
		}
		arena->fastbins[idx++] = (mchunk_t *)0;
	}
}
