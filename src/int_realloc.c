/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_realloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:54:40 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:54:42 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "malloc_private.h"

void	grow_chunk(marena_t *arena, mchunk_t *chunk, size_t size)
{
	mchunk_t	*next;
	mchunk_t	*prev;
	void		*heapmax;
	size_t		cursize;

	next = NEXTCHUNK(chunk);
	next = NEXTCHUNK(next);
	heapmax = (void *)((unsigned long)arena + HEAP_SIZE - M_MINSIZE);
	cursize = CHUNKSIZE(chunk);
	while ((void *)next < heapmax && !PREVINUSE(next))
	{
		prev = PREVCHUNK(next);
		unlink_chunk(prev);
		cursize += CHUNKSIZE(prev);
		if (cursize >= size)
			break ;
		next = NEXTCHUNK(next);
	}
	chunk->size = cursize + CHUNKFLAGS(chunk);
	next = NEXTCHUNK(chunk);
	SETOPT(next, PREV_INUSE);
}

void	*int_realloc(void *mem, size_t size)
{
	mchunk_t	*chunk;
	marena_t	*arena;
	size_t		chunksize;
	size_t		alignsize;

	chunk = MEM2CHUNK(mem);
	if (CHUNKMAPPED(chunk))
		return ((void *)0);
	arena = MEM2ARENA(mem);
	alignsize = REQ2SIZE(size, alignsize);
	if (alignsize >= MMAP_THRESHOLD)
		return ((void *)0);
	pthread_mutex_lock(&arena->mutex);
	chunksize = CHUNKSIZE(chunk);
	if (chunksize < alignsize)
		grow_chunk(arena, chunk, alignsize);
	chunksize = CHUNKSIZE(chunk);
	if (chunksize < alignsize)
	{
		pthread_mutex_unlock(&arena->mutex);
		return ((void *)0);
	}
	alloc_partial_chunk(chunk, alignsize, UNSORTED(arena));
	pthread_mutex_unlock(&arena->mutex);
	return (CHUNK2MEM(chunk));
}
