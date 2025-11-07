# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/08 15:06:11 by tkara2            #+#    #+#              #
#    Updated: 2025/11/07 10:47:21 by tkara2           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft_malloc.so
SRCS = srcs/malloc.c \
       srcs/zone_block_utils.c \
       srcs/show_alloc_mem.c \
	   srcs/malloc_config.c \
	   srcs/utils/utils.c \
	   srcs/utils/ft_dprintf.c

INCS = incs/malloc_internal.h \
       incs/ft_malloc.h

OBJS_DIR = .objs/
OBJS = $(patsubst %.c, $(OBJS_DIR)%.o, $(SRCS))
D_FILES = $(OBJS:.o=.d)

ifeq ($(HOSTTYPE),)
    HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

TARGET = libft_malloc_$(HOSTTYPE).so

CC = cc
CFLAGS = -Wall -Werror -Wextra -Iincs -fPIC -MMD -MP
RM = rm -rf

all: $(TARGET)

$(TARGET): $(OBJS) $(INCS)
	$(CC) $(CFLAGS) -shared $(OBJS) -o $(TARGET)
	ln -sf $(TARGET) $(NAME)

$(OBJS_DIR)%.o: %.c $(INCS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(TARGET) $(NAME)

re: fclean 
	$(MAKE) all

-include $(D_FILES)

.PHONY: all clean fclean re