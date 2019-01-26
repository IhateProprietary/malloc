/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   indexs.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jye <marvin@42.fr>                         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/26 05:22:18 by jye               #+#    #+#             */
/*   Updated: 2019/01/26 05:22:54 by jye              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INDEXS_H
# define INDEXS_H

# define FASTBIN_INDEX(x) ((x) >> 3)
# define SMALLBIN_INDEX(x) (FASTBIN_INDEX(x))
# define LARGEBIN1_INDEX(x) (((x) >> 6) + 56)
# define LARGEBIN2_INDEX(x) (((x) >> 9) + 91)
# define LARGEBIN3_INDEX(x) (((x) >> 12) + 110)
# define LARGEBIN4_INDEX(x) (((x) >> 15) + 119)
# define LARGEBIN5_INDEX(x) (((x) >> 18) + 124)

#endif
