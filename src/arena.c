#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include "malloc_private.h"

static void	*mmap_aligned_heap()
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

marena_t	*arena_new()
{
	mutex_t		mutex;
	marena_t	*new;
	void		*top;
	size_t		offset;
	size_t		pagemask;

	if (pthread_mutex_init(&mutex, (pthread_mutexattr_t *)0) != 0)
		return ((marena_t *)0);
	if ((new = (marena_t *)mmap_aligned_heap()) == (marena_t *)MAP_FAILED)
	{
		pthread_mutex_destroy(&mutex);
		return ((marena_t *)0);
	}
	memset(new, 0, sizeof(marena_t));
	memcpy(&new->mutex, &mutex, sizeof(mutex));
	pagemask = mp.pagesize - 1;
	top = (void *)((char *)new + ((sizeof(marena_t) + pagemask) & ~pagemask));
	offset = (unsigned long)top - (unsigned long)new;
	new->topmost = top;
	new->bottom = top;
	mp.narena += 1;
	((mchunk_t *)top)->size = (HEAP_SIZE - offset) | SIZE_PREV_INUSE;
	return (new);
}

marena_t	*arena_get()
{
	marena_t	*arena;

	arena = (marena_t *)pthread_getspecific(mp.tsd);
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
