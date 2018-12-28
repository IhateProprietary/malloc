#include <stddef.h>
#include "malloc_private.h"

static mchunk_t	*alloc_fastbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	int			index;

	if (size > FASTBIN_MAXSIZE)
		return ((mchunk_t *)0);
	index = FASTBIN_INDEX(size);
	chunk = arena->fastbins[index];
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	arena->fastbins[index] = chunk->fd;
	link_chunk(chunk, USED_POOL(arena));
	arena->fastbinsize -= size;
	return (chunk);
}

static mchunk_t	*alloc_smallbin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	int			idx;

	if (size >= LARGEBIN_MINSIZE)
		return ((mchunk_t *)0);
	idx = SMALLBIN_INDEX(size);
	while (idx < NSMALLBINS &&
		   ((chunk = BIN_AT(arena, idx)) == BIN_AT(arena, idx)->fd))
		idx++;
	chunk = chunk->fd;
	if (idx >= NSMALLBINS)
		return ((mchunk_t *)0);
	unlink_chunk(chunk);
	alloc_partial_chunk(chunk, size, UNSORTED(arena));
	link_chunk(chunk, USED_POOL(arena));
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
	chunk->size = mmapsize | IS_MAPPED;
	pthread_mutex_lock(&mp.global);
	link_chunk(chunk, &mp.pool);
	pthread_mutex_unlock(&mp.global);
	return (mem);
}

void	*int_malloc(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	size_t		alignsize;

	alignsize = REQ2SIZE(size, alignsize);
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
