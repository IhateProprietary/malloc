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

void		link_chunk(t_mchunk *chunk, t_mchunk *head)
{
	t_mchunk	*bk;
	t_mchunk	*fd;

	bk = head;
	fd = head->fd;
	chunk->fd = fd;
	chunk->bk = bk;
	fd->bk = chunk;
	bk->fd = chunk;
}

void		unlink_chunk(t_mchunk *chunk)
{
	t_mchunk *bk;
	t_mchunk *fd;

	bk = chunk->bk;
	fd = chunk->fd;
	bk->fd = fd;
	fd->bk = bk;
}

void		alloc_partial_chunk(t_mchunk *chunk, size_t size, t_bin connect)
{
	t_mchunk	*next;
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

t_mchunk	*alloc_newchunk(t_marena *arena, size_t size)
{
	t_mchunk	*chunk;

	chunk = (t_mchunk *)arena->bottom;
	if (CHUNKSIZE(chunk) < size)
		return ((t_mchunk *)0);
	alloc_partial_chunk(chunk, size, (t_bin)0);
	arena->bottom += CHUNKSIZE(chunk);
	link_chunk(chunk, USED_POOL(arena));
	return (chunk);
}
