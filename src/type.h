/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 03:24:00 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:26:00 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPE_H
# define TYPE_H

typedef struct malloc_arena_s	marena_t;
typedef struct malloc_chunk_s	mchunk_t;
typedef struct malloc_state_s	mstate_t;

typedef mchunk_t*				bin_t;

typedef pthread_key_t			key_t;
typedef pthread_mutex_t			mutex_t;

struct							malloc_chunk_s
{
	size_t		prevsize;
	size_t		size;
	mchunk_t	*fd;
	mchunk_t	*bk;
};

struct							malloc_state_s
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

struct							malloc_arena_s
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
#endif
