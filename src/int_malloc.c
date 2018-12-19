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
	printf("FASTBIN %lu\n", size);
	link_chunk(chunk, &arena->pool);
	printf("arena->pool %p\n", arena->pool);
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
	while (index < (NSMALLBINS) &&
		   ((chunk = arena->bins[index]) == (mchunk_t *)0))
		index++;
	if (chunk == (mchunk_t *)0)
		return ((mchunk_t *)0);
	next = chunk->fd;
	next->bk = chunk->bk;
	if (next->fd == chunk)
		next->fd = next;
	if (chunk == arena->bins[index])
		arena->bins[index] = chunk->fd;
	if (chunk == arena->bins[index])
		arena->bins[index] = (mchunk_t *)0;
	alloc_partial_chunk(chunk, size, &arena->unsortedbin);
	printf("SMALLBIN %lu idx %d\n", size, index);
	link_chunk(chunk, &arena->pool);
	return (chunk);
}

static mchunk_t	*alloc_mmap(size_t size)
{
	void		*mem;
	mchunk_t	*chunk;
	size_t		pagemask;
	size_t		mmapsize;


	printf("MMAP %lu\n", size);
	pagemask = mp.pagesize - 1;
	mmapsize = (size + pagemask) & ~pagemask;
	if ((mem = NEW_HEAP(mmapsize)) == (void *)MAP_FAILED)
		return ((mchunk_t *)0);
	chunk = mem;
	chunk->size = mmapsize | SIZE_IS_MAPPED;
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
	printf("arena %p alloc 0x%lx\n", arena, alignsize);
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
