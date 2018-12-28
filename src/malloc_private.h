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
# define PREV_INUSE		0x1
# define IS_MAPPED		0x2
# define SIZE_FLAGS		(PREV_INUSE | IS_MAPPED)
# define CHUNKFLAGS(x)	((x)->size & SIZE_FLAGS)
# define CHUNKSIZE(x)	((x)->size & ~SIZE_FLAGS)
# define PREVINUSE(c)	((c)->size & PREV_INUSE)
# define CHUNKMAPPED(c)	((c)->size & IS_MAPPED)

# define SETOPT(x, opt)		((x)->size |= (opt))
# define UNSETOPT(x, opt)	((x)->size &= ~(opt))
# define SETNEXTOPT(x, opt)		(SETOPT((NEXTCHUNK(x)), (opt)))
# define UNSETNEXTOPT(x, opt)	(UNSETOPT((NEXTCHUNK(x)), (opt)))
# define NEXTCHUNK(x)	((mchunk_t *)((unsigned long)(x) + CHUNKSIZE(x)))
# define PREVCHUNK(x)	((mchunk_t *)((unsigned long)(x) - (x)->prevsize))

# define MMAP_FLAGS (MAP_ANONYMOUS | MAP_PRIVATE)
# define MPROT_FLAGS (PROT_WRITE | PROT_READ)
# define MMAP(a, s, prot, flags) (mmap(a, s, prot, flags, -1, 0))
# define NEW_HEAP(s) (MMAP(0, s, MPROT_FLAGS, MMAP_FLAGS))

# define HEAP_SIZE			(1024 * 1024)
# define MMAP_THRESHOLD		(128 * 1024)
# define TRIM_SIZE			(128 * 1024)

# define M_ALIGNEMENT	(SIZE_SZ << 1)
# define M_MINSIZE		(sizeof(mchunk_t))
# define M_ALIGN_MASK	(M_ALIGNEMENT - 1)
# define SIZE_SZ		__SIZEOF_SIZE_T__
# define CHUNK2MEM(ptr)	(((void *)(ptr)) + SIZE_SZ * 4)
# define MEM2CHUNK(ptr)	(((void *)(ptr)) - SIZE_SZ * 4)
# define MEM2ARENA(ptr)	((void *)((unsigned long)(ptr) & ~(HEAP_SIZE - 1)))
# define O_	(SIZE_SZ << 1)
# define BIN_AT(a, i)	((bin_t)((char *)&((a)->bins[(i) << 1]) - O_))
# define UNSORTED(a)	(BIN_AT((a), 1))
# define USED_POOL(a)	((mchunk_t *)&(a)->pool)
# define REQCHECK(x)	((x) < M_MINSIZE)
# define REQALIGN(x)	(((x) + (SIZE_SZ * 3) + M_ALIGN_MASK) & ~(M_ALIGN_MASK))
# define REQ2SIZE(req, nb)	(REQCHECK((nb = REQALIGN(req))) ? M_MINSIZE : nb)

# define NFASTBINS	10
# define NSMALLBINS	64
# define NBINS		128

# define FASTBIN_MAXSIZE	(8 * NFASTBINS)
# define FASTBIN_MAXMEM		(64 * 1024)
# define LARGEBIN_MINSIZE	(NSMALLBINS << 3)

# define FASTBIN_INDEX(x) ((x) >> 3)
# define SMALLBIN_INDEX(x) (FASTBIN_INDEX(x))
# define LARGEBIN1_INDEX(x) (((x) >> 6) + 56)
# define LARGEBIN2_INDEX(x) (((x) >> 9) + 91)
# define LARGEBIN3_INDEX(x) (((x) >> 12) + 110)
# define LARGEBIN4_INDEX(x) (((x) >> 15) + 119)
# define LARGEBIN5_INDEX(x) (((x) >> 18) + 124)

# define TER(cond, if_, else_) ((cond) ? (if_) : (else_))

# define MIN(x, y) TER(x < y, x, y)

# define BIN_INDEX1(x) TER(x < (64 << 3), SMALLBIN_INDEX(x), BIN_INDEX2(x))
# define BIN_INDEX2(x) TER(((x) >> 6) <= 32, LARGEBIN1_INDEX(x), BIN_INDEX3(x))
# define BIN_INDEX3(x) TER(((x) >> 9) <= 20, LARGEBIN2_INDEX(x), BIN_INDEX4(x))
# define BIN_INDEX4(x) TER(((x) >> 12) <= 10, LARGEBIN3_INDEX(x), BIN_INDEX5(x))
# define BIN_INDEX5(x) TER(((x) >> 15) <= 4, LARGEBIN4_INDEX(x), BIN_INDEX6(x))
# define BIN_INDEX6(x) TER(((x) >> 18) <= 2, LARGEBIN5_INDEX(x), 126)
# define LARGEBIN_INDEX(x) BIN_INDEX2(x)
# define BIN_INDEX(x) BIN_INDEX1(x)

typedef struct malloc_arena_s	marena_t;
typedef struct malloc_chunk_s	mchunk_t;
typedef struct malloc_state_s	mstate_t;

typedef mchunk_t*	bin_t;

typedef pthread_key_t	key_t;
typedef pthread_mutex_t	mutex_t;

extern mstate_t	mp;

struct	malloc_chunk_s
{
	size_t		prevsize;
	size_t		size;
	mchunk_t	*fd;
	mchunk_t	*bk;
};

struct	malloc_state_s
{
	marena_t	*arena;
	size_t		narena;
	size_t		used;
	size_t		pagesize;
	mchunk_t	pool;
	mutex_t		global;
	key_t		tsd;
	int			malloc_init;
};

struct	malloc_arena_s
{
	marena_t	*next;
	marena_t	*prev;
	mutex_t		mutex;
	size_t		fastbinsize;
	bin_t		fastbins[NFASTBINS];
	bin_t		bins[NBINS * 2];
	mchunk_t	pool;
	void		*topmost;
	void		*bottom;
};

marena_t	*arena_new();
int			arena_shrunk(marena_t *arena, size_t size);
int			arena_grow(marena_t *arena, size_t size);
marena_t	*arena_get();

void		int_malloc_init();
void		*int_malloc(marena_t *arena, size_t size);
void		int_free(void *ptr);
void		*int_realloc(void *ptr, size_t size);

void		alloc_partial_chunk(mchunk_t *chunk, size_t size, bin_t connect);
void		link_chunk(mchunk_t *chunk, mchunk_t *head);
void		unlink_chunk(mchunk_t *chunk);
mchunk_t	*alloc_largebin(marena_t *arena, size_t size);
mchunk_t	*alloc_newchunk(marena_t *arena, size_t size);

mchunk_t	*alloc_unsortedbin(marena_t *arena, size_t size);
mchunk_t	*consolidate_chunk(mchunk_t *chunk);
void		forsake_fastbins(marena_t *arena);
int			sanity_check(void *mem);

#endif
