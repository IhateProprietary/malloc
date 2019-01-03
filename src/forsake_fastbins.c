/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forsake_fastbins.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:55:39 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:55:42 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_private.h"

void	forsake_fastbins(marena_t *arena)
{
	mchunk_t	*chunk;
	mchunk_t	*next;
	mchunk_t	*fd;
	int			idx;

	idx = 0;
	while (idx < NFASTBINS)
	{
		chunk = arena->fastbins[idx];
		while (chunk)
		{
			next = NEXTCHUNK(chunk);
			next = NEXTCHUNK(next);
			fd = chunk->fd;
			if (!PREVINUSE(chunk))
				chunk = consolidate_chunk(chunk);
			next = NEXTCHUNK(chunk);
			UNSETOPT(next, PREV_INUSE);
			next->prevsize = CHUNKSIZE(chunk);
			link_chunk(chunk, UNSORTED(arena));
			chunk = fd;
		}
		arena->fastbins[idx++] = (mchunk_t *)0;
	}
}
