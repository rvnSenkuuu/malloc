# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/08 15:06:11 by tkara2            #+#    #+#              #
#    Updated: 2025/10/10 15:37:03 by tkara2           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft_malloc.so
SRCS = srcs/malloc.c \
       srcs/show_alloc_mem.c

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

TEST_DIR = test/
TEST_SRC = $(TEST_DIR)test.c
TEST_OBJS = $(patsubst $(TEST_DIR)%.c, $(TEST_DIR)%, $(TEST_SRC))

USE_FT_LIB ?= 0

ifeq ($(USE_FT_LIB), 1)
	CFLAGS += -DUSE_FT_LIB
test: $(TARGET) $(INCS) $(TEST_OBJS)
	LD_PRELOAD=./$(NAME) ./test/test
else
test: $(TARGET) $(INCS) $(TEST_OBJS)
	./test/test
endif

$(TEST_DIR)%: $(TEST_DIR)%.c $(INCS)
	$(CC) $(CFLAGS) $< -L. -lft_malloc -Wl,-rpath=. -o $@

clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(TARGET) $(NAME) $(TEST_DIR)*.d $(TEST_OBJS)

re: fclean 
	$(MAKE) all

-include $(D_FILES)

.PHONY: all clean fclean re test