#ifndef __MALLOC_H__
# define __MALLOC_H__

# include <stddef.h>
# include <unistd.h>

# define MALLOC_PAGE_ALIGN		sysconf(_SC_PAGESIZE)
# define MALLOC_PRIVATE_ZONE	MALLOC_PAGE_ALIGN
# define MALLOC_SMALL_SIZE 		(131072 + MALLOC_PRIVATE_ZONE)
# define MALLOC_ALIGN_SIZE		(MALLOC_PRIVATE_ZONE + MALLOC_PAGE_ALIGN)

# define SIZE_NEEDS_ALIGN(x)	((x) & (MALLOC_PAGE_ALIGN - 1))
# define MALLOC_ALIGN(x)		((x) & ~(MALLOC_PAGE_ALIGN - 1) + MALLOC_ALIGN_SIZE)
# define MALLOC_THRESHOLD		131072 // should be aligned with _SC_PAGESIZE
# define SIZE_EXCEEDED_THRESHOLD(x) ((x) >= MALLOC_THRESHOLD)

# define MALLOC_MINIMUM_ALLOC	(16)
# define MALLOC_ADDR_ALIGN		(MALLOC_MINIMUM_ALLOC - 1)
# define MALLOC_SIZEOF_SLICE	(sizeof(malloc_slice_t))
# define MALLOC_MINIMUM_ZONE	(MALLOC_MINIMUM_ALLOC + MALLOC_SLICE_SIZE)

# define MALLOC_PAGE_SMALL		0x0
# define MALLOC_PAGE_LARGE  	0x1
# define MALLOC_PAGE_DEFRAG		0x2

# define MALLOC_SLICE_BUSY		0x0
# define MALLOC_SLICE_AVAILABLE	0x1
# define MALLOC_SLICE_LARGER	0x2

typedef struct	malloc_page_s
{
	struct malloc_page_s	*prev_page;
	struct malloc_page_s	*next_page;
	void					*free;
	void					*tail_zone;
	size_t					size;
	int						flags;
	int						tail;
	int						max_alloc;
	int						busy_slice;
	int						free_slice;
	int						max_free;
}				malloc_page_t;

typedef struct	malloc_page_stat_s
{
	malloc_page_t	*large;
	malloc_page_t	*small;
	malloc_page_t	*drop;
	size_t			allocated;
}				malloc_page_stat_t;

typedef struct	malloc_slice_s
{
	uint64_t	flags:4;
	uint64_t	size:15;
	int64_t		next_free:15;
	int64_t		prev_free:15;
	uint64_t	page:15;
}				malloc_slice_t;

void 	*malloc(size_t mem_size);
void	free(void *mem);

#endif
