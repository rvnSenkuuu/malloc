/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:05:28 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/21 18:31:51 by tkara2           ###   ########.fr       */
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
	zone->size = size;
	zone->next = NULL;
	
	zone->blocks = GET_BLOCKS_FROM_ZONE(zone);
	zone->blocks->free = false;
	zone->blocks->size = size;
	zone->blocks->next = NULL;
	
	add_zone_to_allocator(&g_allocator.large, zone);
    return GET_BLOCK_PTR_FROM_BLOCKS(zone->blocks);
}

t_zone	*create_small_zone(t_zone_type type, size_t size)
{
	t_zone	*zone;

	size_t	real_size = size + sizeof(t_zone) + (100 * sizeof(t_block));
	real_size = ALIGN_TO(real_size, GET_PAGE_SIZE);

	zone = mmap(NULL, real_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED)
		return NULL;

	zone->type = type;
	zone->size = real_size;
	zone->next = NULL;
}

void	*small_malloc(t_zone *global_zone, size_t size)
{
	t_zone_type	type;
	t_zone	*zone;

	if (size <= TINY_BLOCK_SIZE)
		type = TINY;
	else
		type = SMALL;

	if (!global_zone) {
		zone = create_small_zone(type, size);
		if (!zone)
			return NULL;
		add_zone_to_allocator(global_zone, zone);
	}

	return NULL;
}

void	*malloc(size_t size)
{
	if (size == 0)
		return NULL;

	void	*ptr = NULL;

	if (size <= TINY_BLOCK_SIZE)
		ptr = small_malloc(g_allocator.tiny, size);
	else if (size <= SMALL_BLOCK_SIZE)
		ptr = small_malloc(g_allocator.small, size);
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