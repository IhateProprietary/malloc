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

void	forsake_fastbins(t_marena *arena)
{
	t_mchunk	*chunk;
	t_mchunk	*next;
	t_mchunk	*fd;
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
		arena->fastbins[idx++] = (t_mchunk *)0;
	}
}
