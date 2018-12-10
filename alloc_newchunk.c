#include <stddef.h>
#include "malloc_private.h"

mchunk_t	*alloc_partial_chunk(marena_t *arena, mchunk_t *chunk, size_t size)
{
	mchunk_t	*adj;
	mchunk_t	*next;
	size_t		chunksize;
	size_t		nextsize;
	
	if ((adj = chunk->fd) == 0)
		adj->bk = chunk->bk;
	if ((adj = chunk->bk) == 0)
		adj->fd = chunk->fd;
	if (chunk->size == size)
		return (chunk);
	next = (char *)chunk + size;
	chunksize = chunk->size;
	nextsize = chunksize - size;
	if (nextsize < M_MINSIZE)
		return ((mchunk_t *)0);
	chunk->size = size;
	next->size = nextsize;
	adj = arena->unsorted_bin;
	arena->unsorted_bin = next;
	connect_chunk(next, adj);
	return (chunk);
}

void	connect_chunk(mchunk_t *new, mchunk_t *old)
{
	if (old)
	{
		new->fd = old;
		if (old->fd == old)
			old->fd = new;
		new->bk = old->bk;
		old->bk = new;
	}
	else
	{
		new->fd = new;
		new->bk = new;
	}
}

mchunk_t	*alloc_newchunk(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	size_t		remainder;

	remainder = (char *)arena + arena->size - arena->bottom;
	if (remainder < size)
		return ((mchunk_t *)0);
	arena->bottom += size;
	chunk->size = size;
	return (chunk);
}
