/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:05:28 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/21 14:32:00 by tkara2           ###   ########.fr       */
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
	if (zone == MAP_FAILED) return NULL;
	
	zone->type = LARGE;
	zone->size = size;
	
	zone->blocks = GET_BLOCKS_FROM_ZONE(zone);
	zone->blocks->free = false;
	zone->blocks->size = size;
	zone->blocks->next = NULL;
	
	add_zone_to_allocator(&g_allocator.large, zone);
    return GET_BLOCK_PTR_FROM_BLOCKS(zone->blocks);
}

void	*tiny_small_malloc(size_t size)
{
	(void)size;
	return NULL;
}

void	*malloc(size_t size)
{
	if (size == 0) return NULL;
	
	if (size > SMALL_BLOCK_SIZE) return large_malloc(size);
	else return tiny_small_malloc(size);
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