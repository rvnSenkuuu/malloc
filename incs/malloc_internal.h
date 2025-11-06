/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:06:25 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/06 10:38:18 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_H
#define MALLOC_INTERNAL_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>

#define ALIGNMENT 16
#define ALIGN_TO(x, to_align) (((x) + (to_align) - 1) & ~((to_align) - 1))

#define TINY_BLOCK_SIZE 32
#define SMALL_BLOCK_SIZE 512
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

typedef struct	s_config {
	bool	stats;
	bool	verbose;
	bool	config_initialized;
	int	file_fd;

	size_t	total_allocs;
	size_t	total_frees;
	size_t	allocated_block_count;
	size_t	bytes_allocated;
	size_t	bytes_freed;
} t_config;

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
	t_config	config;
} t_allocator;

extern t_allocator	g_allocator;
extern pthread_mutex_t	mutex;

void	free(void *ptr);
void	*malloc(size_t size);

void	add_zone_to_allocator(t_zone **allocator_zone, t_zone *zone);
void	split_block(t_block *block, size_t size);
void	merge_block(t_block *block);
void	merge_with_next(t_block *block);
void	merge_with_prev(t_block *block);
bool	check_zone_has_space(t_zone *zone, size_t total_block_size);
bool	search_ptr_in_zone(t_zone *allocator_zone, void *ptr);
void	*insert_block_in_zone(t_zone *zone, size_t size);
t_zone	*create_small_zone(t_zone_type type);

void	malloc_stats(void *ptr, size_t size);
void	free_stats(void *ptr, size_t size);
void	realloc_stats(void *old_ptr, void *new_ptr, size_t old_size, size_t new_size);
void	malloc_config_init(void);
void	malloc_config_clean(void);

void	ft_memcpy(void *d, const void *s, size_t n);
void	ft_putstr_fd(int fd, const char *s);
void	ft_putnbr_fd(int fd, size_t n);
void	ft_putstr_fd(int fd, const char *s);
void	ft_puthex_fd(int fd, size_t addr);
int	ft_strncmp(const char *s1, const char *s2, size_t n);
int	ft_dprintf(int fd, const char *format, ...);
size_t	ft_strlen(const char *s);

#endif