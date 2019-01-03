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

int		sanity_check_pool(t_mchunk *pool, t_mutex *mutex, void *mem)
{
	t_mchunk	*chunk;
	t_mchunk	*mchunk;
	t_mchunk	*stop;
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
	t_marena	*arena;

	if ((unsigned long)mem & M_ALIGN_MASK)
		return (1);
	arena = g_mp.arena;
	orig = MEM2ARENA(mem);
	while (arena)
	{
		if ((void *)arena == orig)
			return (sanity_check_pool(&arena->pool, &arena->mutex, mem));
		arena = arena->next;
	}
	return (sanity_check_pool(&g_mp.pool, &g_mp.global, mem));
}
