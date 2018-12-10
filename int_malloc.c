#include <stddef.h>
#include "malloc_private.h"

static mchunk_t	*alloc_fastbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	int			index;

	index = FASTBIN_INDEX(size);
	if (index >= NFASTBINS)
		return ((mchunk_t *)0);
	chunk = arena->fastbin[index];
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	arena->fastbin[index] = chunk->fd;
	return (chunk);
}

static mchunk_t	*alloc_smallbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	int			index;

	index = SMALLBIN_INDEX(size);
	chunk = arena->bins[index];
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	next = chunk->fd;
	next->bk = chunk->bk;
	arena->bins[index] = next;
	return (chunk);
}

static mchunk_t *alloc_largebin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*stop;
	int			index;

	index = LARGEBIN_INDEX(x);
	chunk = (mchunk_t *)0;
	while ((index < (NBINS - 2) &&
			chunk = arena->bins[index]) == (mchunk_t *)0)
		index++;
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	stop = chunk->bk;
	while (chunk->size < size && chunk != stop)
		chunk = chunk->next;
	if (chunk->size < size)
		return ((mchunk_t *)0);
	return (alloc_partial_chunk(arena, chunk, size));
}

void	*int_malloc(size_t size)
{
	size = REQ2SIZE(size);
	if (size >= DEFAULT_MMAP_TRESHOLD)
		return (CHUNK2MEM(alloc_mmap(size)));
	if ((chunk = alloc_fastbin(arena, size)))
		return (CHUNK2MEM(chunk));
	if ((chunk = try_unsortedbin(arena, size)))
		return (CHUNK2MEM(chunk));
	while (chunk == (void *)0)
	{
		if ((chunk = alloc_smallbin(arena, size)))
			return (CHUNK2MEM(chunk));
		if ((chunk = alloc_largebin(arena, size)))
			return (CHUNK2MEM(chunk));
		if ((chunk = alloc_newchunk(arena, size)))
			return (CHUNK2MEM(chunk));
		if (arena_grow(arena, size))
			break ;
	}
	return ((void *)0);
}
