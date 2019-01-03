/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_public2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 02:50:22 by jye               #+#    #+#             */
/*   Updated: 2019/01/03 02:52:51 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

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
