#include "malloc_private.h"

static inline void alloc_largebin2(marena_t *arena, mchunk_t *chunk, int index)
{
	mchunk_t	*next;

	next = chunk->fd;
	next->bk = chunk->bk;
	if (next->fd == chunk)
		next->fd = next;
	if (chunk == arena->bins[index])
		arena->bins[index] = chunk->fd;
	if (chunk == arena->bins[index])
		arena->bins[index] = (mchunk_t *)0;
}

mchunk_t *alloc_largebin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*stop;
	int			index;

	index = LARGEBIN_INDEX(size);
	while (1)
	{
		while (index < (NBINS - 1) &&
			   ((chunk = arena->bins[index]) == (mchunk_t *)0))
			index++;
		if (chunk == (mchunk_t *)0)
			return ((mchunk_t *)0);
		stop = chunk->bk;
		while (chunk->size < size && chunk != stop)
			chunk = chunk->fd;
		if (chunk->size >= size)
			break ;
	}
	alloc_largebin2(arena, chunk, index);
	alloc_partial_chunk(chunk, size, &arena->unsortedbin);
	return (chunk);
}
