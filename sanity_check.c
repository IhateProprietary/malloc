#include <stddef.h>
#include "malloc_private.h"

int		sanity_check_pool(bin_t pool, mutex_t *mutex, void *mem)
{
	mchunk_t	*chunk;
	mchunk_t	*mchunk;
	mchunk_t	*stop;
	int			res;

	res = 1;
	pthread_mutex_lock(mutex);
	mchunk = MEM2CHUNK(mem);
	if ((chunk = pool) == (mchunk_t *)0)
		return (res);
	stop = chunk->bk;
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
			return (sanity_check_pool(arena->pool, &arena->mutex, mem));
		arena = arena->next;
	}
	return (sanity_check_pool(mp.pool, &mp.global, mem));
}
