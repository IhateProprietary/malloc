/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc_newchunk.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:31:24 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:31:45 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "malloc_private.h"

void		link_chunk(mchunk_t *chunk, mchunk_t *head)
{
	mchunk_t	*bk;
	mchunk_t	*fd;

	bk = head;
	fd = head->fd;
	chunk->fd = fd;
	chunk->bk = bk;
	fd->bk = chunk;
	bk->fd = chunk;
}

void		unlink_chunk(mchunk_t *chunk)
{
	mchunk_t *bk;
	mchunk_t *fd;

	bk = chunk->bk;
	fd = chunk->fd;
	bk->fd = fd;
	fd->bk = bk;
}

void		alloc_partial_chunk(mchunk_t *chunk, size_t size, bin_t connect)
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
	UNSETOPT(next, PREV_INUSE);
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
	alloc_partial_chunk(chunk, size, (bin_t)0);
	arena->bottom += CHUNKSIZE(chunk);
	link_chunk(chunk, USED_POOL(arena));
	return (chunk);
}
