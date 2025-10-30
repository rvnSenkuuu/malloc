/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone_block_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 17:34:07 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/30 10:36:54 by tkara2           ###   ########.fr       */
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

	zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
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

	size_t	offset = (char *)last + sizeof(t_block) + last->size - (char *)zone;
	if (offset + total_block_size <= zone->size)
		return true;

	return false;
}

void	split_block(t_block *block, size_t size)
{
	size_t	block_current_size = block->size;
	t_block	*next_block = block->next;
	
	block->size = size;
	void	*new_block_addr = (char *)block + sizeof(t_block) + size;
	t_block	*new_block = (t_block *)new_block_addr;

	new_block->size = block_current_size - size - sizeof(t_block);
	new_block->free = true;
	new_block->next = next_block;

	if (next_block)
		next_block->prev = new_block;
	new_block->prev = block;
	block->next = new_block;
}

void	*insert_block_in_zone(t_zone *zone, size_t size)
{
	size_t	total_block_size = size + sizeof(t_block);
	// total_block_size = ALIGN_TO(total_block_size, ALIGNMENT);

	if (zone->used_size + total_block_size > zone->size)
		return NULL;
	
	t_block	*blocks = zone->blocks;
	for (; blocks; blocks = blocks->next) {
		if (blocks->free == true && blocks->size >= size) {
			size_t	remaining_size = blocks->size - size;
			size_t	min_block_size = sizeof(t_block) + ALIGNMENT;
			if (remaining_size >= min_block_size)
				split_block(blocks, size);
			blocks->free = false;
			return GET_PTR_FROM_BLOCKS(blocks);
		}
	}

	void	*new_block_addr = (char *)zone + sizeof(t_zone) + zone->used_size;
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

void	merge_block(t_block *block)
{
	if (block->next && block->next->free) {
		t_block	*next_block = block->next;

		block->size += next_block->size;
		block->next = next_block->next;
		if (next_block->next)
			next_block->next->prev = block;
	}

	if (block->prev && block->prev->free) {
		t_block	*prev_block = block->prev;

		prev_block->size += block->size;
		prev_block->next = block->next;
		if (block->next)
			block->next->prev = prev_block;
	}
}

bool	search_ptr_in_zone(t_zone *allocator_zone, void *ptr)
{
	t_zone	*zone;
	t_block	*blocks;

	zone = allocator_zone;
	for (; zone; zone = zone->next) {
		blocks = zone->blocks;
		for (; blocks; blocks = blocks->next) {
			if (ptr >= GET_PTR_FROM_BLOCKS(blocks) && ptr < (void *)zone + zone->size)
				return true;
		}
	}

	return false;
}

__attribute__((destructor))
void	destroy_zone(void)
{
	t_zone	*zone;
	t_zone	*next_zone;

	if (g_allocator.tiny) {
		zone = g_allocator.tiny;
		while (zone) {
			next_zone = zone->next;
			munmap(zone, zone->size);
			zone = next_zone;
		}
	}
	g_allocator.tiny = NULL;

	if (g_allocator.small) {
		zone = g_allocator.small;
		while (zone) {
			next_zone = zone->next;
			munmap(zone, zone->size);
			zone = next_zone;
		}
	}
	g_allocator.small = NULL;

	if (g_allocator.large) {
		zone = g_allocator.large;
		while (zone) {
			next_zone = zone->next;
			munmap(zone, zone->size);
			zone = next_zone;
		}
	}
	g_allocator.large = NULL;
}