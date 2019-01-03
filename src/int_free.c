/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_free.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 03:05:01 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:21:16 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "malloc_private.h"

t_mchunk	*consolidate_chunk(t_mchunk *chunk)
{
	t_mchunk	*prev;

	prev = PREVCHUNK(chunk);
	unlink_chunk(prev);
	prev->size += CHUNKSIZE(chunk);
	return (prev);
}

void		insert_fastbin(t_mchunk *chunk, t_bin *bin)
{
	t_mchunk *head;
	t_mchunk *next;

	head = *bin;
	*bin = chunk;
	chunk->fd = head;
	next = NEXTCHUNK(chunk);
	SETOPT(next, PREV_INUSE);
}

void		int_free(void *ptr)
{
	t_mchunk	*chunk;
	t_mchunk	*next;
	t_marena	*arena;
	size_t		size;

	chunk = MEM2CHUNK(ptr);
	arena = MEM2ARENA(ptr);
	pthread_mutex_lock(&arena->mutex);
	unlink_chunk(chunk);
	size = CHUNKSIZE(chunk);
	if (size <= FASTBIN_MAXSIZE)
	{
		insert_fastbin(chunk, &arena->fastbins[FASTBIN_INDEX(size)]);
		arena->fastbinsize += size;
		pthread_mutex_unlock(&arena->mutex);
		return ;
	}
	next = NEXTCHUNK(chunk);
	if (!PREVINUSE(chunk))
		chunk = consolidate_chunk(chunk);
	next->prevsize = CHUNKSIZE(chunk);
	UNSETOPT(next, PREV_INUSE);
	link_chunk(chunk, UNSORTED(arena));
	pthread_mutex_unlock(&arena->mutex);
}
