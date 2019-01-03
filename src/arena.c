/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:31:30 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:32:31 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include "libft.h"
#include "malloc_private.h"

static void	*mmap_aligned_heap(void)
{
	void	*p1;
	void	*p2;
	size_t	remainder;
	size_t	align;

	if ((p1 = NEW_HEAP(HEAP_SIZE << 1)) == (void *)MAP_FAILED)
		return ((void *)MAP_FAILED);
	align = -((unsigned long)p1) & (HEAP_SIZE - 1);
	p2 = p1 + align;
	remainder = HEAP_SIZE - align;
	munmap(p1, align);
	munmap(p2 + HEAP_SIZE, remainder);
	return (p2);
}

void		arena_init_bin(t_marena *arena)
{
	t_mchunk	*chunk;
	int			idx;

	idx = 1;
	while (idx < NBINS)
	{
		chunk = BIN_AT(arena, idx);
		chunk->fd = chunk;
		chunk->bk = chunk;
		idx++;
	}
	chunk = USED_POOL(arena);
	chunk->fd = chunk;
	chunk->bk = chunk;
}

t_marena	*arena_new(void)
{
	t_mutex		mutex;
	t_marena	*new;
	void		*top;
	size_t		offset;
	size_t		pagemask;

	if (pthread_mutex_init(&mutex, (pthread_mutexattr_t *)0) != 0)
		return ((t_marena *)0);
	if ((new = (t_marena *)mmap_aligned_heap()) == (t_marena *)MAP_FAILED)
	{
		pthread_mutex_destroy(&mutex);
		return ((t_marena *)0);
	}
	ft_memset(new, 0, sizeof(t_marena));
	ft_memcpy(&new->mutex, &mutex, sizeof(mutex));
	pagemask = mp.pagesize - 1;
	top = (void *)((char *)new + ((sizeof(t_marena) + pagemask) & ~pagemask));
	offset = (unsigned long)top - (unsigned long)new;
	new->topmost = top;
	new->bottom = top;
	mp.narena += 1;
	((t_mchunk *)top)->size = (HEAP_SIZE - offset - M_MINSIZE) | PREV_INUSE;
	arena_init_bin(new);
	return (new);
}

t_marena	*arena_get(void)
{
	t_marena	*arena;

	arena = (t_marena *)pthread_getspecific(mp.tsd);
	if (!arena || pthread_mutex_trylock(&arena->mutex) != 0)
	{
		arena = mp.arena;
		while (arena)
		{
			if (pthread_mutex_trylock(&arena->mutex) == 0)
				break ;
			arena = arena->next;
		}
		pthread_setspecific(mp.tsd, (void *)arena);
	}
	if (arena)
		pthread_mutex_unlock(&arena->mutex);
	else
	{
		arena = arena_new();
		pthread_setspecific(mp.tsd, (void *)arena);
	}
	return (arena);
}
