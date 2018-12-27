#include "malloc_private.h"

void	forsake_fastbins(marena_t *arena)
{
	/* mchunk_t	*chunk; */
	/* mchunk_t	*nc; */
	/* mchunk_t	*next; */
	/* int			idx; */

	/* idx = 0; */
	/* while (idx < NFASTBINS) */
	/* { */
	/* 	chunk = arena->fastbins[idx]; */
	/* 	while (chunk) */
	/* 	{ */
	/* 		next = NEXTCHUNK(next); */
			
	/* 		chunk = chunk->fd; */
	/* 	} */
	/* 	arena->fastbins[idx++] = (mchunk_t *)0; */
	/* } */
}
