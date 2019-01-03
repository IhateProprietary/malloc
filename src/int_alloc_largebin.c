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

mchunk_t	*alloc_largebin(marena_t *arena, size_t size)
{
	mchunk_t	*chunk;
	mchunk_t	*stop;
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
		return ((mchunk_t *)0);
	unlink_chunk(chunk);
	alloc_partial_chunk(chunk, size, UNSORTED(arena));
	link_chunk(chunk, USED_POOL(arena));
	return (chunk);
}
