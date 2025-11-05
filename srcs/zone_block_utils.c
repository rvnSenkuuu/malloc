/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone_block_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 17:34:07 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/05 12:53:51 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

void	add_zone_to_allocator(t_zone **allocator_zone, t_zone *zone)
{
	if (!*allocator_zone) {
		*allocator_zone = zone;
		zone->next = NULL;
		return;
	}

	t_zone	*current = *allocator_zone;
	while (current->next)
		current = current->next;
	current->next = zone;
	zone->next = NULL;
}

t_zone	*create_small_zone(t_zone_type type)
{
	size_t	zone_size = 0;
	t_zone	*zone;

	if (type == TINY)
		zone_size = sizeof(t_zone) + (ALIGN_TO(TINY_BLOCK_SIZE + sizeof(t_block), ALIGNMENT)) * MIN_BLOCK_COUNT;
	else
		zone_size = sizeof(t_zone) + (ALIGN_TO(SMALL_BLOCK_SIZE + sizeof(t_block), ALIGNMENT)) * MIN_BLOCK_COUNT;
	zone_size = ALIGN_TO(zone_size, GET_PAGE_SIZE);

	zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED)
		return NULL;

	zone->type = type;
	zone->size = zone_size;
	zone->used_size = sizeof(t_zone);
	zone->next = NULL;
	zone->blocks = NULL;

	return zone;
}

bool	check_zone_has_space(t_zone *zone, size_t total_block_size)
{
	if (zone->used_size + total_block_size > zone->size)
		return false;
	
	t_block	*blocks = zone->blocks;
	if (!blocks)
		return total_block_size < zone->size - sizeof(t_zone);
	
	for (; blocks; blocks = blocks->next) {
		if (blocks->free && blocks->size >= total_block_size)
			return true;
	}

	t_block	*last = zone->blocks;
	while (last->next)
		last = last->next;

	size_t	offset = (uintptr_t)last + sizeof(t_block) + last->size - (uintptr_t)zone;
	offset = ALIGN_TO(offset, ALIGNMENT);
	if (offset + total_block_size <= zone->size)
		return true;

	return false;
}

void	split_block(t_block *block, size_t size)
{
	size_t	aligned_size = ALIGN_TO(size, ALIGNMENT);
	size_t	block_current_size = block->size;
	t_block	*next_block = block->next;
	
	block->size = aligned_size;
	uintptr_t	new_block_addr = (uintptr_t)block + sizeof(t_block) + aligned_size;
	new_block_addr = ALIGN_TO(new_block_addr, ALIGNMENT);
	t_block	*new_block = (t_block *)new_block_addr;

	new_block->size = block_current_size - aligned_size - sizeof(t_block);
	new_block->free = true;
	new_block->next = next_block;

	if (next_block)
		next_block->prev = new_block;
	new_block->prev = block;
	block->next = new_block;
	block->free = false;
}

void	*insert_block_in_zone(t_zone *zone, size_t size)
{
	size_t	total_block_size = size + sizeof(t_block);

	if (zone->used_size + total_block_size > zone->size)
		return NULL;
	
	t_block	*blocks = zone->blocks;
	for (; blocks; blocks = blocks->next) {
		if (blocks->free == true && blocks->size >= size) {
			size_t	remaining_size = blocks->size - size;
			size_t	min_block_size = sizeof(t_block) + ALIGNMENT;
			if (remaining_size >= min_block_size)
				split_block(blocks, size);
			return GET_PTR_FROM_BLOCKS(blocks);
		}
	}

	uintptr_t	new_block_addr = (uintptr_t)zone + sizeof(t_zone) + zone->used_size;
	new_block_addr = ALIGN_TO(new_block_addr, ALIGNMENT);
	t_block	*new_block = (t_block *)new_block_addr;

	new_block->size = size;
	new_block->free = false;
	new_block->next = NULL;

	if (!zone->blocks) {
		zone->blocks = new_block;
		new_block->prev = NULL;
	} else {
		t_block	*last = zone->blocks;
		while (last->next)
			last = last->next;
		last->next = new_block;
		new_block->prev = last;
	}

	zone->used_size += total_block_size;

	return GET_PTR_FROM_BLOCKS(new_block);
}

void	merge_with_next(t_block *block)
{
	t_block	*next_block = block->next;

	block->size += next_block->size + sizeof(t_block);
	block->next = next_block->next;
	if (next_block->next)
		next_block->next->prev = block;
}

void	merge_with_prev(t_block *block)
{
	t_block	*prev_block = block->prev;

	prev_block->size += block->size + sizeof(t_block);
	prev_block->next = block->next;
	if (block->next)
		block->next->prev = prev_block;	
}

void	merge_block(t_block *block)
{
	if (block->next && block->next->free)
		merge_with_next(block);

	if (block->prev && block->prev->free)
		merge_with_prev(block);
}

bool	search_ptr_in_zone(t_zone *allocator_zone, void *ptr)
{
	for (t_zone *zone = allocator_zone; zone; zone = zone->next) {
		void	*zone_start = (void *)((char *)zone + sizeof(t_zone));
		void	*zone_end = (char *)zone + zone->size;
		if (ptr >= zone_start && ptr < zone_end)
			return true;
	}

	return false;
}

void	ft_memcpy(void *d, const void *s, size_t n)
{
	unsigned char	*src = (unsigned char *)s;
	unsigned char	*dest = d;

	while (n--)
		*dest++ = *src++;
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	while (s1 && s2 && *s1 == *s2 && n--) {
		s1++;
		s2++;
	}

	return (unsigned char)*s1 - (unsigned char)*s2;
}

__attribute__((constructor))
void	init_malloc(void)
{
	pthread_mutexattr_t	attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &attr);
}

__attribute__((destructor))
void	destroy_zone(void)
{
	t_zone	*zone;
	t_zone	*next_zone;

	if (g_allocator.tiny)
		MUNMAP_ZONES (g_allocator.tiny)
	g_allocator.tiny = NULL;

	if (g_allocator.small)
		MUNMAP_ZONES (g_allocator.small)
	g_allocator.small = NULL;
	
	if (g_allocator.large)
		MUNMAP_ZONES (g_allocator.large)
	g_allocator.large = NULL;
}