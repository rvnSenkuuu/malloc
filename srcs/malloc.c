/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:05:28 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/28 12:00:41 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

t_allocator	g_allocator = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL
};

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

void	*large_malloc(size_t size)
{
	t_zone	*zone;

	size_t	real_size = size + sizeof(t_zone) + sizeof(t_block);
	real_size = ALIGN_TO(real_size, GET_PAGE_SIZE);

	zone = mmap(NULL, real_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED)
		return NULL;
	
	zone->type = LARGE;
	zone->size = real_size;
	zone->next = NULL;
	
	zone->blocks = GET_BLOCKS_FROM_ZONE(zone);
	zone->blocks->free = false;
	zone->blocks->size = size;
	zone->blocks->next = NULL;
	zone->blocks->prev = NULL;
	
	add_zone_to_allocator(&g_allocator.large, zone);
    return GET_BLOCK_PTR_FROM_BLOCKS(zone->blocks);
}

t_zone	*create_small_zone(t_zone_type type, size_t size)
{
	t_zone	*zone;

	size_t	real_size = size + sizeof(t_zone) + (MIN_BLOCK_COUNT * sizeof(t_block));
	real_size = ALIGN_TO(real_size, GET_PAGE_SIZE);

	zone = mmap(NULL, real_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED)
		return NULL;

	zone->type = type;
	zone->size = real_size;
	zone->used_size = sizeof(t_block) + sizeof(t_zone);
	zone->next = NULL;

	zone->blocks = GET_BLOCKS_FROM_ZONE(zone);
	zone->blocks->size = real_size - sizeof(t_zone) - sizeof(t_block);
	zone->blocks->free = true;
	zone->blocks->next = NULL;
	zone->blocks->prev = NULL;

	return zone;
}

/*
Trouver une zone avec assez d'espace pour mettre un nouveau block
	-Iterer dans toutes les zone pour savoir si
	la taille de la zone - la taille du block est superieur a 0
	
	-Iterer dans tout les blocks pour voir si il y'a un block non utiliser avec
	au minimum la taille du nouveau block que l'on souhaite
		-si elle est plus grande decoupe le block:
			taille du block entier - (taille demander - taille du block) 

*/

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

	t_block	*blocks = zone->blocks;
	for (; blocks; blocks = blocks->next) {
		if (blocks->free == true && blocks->size >= size) {
			size_t	remaining_size = blocks->size - size;
			size_t	min_block_size = sizeof(t_block) + ALIGNMENT;
			if (remaining_size >= min_block_size)
				split_block(blocks, size);
			blocks->free = false;
			return GET_BLOCK_PTR_FROM_BLOCKS(blocks);
		}
	}

	if (zone->used_size + total_block_size > zone->size)
		return NULL;

	void	*new_block_addr = (char *)zone + zone->used_size;
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

	return GET_BLOCK_PTR_FROM_BLOCKS(new_block);
}

void	*small_malloc(t_zone **global_zone, size_t size)
{
	t_zone_type	type;
	t_zone	*zone;
	void	*ptr;

	if (size <= TINY_BLOCK_SIZE)
		type = TINY;
	else
		type = SMALL;

	size_t	total_block_size = size + sizeof(t_block);
	zone = *global_zone;
	for (; zone; zone = zone->next) {
		if (check_zone_has_space(zone, total_block_size) == true)
			break;
	}

	if (!zone) {
		zone = create_small_zone(type, size);
		if (!zone)
			return NULL;
		add_zone_to_allocator(global_zone, zone);
		return GET_BLOCK_PTR_FROM_BLOCKS(zone->blocks);
	}

	ptr = insert_block_in_zone(zone, size);

	return ptr;
}

void	*malloc(size_t size)
{
	if (size == 0)
		return NULL;

	void	*ptr = NULL;

	if (size <= TINY_BLOCK_SIZE)
		ptr = small_malloc(&g_allocator.tiny, size);
	else if (size <= SMALL_BLOCK_SIZE)
		ptr = small_malloc(&g_allocator.small, size);
	else 
		ptr = large_malloc(size);

	return ptr;
}

void	free(void *ptr)
{
	(void)ptr;
}

void	*realloc(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	return NULL;
}