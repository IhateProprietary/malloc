#include <string.h>
#include <pthread.h>
#include "malloc_private.h"

mstate_t	mp;

void	*malloc(size_t size)
{
	marena_t	*arena;
	void		*victim;

	if (mp.malloc_init < 1)
		int_malloc_init();
	arena = get_arena();
	if (arena == (marena_t *)0)
		return ((void *)0);
	mutex_lock(arena->mutex);
	victim = int_malloc(size);
	mutex_unlock(arena->mutex);
	return (victim);
}

void	free(void *mem)
{

}

void	*realloc(void *mem, size_t size)
{
	if (mem == (void *)0)
		return (malloc(size));	
}

/*
** The calloc() function contiguously allocates enough space for count objects that are size bytes of memory each and returns a pointer to the allocated memory.
** The allocated memory is filled with bytes of value zero.
*/

void	calloc(size_t count, size_t size)
{
	size_t	total;
	void	*victim;

	total = count * size;
	if ((victim = malloc(total)) == (void *)0)
		return ((void *)0);
	return (memset(victim, 0, total));
}

void	reallocf(void *mem, size_t size)
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
