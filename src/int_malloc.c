/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_malloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 03:21:23 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:23:02 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "malloc_private.h"

static t_mchunk	*alloc_fastbin(t_marena *arena, size_t size)
{
	t_mchunk	*chunk;
	int			index;

	if (size > FASTBIN_MAXSIZE)
		return ((t_mchunk *)0);
	index = FASTBIN_INDEX(size);
	chunk = arena->fastbins[index];
	if (chunk == (t_mchunk *)0)
		return ((t_mchunk *)0);
	arena->fastbins[index] = chunk->fd;
	link_chunk(chunk, USED_POOL(arena));
	arena->fastbinsize -= size;
	return (chunk);
}

static t_mchunk	*alloc_smallbin(t_marena *arena, size_t size)
{
	t_mchunk	*chunk;
	int			idx;

	if (size >= LARGEBIN_MINSIZE)
		return ((t_mchunk *)0);
	idx = SMALLBIN_INDEX(size);
	while (idx < NSMALLBINS &&
		((chunk = BIN_AT(arena, idx)) == BIN_AT(arena, idx)->fd))
		idx++;
	chunk = chunk->fd;
	if (idx >= NSMALLBINS)
		return ((t_mchunk *)0);
	unlink_chunk(chunk);
	alloc_partial_chunk(chunk, size, UNSORTED(arena));
	link_chunk(chunk, USED_POOL(arena));
	return (chunk);
}

static t_mchunk	*alloc_mmap(size_t size)
{
	void		*mem;
	t_mchunk	*chunk;
	size_t		pagemask;
	size_t		mmapsize;

	pagemask = g_mp.pagesize - 1;
	mmapsize = (size + pagemask) & ~pagemask;
	if ((mem = NEW_HEAP(mmapsize)) == (void *)MAP_FAILED)
		return ((t_mchunk *)0);
	chunk = mem;
	chunk->size = mmapsize | IS_MAPPED;
	pthread_mutex_lock(&g_mp.global);
	link_chunk(chunk, &g_mp.pool);
	pthread_mutex_unlock(&g_mp.global);
	return (mem);
}

void			*int_malloc(t_marena *arena, size_t size)
{
	t_mchunk	*chunk;
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
