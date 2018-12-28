#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include "malloc_private.h"
#include "malloc.h"
#include "ft_printf.h"
#include "libft.h"

# define N 1024
# define SZ 1024
# define RSZ 56
# define RN 256
# define CHARSET "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
# define MODULO	36

# define MSZ (MMAP_THRESHOLD)
# define RMSZ 8192

extern mstate_t mp;

int		main()
{
	void	*ptr[N];
	void	*ptr2 = 0;
	void	*ptr3 = 0;
	int fd = open("/dev/urandom", O_RDONLY);
	void *random;


	// SANITY CHECK MALLOC NORMAL
	for (int i = 0; i < N; i++)
	{
		ptr[i] = malloc(SZ);
		ft_memset(ptr[i], 0xff, N);
		ft_dprintf(2, "SANITY_CHECK %p %d 0 == good, 1 == bad\n",
				   MEM2CHUNK(ptr[i]), sanity_check(ptr[i]));
	}
	// RANDOM POINTER FREE TEST
	for (int i = 0; i < N; i++)
	{
		read(fd, &random, sizeof(random));
		ft_dprintf(2, "DID NOT SEGFAULT FOR %p TRY NUMBER %d\n", random, i);
		free(random);
	}
	// REALLOC TEST
	for (int i = 0, off = 0; i < RN; i++)
	{
		ptr2 = realloc(ptr2, RSZ + off);
		ft_memset(ptr2 + off, CHARSET[i % MODULO], RSZ);
		off += RSZ;
	}
	// RANDOM POINTER FREE TEST
	for (int i = 0; i < N; i++)
	{
		read(fd, &random, sizeof(random));
		ft_dprintf(2, "DID NOT SEGFAULT FOR %p TRY NUMBER %d\n", random, i);
		free(random);
	}
	for (int i = 0, off = 0; i < RN; i ++)
	{
		unsigned char bytes[RSZ];
		unsigned char test = CHARSET[i%MODULO];
		ft_memset(bytes, test, RSZ);
		ft_dprintf(2, "ptr %p + 0x%lx %d for %c\n",
				   ptr2, off, ft_memcmp(ptr2 + off, bytes, RSZ), test);
		off += RSZ;
	}
	// RANDOM POINTER FREE TEST
	for (int i = 0; i < N; i++)
	{
		read(fd, &random, sizeof(random));
		ft_dprintf(2, "DID NOT SEGFAULT FOR %p TRY NUMBER %d\n", random, i);
		free(random);
	}
	// FREE TEST
	free(ptr2);
	for (int i = 0; i < N; i++)
	{
		free(ptr[i]);
		ft_dprintf(2, "FREE CHECK %p sanity_check %d\n", ptr[i], sanity_check(ptr[i]));
	}
	// RANDOM POINTER FREE TEST
	for (int i = 0; i < N; i++)
	{
		read(fd, &random, sizeof(random));
		ft_dprintf(2, "DID NOT SEGFAULT FOR %p TRY NUMBER %d\n", random, i);
		free(random);
	}
	// BIG BIG MALLOC
	for (int i = 0; i < 10; i++)
	{
		void *ptr4 = malloc(MMAP_THRESHOLD);
		ft_dprintf(2, "SANITY CHECK BIG MALLOC %p %d\n", ptr, sanity_check(ptr4));
		free(ptr4);
	}
	show_alloc_mem();
}
