#include "malloc_private.h"

void	forsake_fastbins(marena_t *arena)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*nextnext;
	int			idx;

	idx = 0;
	while (idx < NFASTBINS)
	{
		chunk = arena->fastbins[idx];
		while (chunk)
		{
			next = NEXTCHUNK(chunk);
			nextnext = NEXTCHUNK(next);
			if (!PREVINUSE(nextnext))
			{
				chunk = consolidate_chunk(arena, next);
				next = nextnext;
				link_chunk(chunk, &arena->unsortedbin);
			}
			UNSETOPT(next, SIZE_PREV_INUSE);
			next->prevsize = chunk->size;
			chunk = chunk->fd;
		}
		arena->fastbins[idx++] = (mchunk_t *)0;
	}
}
