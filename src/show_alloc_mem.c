/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:32:35 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:34:00 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "malloc_private.h"

void	dump_arena_pool(t_marena *arena)
{
	static char	*human_size[3] = {"TINY", "SMALL", "LARGE"};
	t_mchunk	*chunk;
	size_t		size;
	int			idx[2];
	void		*maxheap;

	pthread_mutex_lock(&arena->mutex);
	maxheap = (void *)((unsigned long)arena + HEAP_SIZE - M_MINSIZE);
	chunk = (t_mchunk *)arena->topmost;
	idx[1] = -1;
	while ((void *)chunk < maxheap)
	{
		size = CHUNKSIZE(chunk) - (8 * 3);
		idx[0] = size >= LARGEBIN_MINSIZE;
		idx[0] += size >= (MMAP_THRESHOLD >> 5);
		if (idx[0] != idx[1])
		{
			ft_printf("%s: %p\n", human_size[idx[0]], chunk);
			idx[1] = idx[0];
		}
		ft_printf("%p - %p : %lu bytes\n", CHUNK2MEM(chunk), NEXTCHUNK(chunk),
			size);
		chunk = NEXTCHUNK(chunk);
	}
	pthread_mutex_unlock(&arena->mutex);
}

void	dump_large_pool(void)
{
	t_mchunk	*stop;
	t_mchunk	*chunk;
	size_t		size;

	chunk = &mp.pool;
	stop = chunk;
	chunk = chunk->fd;
	while (stop != chunk)
	{
		size = CHUNKSIZE(chunk) - SIZE_SZ * 4;
		mp.used += size;
		ft_printf("LARGE: %p\n%p - %p : %lu bytes\n", chunk,
			CHUNK2MEM(chunk), NEXTCHUNK(chunk), size);
		if (chunk == stop)
			break ;
		chunk = chunk->fd;
	}
}

void	show_alloc_mem(void)
{
	t_marena	*arena;
	size_t		pagemask;
	void		*stop;

	pthread_mutex_lock(&mp.global);
	pagemask = mp.pagesize - 1;
	mp.used =
		mp.narena * (HEAP_SIZE - ((sizeof(t_marena) + pagemask) & ~pagemask));
	dump_large_pool();
	if (mp.arena)
	{
		arena = mp.arena->prev;
		stop = (void *)mp.arena;
		while (1)
		{
			dump_arena_pool(arena);
			if ((void *)arena == stop)
				break ;
			arena = arena->prev;
		}
	}
	ft_printf("Total : %lu\n", mp.used);
	pthread_mutex_unlock(&mp.global);
}
