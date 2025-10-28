/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:06:25 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/28 18:11:05 by tkara2           ###   ########.fr       */
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
#define MIN_BLOCK_COUNT 100

#define GET_PAGE_SIZE sysconf(_SC_PAGE_SIZE)

#define GET_BLOCKS_FROM_ZONE(zone) ((t_block *)((char *)zone + sizeof(t_zone)))
#define GET_PTR_FROM_BLOCKS(blocks) ((void *)((char *)blocks + sizeof(t_block)))
#define GET_BLOCKS_FROM_PTR(ptr) ((t_block *)((char *)ptr - sizeof(t_block)))

typedef enum {
	TINY,
	SMALL,
	LARGE,
} t_zone_type;

typedef struct __attribute__((aligned(ALIGNMENT))) s_block {
	bool	free;
	size_t	size;
	struct s_block	*next;
	struct s_block	*prev;
} t_block;

typedef struct __attribute__((aligned(ALIGNMENT))) s_zone {
	t_zone_type	type;
	size_t	size;
	size_t	used_size;
	t_block	*blocks;
	struct s_zone	*next;
} t_zone;

typedef struct __attribute__((aligned(ALIGNMENT))) s_allocator {
	t_zone	*tiny;
	t_zone	*small;
	t_zone	*large;
} t_allocator;

extern t_allocator	g_allocator;

void	free(void *ptr);
void	*malloc(size_t size);

#endif