/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 03:24:00 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 03:29:49 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPE_H
# define TYPE_H

# define NFASTBINS	10
# define NSMALLBINS	64
# define NBINS		128

typedef struct malloc_arena_s	t_marena;
typedef struct malloc_chunk_s	t_mchunk;
typedef struct malloc_state_s	t_mstate;

typedef t_mchunk*				t_bin;

typedef pthread_key_t			t_key;
typedef pthread_mutex_t			t_mutex;

struct							s_malloc_chunk
{
	size_t		prevsize;
	size_t		size;
	t_mchunk	*fd;
	t_mchunk	*bk;
};

struct							s_malloc_state
{
	t_marena	*arena;
	size_t		narena;
	size_t		used;
	size_t		pagesize;
	t_mchunk	pool;
	t_mutex		global;
	key_t		tsd;
	int			malloc_init;
};

struct							s_malloc_arena
{
	t_marena	*next;
	t_marena	*prev;
	t_mutex		mutex;
	size_t		fastbinsize;
	t_bin		fastbins[NFASTBINS];
	t_bin		bins[NBINS * 2];
	t_mchunk	pool;
	void		*topmost;
	void		*bottom;
};

#endif
