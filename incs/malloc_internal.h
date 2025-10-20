/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:06:25 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/20 18:13:38 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_H
#define MALLOC_INTERNAL_H

#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

#define ALIGNMENT 16
#define ALIGN_TO(x, to_align) (((x) + (to_align) - 1) & ~((to_align) - 1))

#define TINY_BLOCK_SIZE 256
#define SMALL_BLOCK_SIZE 1024

#define GET_PAGE_SIZE sysconf(_SC_PAGE_SIZE)

typedef enum {
	TINY,
	SMALL,
	LARGE,
} t_zone_type;

typedef struct __attribute__((aligned(ALIGNMENT))) s_block {
	bool	free;
	size_t	size;
	struct s_block	*next;
} t_block;

typedef struct __attribute__((aligned(ALIGNMENT))) s_zone {
	t_zone_type	type;
	size_t	size;
	t_block	*blocks;
	struct s_zone	*next;
} t_zone;

typedef struct __attribute__((aligned(ALIGNMENT))) s_allocator {
	t_zone	*tiny;
	t_zone	*small;
	t_zone	*large;
} t_allocator;

extern t_allocator	g_allocator;

#endif