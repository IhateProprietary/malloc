/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unsortedbin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:34:09 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:34:10 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_private.h"

static void	insert_chunk_bin(t_marena *arena, t_mchunk *chunk)
{
	int			idx;

	unlink_chunk(chunk);
	idx = BIN_INDEX(CHUNKSIZE(chunk));
	link_chunk(chunk, BIN_AT(arena, idx));
}

t_mchunk	*alloc_unsortedbin(t_marena *arena, size_t size)
{
	t_mchunk	*chunk;
	t_mchunk	*next;
	t_mchunk	*stop;

	if ((chunk = UNSORTED(arena)->fd) == UNSORTED(arena))
		return ((t_mchunk *)0);
	stop = UNSORTED(arena)->bk;
	while (chunk != stop)
	{
		if (CHUNKSIZE(chunk) >= size)
			break ;
		next = chunk->fd;
		insert_chunk_bin(arena, chunk);
		chunk = next;
	}
	if (CHUNKSIZE(chunk) >= size)
	{
		unlink_chunk(chunk);
		alloc_partial_chunk(chunk, size, UNSORTED(arena));
		link_chunk(chunk, USED_POOL(arena));
		SETNEXTOPT(chunk, PREV_INUSE);
		return (chunk);
	}
	insert_chunk_bin(arena, chunk);
	return ((t_mchunk *)0);
}
