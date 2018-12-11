#include <pthread.h>
#include <unistd.h>
#include "malloc_private.h"

int		int_malloc_init()
{
	marena_t	*arena;

	if ((pthread_key_create(&mp.tsd, (void (*)(void *))0)) != 0)
	{
		mp.malloc_init = 1;
		return (1);
	}
	if ((arena = arena_new(HEAP_SIZE)) != (marena_t *)0)
	{
		mp.malloc_init = 1;
		pthread_key_delete(mp.tsd);
		return (1);
	}
	pthread_setspecific(mp.tsd, arena);
	mp.arena = arena;
	mp.mmap = 1;
	mp.narena = 1;
	mp.malloc_init = 2;
	mp.pagesize = sysconf(_SC_PAGESIZE);
	return (0);
}
