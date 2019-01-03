/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sanity_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:34:22 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:34:24 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include "malloc_private.h"

int		sanity_check_pool(mchunk_t *pool, mutex_t *mutex, void *mem)
{
	mchunk_t	*chunk;
	mchunk_t	*mchunk;
	mchunk_t	*stop;
	int			res;

	res = 1;
	pthread_mutex_lock(mutex);
	mchunk = MEM2CHUNK(mem);
	if ((chunk = pool) == pool->fd)
	{
		pthread_mutex_unlock(mutex);
		return (res);
	}
	stop = chunk->bk;
	chunk = chunk->fd;
	while (res)
	{
		res = chunk != mchunk;
		if (chunk == stop)
			break ;
		chunk = chunk->fd;
	}
	pthread_mutex_unlock(mutex);
	return (res);
}

int		sanity_check(void *mem)
{
	void		*orig;
	marena_t	*arena;

	if ((unsigned long)mem & M_ALIGN_MASK)
		return (1);
	arena = mp.arena;
	orig = MEM2ARENA(mem);
	while (arena)
	{
		if ((void *)arena == orig)
			return (sanity_check_pool(&arena->pool, &arena->mutex, mem));
		arena = arena->next;
	}
	return (sanity_check_pool(&mp.pool, &mp.global, mem));
}
