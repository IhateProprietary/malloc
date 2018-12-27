#include <stddef.h>
#include "malloc_private.h"

void	link_chunk(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t	*head;
	mchunk_t	*bk;
	mchunk_t	*fd;

	head = *bin;
	*bin = chunk;
	if (head)
	{
		bk = head->bk;
		fd = head;
	}
	else
	{
		bk = chunk;
		fd = chunk;
	}
	chunk->fd = fd;
	chunk->bk = bk;
	fd->bk = chunk;
	bk->fd = chunk;
}

void	unlink_chunk(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t *head;
	mchunk_t *bk;
	mchunk_t *fd;

	head = *bin;
	if (chunk == head)
	{
		head = chunk->fd;
		if (head == chunk)
		{
			*bin = (mchunk_t *)0;
			return ;
		}
		*bin = head;
	}
	bk = chunk->bk;
	fd = chunk->fd;
	bk->fd = fd;
	fd->bk = bk;
}

void	alloc_partial_chunk(mchunk_t *chunk, size_t size, bin_t *connect)
{
	mchunk_t	*next;
	size_t		chunksize;
	size_t		nextsize;

	chunksize = CHUNKSIZE(chunk);
	nextsize = chunksize - size;
	next = NEXTCHUNK(chunk);
	if (nextsize < M_MINSIZE)
	{
		next->size |= PREV_INUSE;
		return ;
	}
	next->prevsize = nextsize;
	chunk->size = size + CHUNKFLAGS(chunk);
	next = NEXTCHUNK(chunk);
	next->size = nextsize | PREV_INUSE;
	if (connect)
		link_chunk(next, connect);
}

mchunk_t	*alloc_newchunk(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;

	chunk = (mchunk_t *)arena->bottom;
	if (CHUNKSIZE(chunk) < size)
		return ((mchunk_t *)0);
	alloc_partial_chunk(chunk, size, (bin_t *)0);
	arena->bottom += size;
	link_chunk(chunk, &arena->pool);
	return (chunk);
}
