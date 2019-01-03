/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_alloc_largebin.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:56:11 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:56:11 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_private.h"

t_mchunk	*alloc_largebin(t_marena *arena, size_t size)
{
	t_mchunk	*chunk;
	t_mchunk	*stop;
	int			idx;

	idx = LARGEBIN_INDEX(size);
	while (idx < NBINS)
	{
		while (idx < (NBINS) && (chunk = BIN_AT(arena, idx))
			== BIN_AT(arena, idx)->fd)
			idx++;
		stop = chunk->bk;
		chunk = chunk->fd;
		while (chunk != stop && CHUNKSIZE(chunk) < size)
			chunk = chunk->fd;
		if (CHUNKSIZE(chunk) >= size)
			break ;
		idx++;
	}
	if (idx >= NBINS)
		return ((t_mchunk *)0);
	unlink_chunk(chunk);
	alloc_partial_chunk(chunk, size, UNSORTED(arena));
	link_chunk(chunk, USED_POOL(arena));
	return (chunk);
}
