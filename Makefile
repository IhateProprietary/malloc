#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jye <marvin@42.fr>                         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/12/19 04:43:40 by jye               #+#    #+#              #
#    Updated: 2018/12/21 03:22:21 by jye              ###   ########.fr        #
#                                                                              #
#******************************************************************************#

CC		= gcc
CFLAGS	= -Wall -Wextra -fPIC
CDEBUG	= -g3

CFOLDER	= src
HFOLDER	= src
OFOLDER	= obj

HOSTTYPE = $(shell uname -m)_$(shell uname -s)

SRCS	= alloc_newchunk arena forsake_fastbins int_alloc_largebin int_free \
		  int_malloc int_malloc_init int_realloc malloc_public sanity_check \
          show_mem_alloc unsortedbin
CNAME	= $(addsuffix .c, $(SRCS))
ONAME	= $(addsuffix .o, $(SRCS))

CWPATH	= $(addprefix $(CFOLDER)/, $(CNAME))
OWPATH	= $(addprefix $(OFOLDER)/, $(ONAME))

LIB = libft_malloc
EXT = .so
NAME = $(LIB)$(EXT)

all: libft_printf.a libft.a $(NAME)

$(NAME): $(OWPATH)
	$(CC) -shared $^ -Lft_printf -lftprintf -Llibft -lft -o $(LIB)_$(HOSTTYPE)$(EXT)
	ln -sf $(LIB)_$(HOSTTYPE)$(EXT) $@

$(OFOLDER)/%.o : $(CFOLDER)/%.c
	@mkdir -p $(OFOLDER)
	$(CC) $(CFLAGS) $(CDEBUG) -Ilibft -Ift_printf/includes -c $^ -o $@

libft_printf.a:
	make -C ft_printf

libft.a:
	make -C libft

clean:
	make -C ft_printf clean
	make -C libft clean
	rm -rf $(OFOLDER)

fclean: clean
	make -C ft_printf fclean
	make -C libft fclean
	rm -rf $(NAME) $(LIB)_$(HOSTTYPE)$(EXT)

re: fclean all
