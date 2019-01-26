/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/26 05:16:53 by jye               #+#    #+#             */
/*   Updated: 2019/01/26 05:22:53 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIN_H
# define BIN_H

# include "libft.h"
# include "indexs.h"

# define BIN_INDEX1(x) TER(x < (64 << 3), SMALLBIN_INDEX(x), BIN_INDEX2(x))
# define BIN_INDEX2(x) TER(((x) >> 6) <= 32, LARGEBIN1_INDEX(x), BIN_INDEX3(x))
# define BIN_INDEX3(x) TER(((x) >> 9) <= 20, LARGEBIN2_INDEX(x), BIN_INDEX4(x))
# define BIN_INDEX4(x) TER(((x) >> 12) <= 10, LARGEBIN3_INDEX(x), BIN_INDEX5(x))
# define BIN_INDEX5(x) TER(((x) >> 15) <= 4, LARGEBIN4_INDEX(x), BIN_INDEX6(x))
# define BIN_INDEX6(x) TER(((x) >> 18) <= 2, LARGEBIN5_INDEX(x), 126)
# define LARGEBIN_INDEX(x) BIN_INDEX2(x)
# define BIN_INDEX(x) BIN_INDEX1(x)

#endif
