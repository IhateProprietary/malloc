#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include "malloc_private.h"

marena_t	*arena_new(size_t size)
{
	void		*mem;
	marena_t	*new;
	mutex_t		mutex;
	void		*top;

	if (pthread_mutex_init(&mutex, (pthread_mutexattr_t *)0) != 0)
		return ((marena_t *)0);
	if ((mem = NEW_HEAP(HEAP_SIZE)) == (void *)MAP_FAILED)
	{
		pthread_mutex_destroy(&mutex);
		return ((marena_t *)0);
	}
	memset(new, 0, sizeof(marena_t));
	new->size = size;
	new->mutex = mutex;
	top = (void *)((char *)(new + ((sizeof(marena_t) + 255) & ~255)));
	new->topmost = top;
	new->bottom = top;
	((mchunk_t *)top)->size =
		(size - ((size_t)top - (size_t)new)) | SIZE_PREV_INUSE;
	return (new);
}

marena_t	*arena_get()
{
	marena_t	*arena;

	arena = pthread_getspecific(mp.tsd);
	if (pthread_mutex_trylock(&arena->mutex) != 0)
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
	pthread_mutex_unlock(&arena->mutex);
	return (arena);
}

// all of this shit for get arena thanks very much

// pthread_mutex_unlock
// pthread_mutex_lock
// pthread_mutex_trylock
// pthread_key_craete
// pthread_key_delete
// pthread_setspecific
// pthread_getspecific
// pthread_self
