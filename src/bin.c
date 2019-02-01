/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/01 17:14:53 by jye               #+#    #+#             */
/*   Updated: 2019/02/01 17:14:55 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

#define FASTBIN_INDEX(x) ((x) >> 3)
#define SMALLBIN_INDEX(x) (FASTBIN_INDEX(x))
#define LARGEBIN1_INDEX(x) (((x) >> 6) + 56)
#define LARGEBIN2_INDEX(x) (((x) >> 9) + 91)
#define LARGEBIN3_INDEX(x) (((x) >> 12) + 110)
#define LARGEBIN4_INDEX(x) (((x) >> 15) + 119)
#define LARGEBIN5_INDEX(x) (((x) >> 18) + 124)

int		bin_index(size_t x)
{
	if (x < (64 << 3))
		return (SMALLBIN_INDEX(x));
	if ((x >> 6) <= 32)
		return (LARGEBIN1_INDEX(x));
	if ((x >> 9) <= 20)
		return (LARGEBIN2_INDEX(x));
	if ((x >> 12) <= 10)
		return (LARGEBIN3_INDEX(x));
	if ((x >> 15) <= 4)
		return (LARGEBIN4_INDEX(x));
	if ((x >> 18) <= 2)
		return (LARGEBIN5_INDEX(x));
	return (126);
}

int		largebin_index(size_t x)
{
	if ((x >> 6) <= 32)
		return (LARGEBIN1_INDEX(x));
	if ((x >> 9) <= 20)
		return (LARGEBIN2_INDEX(x));
	if ((x >> 12) <= 10)
		return (LARGEBIN3_INDEX(x));
	if ((x >> 15) <= 4)
		return (LARGEBIN4_INDEX(x));
	if ((x >> 18) <= 2)
		return (LARGEBIN5_INDEX(x));
	return (126);
}

int		smallbin_index(size_t x)
{
	return (SMALLBIN_INDEX(x));
}

int		fastbin_index(size_t x)
{
	return (FASTBIN_INDEX(x));
}
