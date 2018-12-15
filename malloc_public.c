#include <pthread.h>
#include "malloc_private.h"

mstate_t	mp;

static void *malloc2(size_t size)
{
	marena_t	*arena;
	void		*victim;

	victim = (void *)0;
	arena = mp.arena;
	pthread_mutex_lock(&mp.global);
	while (arena && victim == (void *)0)
	{
		pthread_mutex_lock(&arena->mutex);
		victim = int_malloc(arena, size);
		pthread_mutex_unlock(&arena->mutex);
		arena = arena->next;
	}
	if (victim == (void *)0 && (arena = arena_new()) != (marena_t *)0)
	{
		victim = int_malloc(arena, size);
		arena->next = mp.arena;
		mp.arena = arena;
	}
	if (arena)
		pthread_setspecific(mp.tsd, (void *)arena);
	pthread_mutex_unlock(&mp.global);
	return (victim);
}

void	*malloc(size_t size)
{
	marena_t	*arena;
	void		*victim;

	if (mp.malloc_init < 1)
		int_malloc_init();
	arena = arena_get();
	if (arena == (marena_t *)0)
		return ((void *)0);
	pthread_mutex_lock(&arena->mutex);
	victim = int_malloc(arena, size);
	pthread_mutex_unlock(&arena->mutex);
	if (victim == (void *)0)
		victim = malloc2(size);
	return (victim);
}

void	free(void *mem)
{
	mchunk_t	*chunk;

	if (mem == (void *)0)
		return ;
	chunk = MEM2CHUNK(mem);
	if (chunk->size & SIZE_IS_MAPPED)
		munmap((void *)chunk, chunk->size);
	int_free(mem);
}

void	*realloc(void *mem, size_t size)
{
	void		*victim;
	mchunk_t	*chunk;

	if (mem == (void *)0)
		return (malloc(size));
	victim = int_realloc(mem, size);
	if (victim == (void *)0 && (victim = malloc(size)))
	{
		chunk = MEM2CHUNK(mem);
		memcpy(mem, victim, MIN(chunk->size, REQ2SIZE(size)) - SIZE_SZ);
		free(mem);
	}
	return (victim);
}

/*
** The calloc() function contiguously allocates enough space for count objects that are size bytes of memory each and returns a pointer to the allocated memory.
** The allocated memory is filled with bytes of value zero.
*/

void	*calloc(size_t count, size_t size)
{
	size_t	total;
	void	*victim;

	total = count * size;
	if ((victim = malloc(total)) == (void *)0)
		return ((void *)0);
	return (memset(victim, 0, total));
}

void	*reallocf(void *mem, size_t size)
{
	void	*victim;

	victim = realloc(mem, size);
	if (victim == (void *)0)
	{
		free(mem);
		return ((void *)0);
	}
	return (victim);
}
