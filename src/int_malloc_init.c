#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "malloc_private.h"

static int	init_global_lock()
{
	if (pthread_mutex_trylock(&mp.global) == EBUSY)
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

static int	init_global_key()
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

void	int_malloc_init()
{
	marena_t	*arena;

	if (mp.malloc_init >= 1)
		return ;
	mp.malloc_init = 1;
	if (init_global_lock())
		return ;
	mp.pagesize = getpagesize();
	pthread_mutex_lock(&mp.global);
	if (init_global_key())
		return ;
	if ((arena = arena_new(HEAP_SIZE)) == (marena_t *)0)
	{
		mp.malloc_init = 2;
		pthread_key_delete(mp.tsd);
		pthread_mutex_unlock(&mp.global);
		pthread_mutex_destroy(&mp.global);
		return ;
	}
	pthread_setspecific(mp.tsd, arena);
	mp.arena = arena;
	mp.narena = 1;
	mp.malloc_init = 3;
	arena->prev = arena;
	pthread_mutex_unlock(&mp.global);
}
