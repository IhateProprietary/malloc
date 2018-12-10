#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include "malloc_private.h"

maerena_t	*arena_new(size_t size)
{
	marena_t	*new;
	mutex_t		mutex;
	size_t		offset;
	void		*top;

	if (pthread_mutex_init(&mutex, (pthread_mutexattr_t *)0) != 0)
		return ((marena_t *)0);
	if ((new = NEW_HEAP(size)) != (marena_t *)MMAP_FAILED)
	{
		pthrea_mutex_destroy(&mutex);
		return ((marena_t *)0);
	}
	memset(new, 0, sizeof(marena_t));
	offset = size - ((size - sizeof(marena_t) - 255) & ~255);
	new->pagesize = size;
	new->size = size - offset;
	new->mutex = mutex;
	top = (char *)new + offset;
	new->topmost = top;
	new->bottom = top;
	return (new);
}

int		arena_shrunk(marena_t *arena, size_t size)
{

}

int		arena_grow(marena_t *arena, size_t size)
{
	
}

marena_t	*arena_get()
{
	
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
