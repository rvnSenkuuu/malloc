/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:06:25 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/31 11:56:58 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_H
#define MALLOC_INTERNAL_H

#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

#define ALIGNMENT 16
#define ALIGN_TO(x, to_align) (((x) + (to_align) - 1) & ~((to_align) - 1))

#define TINY_BLOCK_SIZE 256
#define SMALL_BLOCK_SIZE 1024
#define MIN_BLOCK_COUNT 100

#define GET_PAGE_SIZE sysconf(_SC_PAGE_SIZE)

#define GET_BLOCKS_FROM_ZONE(zone) ((t_block *)((uintptr_t)zone + sizeof(t_zone)))
#define GET_PTR_FROM_BLOCKS(blocks) ((void *)((uintptr_t)blocks + sizeof(t_block)))
#define GET_BLOCKS_FROM_PTR(ptr) ((t_block *)((uintptr_t)ptr - sizeof(t_block)))

#define MUNMAP_ZONES(zones) do {      \
		zone = zones;                 \
		while (zone) {                \
			next_zone = zone->next;   \
			munmap(zone, zone->size); \
			zone = next_zone;         \
		}                             \
} while (0);

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

void	add_zone_to_allocator(t_zone **allocator_zone, t_zone *zone);
void	split_block(t_block *block, size_t size);
void	merge_block(t_block *block);
bool	check_zone_has_space(t_zone *zone, size_t total_block_size);
bool	search_ptr_in_zone(t_zone *allocator_zone, void *ptr);
void	*insert_block_in_zone(t_zone *zone, size_t size);
t_zone	*create_small_zone(t_zone_type type);

#endif