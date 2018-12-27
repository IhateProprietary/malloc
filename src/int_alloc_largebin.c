#include "malloc_private.h"

mchunk_t *alloc_largebin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*stop;
	int			index;

	index = LARGEBIN_INDEX(size);
	while (1)
	{
		while (index < (NBINS) &&
			   ((chunk = arena->bins[index]) == (mchunk_t *)0))
			index++;
		if (chunk == (mchunk_t *)0)
			return ((mchunk_t *)0);
		stop = chunk->bk;
		while (chunk->size < size && chunk != stop)
			chunk = chunk->fd;
		if (chunk->size >= size)
			break ;
		index++;
	}
	if (chunk->size < size)
		return ((mchunk_t *)0);
	unlink_chunk(chunk, &arena->bins[index]);
	alloc_partial_chunk(chunk, size, &arena->unsortedbin);
	link_chunk(chunk, &arena->pool);
	return (chunk);
}
