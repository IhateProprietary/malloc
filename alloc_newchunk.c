#include <stddef.h>
#include "malloc_private.h"

void	link_chunk(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t	*head;

	head = *bin;
	if (head == chunk)
		return ;
	*bin = chunk;
	chunk->fd = TER(head, head, chunk);
	chunk->bk = TER(head, head->bk, chunk);
	if (head && head->fd == head)
	{
		head->fd = chunk;
		head->bk = chunk;
	}
}

void	unlink_chunk(mchunk_t *chunk, bin_t *bin)
{
	mchunk_t *head;
	mchunk_t *tmp;

	head = *bin;
	if (chunk == head)
	{
		head = chunk->fd;
		if ((*bin = head) == chunk)
		{
			*bin = (mchunk_t *)0;
			return ;
		}
	}
	tmp = chunk->fd;
	tmp->bk = chunk->bk;
	if (tmp->fd == chunk)
		tmp->fd = tmp;
}

void	alloc_partial_chunk(mchunk_t *chunk, size_t size, bin_t *connect)
{
	mchunk_t	*next;
	size_t		chunksize;
	size_t		nextsize;

	if (chunk->size == size)
		return ;
	next = (mchunk_t *)((unsigned long)chunk + size);
	chunksize = chunk->size;
	nextsize = UCHUNKSIZE(chunksize) - size;
	if (nextsize < M_MINSIZE)
	{
		printf("NODIVIDE chunk %p size 0x%lx req 0x%lx\n", chunk, chunk->size, size);
		next = (mchunk_t *)((unsigned long)chunk + UCHUNKSIZE(chunksize));
		if ((unsigned long)next & (HEAP_SIZE - 1))
		{
			next->size |= SIZE_PREV_INUSE;
			printf("next %p size 0x%lx\n", next, next->size);
		}
		return ;
	}
	printf("DIVIDE chunk %p from 0x%lx to 0x%lx\n", chunk, chunk->size, size);
	chunk->size = size + UCHUNKFLAGS(chunksize);
	next->size = nextsize | SIZE_PREV_INUSE;
	printf("REMAIN CHUNK %p size 0x%lx\n", next, next->size);
	if (connect)
		link_chunk(next, connect);
}

mchunk_t	*alloc_newchunk(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;

	chunk = arena->bottom;
	if (CHUNKSIZE(chunk) < size)
		return ((mchunk_t *)0);
	alloc_partial_chunk(chunk, size, (bin_t *)0);
	arena->bottom += size;
	printf("NEWCHUNK %lu %p\n", size, chunk);
	return (chunk);
}
