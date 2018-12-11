#include <stddef.h>
#include "malloc_private.h"

void	set_link_chunk(mchunk_t *next, bin_t *connect)
{
	mchunk_t	*head;

	head = *connect;
	*connect = next;
	next->fd = head ? head : next;
	if (head && head->fd == head)
		head->fd = next;
	next->bk = head ? head->bk : next;
	if (head && head->bk == head)
		head->bk = next;
}

void	alloc_partial_chunk(mchunk_t *chunk, size_t size, bin_t *connect)
{
	mchunk_t	*next;
	size_t		chunksize;
	size_t		nextsize;

	if (chunk->size == size)
		return ;
	next = (mchunk_t *)(((char *)chunk) + size);
	chunksize = chunk->size;
	nextsize = chunksize - size;
	if (nextsize < M_MINSIZE)
		return ;
	chunk->size = size + (chunksize & SIZE_FLAGS);
	next->size = nextsize | SIZE_PREV_INUSE;
	if (connect)
		set_link_chunk(next, connect);
}

mchunk_t	*alloc_newchunk(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*nextchunk;
	size_t		remainder;

	chunk = arena->bottom;
	if ((chunk->size & ~SIZE_FLAGS) < size)
		return ((mchunk_t *)0);
	alloc_partial_chunk(chunk, size, (bin_t *)0);
	return (chunk);
}
