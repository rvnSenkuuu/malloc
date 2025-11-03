/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:05:28 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/03 11:31:57 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

t_allocator	g_allocator = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL,
	.config = {0}
};
pthread_mutex_t	mutex;

void	*large_malloc(size_t size)
{
	t_zone	*zone;

	size_t	real_size = size + sizeof(t_zone) + sizeof(t_block);
	real_size = ALIGN_TO(real_size, GET_PAGE_SIZE);

	zone = mmap(NULL, real_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
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
    return GET_PTR_FROM_BLOCKS(zone->blocks);
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
		zone = create_small_zone(type);
		if (!zone)
			return NULL;
		add_zone_to_allocator(global_zone, zone);
	}

	ptr = insert_block_in_zone(zone, size);

	return ptr;
}

void	*malloc(size_t size)
{
	if (size == 0)
		return NULL;

	pthread_mutex_lock(&mutex);

	void	*ptr = NULL;

	if (size <= TINY_BLOCK_SIZE)
		ptr = small_malloc(&g_allocator.tiny, size);
	else if (size <= SMALL_BLOCK_SIZE)
		ptr = small_malloc(&g_allocator.small, size);
	else 
		ptr = large_malloc(size);

	if (!ptr) {
		errno = ENOMEM;
		pthread_mutex_unlock(&mutex);
		return NULL;
	}

	malloc_stats(ptr, size);
	pthread_mutex_unlock(&mutex);
	return ptr;
}

void	free(void *ptr)
{
	if (!ptr)
		return;

	pthread_mutex_lock(&mutex);

	if (!search_ptr_in_zone(g_allocator.tiny, ptr)
	&& !search_ptr_in_zone(g_allocator.small, ptr) 
	&& !search_ptr_in_zone(g_allocator.large, ptr)) {
		pthread_mutex_unlock(&mutex);
		return;
	}
	
	t_block	*block = GET_BLOCKS_FROM_PTR(ptr);
	size_t	block_size = block->size;
	
	if (block->free == true) {
		pthread_mutex_unlock(&mutex);
		return;
	}
	block->free = true;
	merge_block(block);
	
	free_stats(ptr, block_size);
	pthread_mutex_unlock(&mutex);
}

void	*try_avoid_reallocating(t_block *block, size_t size)
{
	size_t	merged_size;

	if (block->next && block->next->free) {
		merged_size = block->size + sizeof(t_block) + block->next->size;
		if (merged_size >= size) {
			merge_with_next(block);
			return GET_PTR_FROM_BLOCKS(block);
		}
	}

	if (block->prev && block->prev->free) {
		merged_size = block->size + sizeof(t_block) + block->prev->size;
		if (merged_size >= size) {
			merge_with_prev(block);
			return GET_PTR_FROM_BLOCKS(block->prev);
		}
	}
	return NULL;
}

void	*realloc(void *ptr, size_t size)
{
	void	*new_ptr = NULL;

	pthread_mutex_lock(&mutex);

	if (!ptr) {
		new_ptr = malloc(size);
		pthread_mutex_unlock(&mutex);
		return new_ptr;
	}

	if (size == 0) {
		free(ptr);
		pthread_mutex_unlock(&mutex);
		return new_ptr;
	}

	t_block	*block = GET_BLOCKS_FROM_PTR(ptr);
	size_t	old_block_size = block->size;

	if (block->size < size) {
		pthread_mutex_unlock(&mutex);
		void	*merged_ptr = try_avoid_reallocating(block, size);
		if (merged_ptr) {
			pthread_mutex_unlock(&mutex);
			return merged_ptr;
		}
	}

	new_ptr = malloc(size);
	if (!new_ptr) {
		pthread_mutex_unlock(&mutex);
		return ptr;
	}

	ft_memcpy(new_ptr, ptr, block->size);
	free(ptr);

	realloc_stats(ptr, new_ptr, old_block_size, size);
	pthread_mutex_unlock(&mutex);
	return new_ptr;
}