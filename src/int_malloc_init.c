/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_malloc_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 03:04:13 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:32:56 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "malloc_private.h"

static int	init_global_lock(void)
{
	if (pthread_mutex_trylock(&g_mp.global) == EBUSY)
	{
		pthread_mutex_lock(&g_mp.global);
		pthread_mutex_unlock(&g_mp.global);
		return (1);
	}
	if (pthread_mutex_init(&g_mp.global, (pthread_mutexattr_t *)0) != 0)
	{
		g_mp.malloc_init = 2;
		return (1);
	}
	return (0);
}

static int	init_global_key(void)
{
	if ((pthread_key_create(&g_mp.tsd, (void (*)(void *))0)) != 0)
	{
		g_mp.malloc_init = 2;
		pthread_mutex_unlock(&g_mp.global);
		pthread_mutex_destroy(&g_mp.global);
		return (1);
	}
	return (0);
}

static void	init_global_data(t_marena *arena)
{
	g_mp.arena = arena;
	g_mp.pool.fd = &g_mp.pool;
	g_mp.pool.bk = &g_mp.pool;
	g_mp.narena = 1;
	g_mp.malloc_init = 3;
	arena->prev = arena;
}

void		int_malloc_init(void)
{
	t_marena	*arena;

	if (g_mp.malloc_init >= 1)
		return ;
	g_mp.malloc_init = 1;
	if (init_global_lock())
		return ;
	g_mp.pagesize = getpagesize();
	pthread_mutex_lock(&g_mp.global);
	if (init_global_key())
		return ;
	if ((arena = arena_new(HEAP_SIZE)) == (t_marena *)0)
	{
		g_mp.malloc_init = 2;
		pthread_key_delete(g_mp.tsd);
		pthread_mutex_unlock(&g_mp.global);
		pthread_mutex_destroy(&g_mp.global);
		return ;
	}
	pthread_setspecific(g_mp.tsd, arena);
	init_global_data(arena);
	pthread_mutex_unlock(&g_mp.global);
}
