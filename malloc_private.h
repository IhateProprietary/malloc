#ifndef __MALLOC_PRIVATE__
# define __MALLOC_PRIVATE__

# include <stddef.h>
# include <stdint.h>
# include <sys/mman.h>
# include <unistd.h>
# include <pthread.h>

# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#  define MAP_ANONYMOUS MAP_ANON
# endif
# if !defined(MAP_ANONYMOUS)
#  error "implementation cannot work without MAP_ANONYMOUS"
# endif
# if !defined(MAP_NORESERVE) && defined(MAP_NOCACHE)
#  define MAP_NORESERVE MAP_NOCACHE
# endif
# define SIZE_PREV_INUSE	0x1
# define SIZE_IS_MMAPED		0x2
# define SIZE_FLAGS			(SIZE_PREV_INUSE | SIZE_IS_MMAPED)

# define MMAP_FLAGS (MAP_ANONYMOUS | MAP_PRIVATE)
# define MPROT_FLAGS (PROT_WRITE | PROT_READ)
# define MMAP(a, s, prot, flags) mmap(a, s, prot, flags, -1, 0)
# define NEW_HEAP(s) MMAP(0, s, MPROT_FLAGS, MMAP_FLAGS)

# define HEAP_SIZE			(1024 * 1024)
# define REMAP_SIZE			(64 *1024)
# define REMAP_SIZE_MASK	(REMAP_SIZE - 1)
# define MMAP_THRESHOLD		(128 * 1024)
# define TRIM_SIZE			(128 * 1024)

# define M_ALIGNEMENT		0x10
# define M_MINSIZE			(sizeof(mchunk_t))
# define M_ALIGN_MASK		(M_ALIGNEMENT - 1)
# define SIZE_SZ			__SIZEOF_SIZE_T__
# define CHUNK2MEM(ptr)		(((void *)ptr) + SIZE_SZ * 2)
# define MEM2CHUNK(ptr)		(((void *)ptr) - SIZE_SZ * 2)
# define REQCHECK(x)		((x) < M_MINSIZE)
# define REQALIGN(x)		((x) + SIZE_SZ + M_ALIGN_MASK & ~(M_ALIGN_MASK))
# define REQ2SIZE(req)		(REQCHECK(req) ? M_MINSIZE : REQALIGN(req))

# define ARENA_SHOULD_TRIM			0x1
# define ARENA_SHOULD_CONSOLIDATE	0x2

# define NFASTBINS	10
# define NSMALLBINS	64
# define NBINS		128

# define LARGEBIN_MINSIZE (NSMALLBINS << 3)

# define FASTBIN_INDEX(x) ((x) >> 3)
# define SMALLBIN_INDEX(x) (FASTBIN_INDEX(x) - 1)
# define LARGEBIN1_INDEX(x) (((x) >> 6) + NSMALLBINS - 1)
# define LARGEBIN2_INDEX(x) (((x) >> 9) + NSMALLBINS + 31)
# define LARGEBIN3_INDEX(x) (((x) >> 12) + NSMALLBINS + 47)
# define LARGEBIN4_INDEX(x) (((x) >> 15) + NSMALLBINS + 55)
# define LARGEBIN5_INDEX(x) (((x) >> 18) + NSMALLBINS + 59)

# define TER(cond, if_, else_) ((cond) ? (if_) : (else_))

# define BIN_INDEX1(x) TER(x < (64 << 3), SMALLBIN_INDEX(x), BIN_INDEX2(x))
# define BIN_INDEX2(x) TER(((x) >> 6) <= 32, LARGEBIN1_INDEX(x), BIN_INDEX3(x))
# define BIN_INDEX3(x) TER(((x) >> 9) <= 16, LARGEBIN2_INDEX(x), BIN_INDEX4(x))
# define BIN_INDEX4(x) TER(((x) >> 12) <= 8, LARGEBIN3_INDEX(x), BIN_INDEX5(x))
# define BIN_INDEX5(x) TER(((x) >> 15) <= 4, LARGEBIN4_INDEX(x), BIN_INDEX6(x))
# define BIN_INDEX6(x) LARGEBIN5_INDEX(x)
# define LARGEBIN_INDEX(x) BIN_INDEX2(x)
# define BIN_INDEX(x) BIN_INDEX1(x)

typedef struct malloc_arena_s	marena_t;
typedef struct malloc_chunk_s	mchunk_t;
typedef struct malloc_state_s	mstate_t;

typedef mchunk_t*	bin_t;

typedef pthread_key_t	key_t;
typedef pthread_mutex_t	mutex_t;

extern mstate_t	mp;

struct	malloc_state_s
{
	marena_t	*arena;
	size_t		trylock;
	size_t		lock;
	size_t		unlock;
	size_t		malloc;
	size_t		free;
	size_t		mmap;
	size_t		munmap;
	size_t		narena;
	size_t		used;
	size_t		pagesize;
	key_t		tsd;
	int			malloc_init;
};

struct	malloc_arena_s
{
	struct malloc_arena_s	*next;
	mutex_t	mutex;
	size_t	pagesize;
	size_t	size;
	size_t	used;
	size_t	unused;
	size_t	flags;
	bin_t	fastbins[NFASTBINS];
	bin_t	bins[NBINS - 2];
	bin_t	unsortedbin;
	void	*topmost;
	void	*bottom;
};

struct	malloc_chunk_s
{
	size_t		prev_size;
	size_t		size;
	mchunk_t	*fd;
	mchunk_t	*bk;
};

marena_t	*arena_new(size_t size);
int			arena_shrunk(marena_t *arena, size_t size);
int			arena_grow(marena_t *arena, size_t size);
marena_t	*arena_get();

int			int_malloc_init();
void		*int_malloc(marena_t *arena, size_t size);

void		alloc_partial_chunk(mchunk_t *chunk, size_t size, bin_t *connect);
void		set_link_chunk(mchunk_t *next, bin_t *connect);
mchunk_t	*alloc_newchunk(marena_t *arena, size_t size);

mchunk_t	*alloc_unsortedbin(marena_t *arena, size_t size);

#endif
