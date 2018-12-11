#include <stddef.h>
#include "malloc_private.h"

static mchunk_t	*alloc_fastbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	int			index;

	index = FASTBIN_INDEX(size);
	if (index >= NFASTBINS)
		return ((mchunk_t *)0);
	chunk = arena->fastbins[index];
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	arena->fastbins[index] = chunk->fd;
	return (chunk);
}

static mchunk_t	*alloc_smallbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	int			index;

	if (size >= LARGEBIN_MINSIZE)
		return ((mchunk_t *)0);
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

	index = LARGEBIN_INDEX(size);
	chunk = (mchunk_t *)0;
	while (index < (NBINS - 2) &&
			((chunk = arena->bins[index]) == (mchunk_t *)0))
		index++;
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	stop = chunk->bk;
	while (chunk->size < size && chunk != stop)
		chunk = chunk->fd;
	if (chunk->size < size)
		return ((mchunk_t *)0);
	stop = chunk->fd;
	stop->bk = chunk->bk;
	if (chunk == arena->bins[index])
		arena->bins[index] = chunk->fd;
	if (chunk == arena->bins[index])
		arena->bins[index] = (mchunk_t *)0;
	alloc_partial_chunk(chunk, size, &arena->unsortedbin);
	return (chunk);
}

static mchunk_t	*alloc_mmap(size_t size)
{
	void		*mem;
	mchunk_t	*chunk;
	size_t		pagemask;
	size_t		mmapsize;

	pagemask = mp.pagesize - 1;
	mmapsize = (size + pagemask) & ~pagemask;
	if ((mem = NEW_HEAP(mmapsize)) == (void *)MAP_FAILED)
		return ((mchunk_t *)0);
	chunk = mem;
	chunk->size = mmapsize | SIZE_IS_MMAPED;
	return (mem);
}

void	*int_malloc(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	size_t		alignsize;

	alignsize = REQ2SIZE(size);
	if (alignsize >= MMAP_THRESHOLD)
	{
		if ((chunk = alloc_mmap(alignsize)))
			return (CHUNK2MEM(chunk));
		return ((void *)0);
	}
	if ((chunk = alloc_fastbin(arena, alignsize)))
		return (CHUNK2MEM(chunk));
	if ((chunk = alloc_unsortedbin(arena, alignsize)))
		return (CHUNK2MEM(chunk));
	if ((chunk = alloc_smallbin(arena, alignsize)))
		return (CHUNK2MEM(chunk));
	if ((chunk = alloc_largebin(arena, alignsize)))
		return (CHUNK2MEM(chunk));
	if ((chunk = alloc_newchunk(arena, alignsize)))
		return (CHUNK2MEM(chunk));
	return ((void *)0);
}
