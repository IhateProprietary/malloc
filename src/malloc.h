#ifndef __MALLOC_H__
# define __MALLOC_H__

# ifndef size_t
#  include <stddef.h>
# endif

void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
void	*calloc(size_t count, size_t size);
void	*reallocf(void *ptr, size_t size);

#endif
