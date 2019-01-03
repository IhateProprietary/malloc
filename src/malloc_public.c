/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_public.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:34:35 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 05:17:08 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <unistd.h>
#include "libft.h"
#include "malloc_private.h"

t_mstate	g_mp;

static void	*malloc2(size_t size)
{
	t_marena	*arena;
	void		*victim;

	victim = (void *)0;
	arena = g_mp.arena;
	while (arena && victim == (void *)0)
	{
		pthread_mutex_lock(&arena->mutex);
		victim = int_malloc(arena, size);
		pthread_mutex_unlock(&arena->mutex);
		arena = arena->next;
	}
	pthread_mutex_lock(&g_mp.global);
	if (victim == (void *)0 && (arena = arena_new()) != (t_marena *)0)
	{
		victim = int_malloc(arena, size);
		arena->next = g_mp.arena;
		arena->prev = g_mp.arena->prev;
		g_mp.arena->prev = arena;
		g_mp.arena = arena;
	}
	pthread_mutex_unlock(&g_mp.global);
	if (arena)
		pthread_setspecific(g_mp.tsd, (void *)arena);
	return (victim);
}

void		*malloc(size_t size)
{
	t_marena	*arena;
	void		*victim;

	if (g_mp.malloc_init < 1)
		int_malloc_init();
	arena = arena_get();
	if (arena == (t_marena *)0)
		return ((void *)0);
	pthread_mutex_lock(&arena->mutex);
	victim = int_malloc(arena, size);
	if (victim == (void *)0 && arena->fastbinsize >= FASTBIN_MAXMEM)
	{
		forsake_fastbins(arena);
		arena->fastbinsize = 0;
		victim = int_malloc(arena, size);
	}
	pthread_mutex_unlock(&arena->mutex);
	if (victim == (void *)0)
		victim = malloc2(size);
	return (victim);
}

void		free(void *mem)
{
	t_mchunk	*chunk;

	if (mem == (void *)0 || sanity_check(mem))
		return ;
	chunk = MEM2CHUNK(mem);
	if (CHUNKMAPPED(chunk))
	{
		pthread_mutex_lock(&g_mp.global);
		unlink_chunk(chunk);
		munmap((void *)chunk, CHUNKSIZE(chunk));
		pthread_mutex_unlock(&g_mp.global);
		return ;
	}
	int_free(mem);
}

void		*realloc(void *mem, size_t size)
{
	void		*victim;
	t_mchunk	*chunk;
	size_t		chunksize;

	if (mem == (void *)0)
		return (malloc(size));
	if (sanity_check(mem))
		return ((void *)0);
	if (size == 0)
	{
		free(mem);
		return ((void *)0);
	}
	victim = int_realloc(mem, size);
	if (victim == (void *)0 && (victim = malloc(size)))
	{
		chunk = MEM2CHUNK(mem);
		chunksize = CHUNKSIZE(chunk);
		chunksize -= SIZE_SZ * (3 + CHUNKMAPPED(chunk));
		ft_memcpy(victim, mem, MIN(chunksize, size));
		free(mem);
	}
	return (victim);
}

/*
** The calloc() function contiguously allocates enough space
** for count objects that are size bytes of memory each and returns
** a pointer to the allocated memory.
** The allocated memory is filled with bytes of value zero.
*/

void		*calloc(size_t count, size_t size)
{
	size_t	total;
	void	*victim;

	total = count * size;
	if ((victim = malloc(total)) == (void *)0)
		return ((void *)0);
	return (ft_memset(victim, 0, total));
}
