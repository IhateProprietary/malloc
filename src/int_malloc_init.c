/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_malloc_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 03:04:13 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:22:33 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "malloc_private.h"

static int	init_global_lock(void)
{
	if (pthread_t_mutexrylock(&mp.global) == EBUSY)
	{
		pthread_mutex_lock(&mp.global);
		pthread_mutex_unlock(&mp.global);
		return (1);
	}
	if (pthread_mutex_init(&mp.global, (pthread_mutexattr_t *)0) != 0)
	{
		mp.malloc_init = 2;
		return (1);
	}
	return (0);
}

static int	init_global_key(void)
{
	if ((pthread_key_create(&mp.tsd, (void (*)(void *))0)) != 0)
	{
		mp.malloc_init = 2;
		pthread_mutex_unlock(&mp.global);
		pthread_mutex_destroy(&mp.global);
		return (1);
	}
	return (0);
}

static void	init_global_data(t_marena *arena)
{
	mp.arena = arena;
	mp.pool.fd = &mp.pool;
	mp.pool.bk = &mp.pool;
	mp.narena = 1;
	mp.malloc_init = 3;
	arena->prev = arena;
}

void		int_malloc_init(void)
{
	t_marena	*arena;

	if (mp.malloc_init >= 1)
		return ;
	mp.malloc_init = 1;
	if (init_global_lock())
		return ;
	mp.pagesize = getpagesize();
	pthread_mutex_lock(&mp.global);
	if (init_global_key())
		return ;
	if ((arena = arena_new(HEAP_SIZE)) == (t_marena *)0)
	{
		mp.malloc_init = 2;
		pthread_key_delete(mp.tsd);
		pthread_mutex_unlock(&mp.global);
		pthread_mutex_destroy(&mp.global);
		return ;
	}
	pthread_setspecific(mp.tsd, arena);
	init_global_data(arena);
	pthread_mutex_unlock(&mp.global);
}
