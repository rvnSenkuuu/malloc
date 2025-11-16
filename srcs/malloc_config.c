/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_config.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 10:26:17 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/10 09:54:38 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

__attribute__((constructor))
void	malloc_config_init(void)
{
	if (g_allocator.config.config_initialized)
		return;

	char	*stats = getenv("MALLOC_STATS");
	char	*verbose = getenv("MALLOC_VERBOSE");
	char	*log_file = getenv("MALLOC_TRACE");
	
	if (stats && !ft_strncmp(stats, "true", 4))
		g_allocator.config.stats = true;
	if (verbose && !ft_strncmp(verbose, "yes", 3))
		g_allocator.config.verbose = true;
	if (log_file) {
		g_allocator.config.file_fd = open(log_file, O_CREAT | O_WRONLY, 0644);
		if (g_allocator.config.file_fd < 0) {
			perror("Open failed for malloc trace config file using STDERR_FILENO as default fd");
		}
	}
	else
		g_allocator.config.file_fd = STDERR_FILENO;
	g_allocator.config.config_initialized = true;
}

__attribute__((destructor))
void	malloc_config_clean(void)
{
	if (!g_allocator.config.config_initialized)
		return;

	if (g_allocator.config.stats) {
		
		ft_dprintf(g_allocator.config.file_fd, "=== MALLOC STATISTICS ===\n");

		ft_dprintf(g_allocator.config.file_fd, "Total allocations: %d\n", g_allocator.config.total_allocs);
		
		ft_dprintf(g_allocator.config.file_fd, "Total frees: %d\n", g_allocator.config.total_frees);
		
		ft_dprintf(g_allocator.config.file_fd, "Block allocation count: %d\n", g_allocator.config.total_frees);
		
		ft_dprintf(g_allocator.config.file_fd, "Block allocated: %d\n", g_allocator.config.bytes_allocated);
		
		ft_dprintf(g_allocator.config.file_fd, "Block freed: %d\n", g_allocator.config.bytes_freed);
		
		if (g_allocator.config.allocated_block_count > 0)
			ft_dprintf(g_allocator.config.file_fd,
                       "\nWARNING: %d memory block(s) leaks detected!\n",
                       g_allocator.config.allocated_block_count);
		else
			ft_dprintf(g_allocator.config.file_fd, "\nNo memory leaks detected\n");
		
		ft_dprintf(g_allocator.config.file_fd, "=========================\n");
	}

	if (g_allocator.config.file_fd != STDERR_FILENO && g_allocator.config.file_fd != -1)
		close(g_allocator.config.file_fd);
}

void	malloc_stats(void *ptr, size_t size)
{
	if (!g_allocator.config.config_initialized)
		malloc_config_init();

	g_allocator.config.total_allocs++;
	g_allocator.config.allocated_block_count++;
	g_allocator.config.bytes_allocated += size;

	if (g_allocator.config.verbose)
		ft_dprintf(g_allocator.config.file_fd, "[MALLOC] malloc(%d) = %p\n", size, ptr);
}

void	free_stats(void *ptr, size_t size)
{
	if (!g_allocator.config.config_initialized)
		return;

	g_allocator.config.total_frees++;
	g_allocator.config.allocated_block_count--;
	g_allocator.config.bytes_freed += size;

	if (g_allocator.config.verbose)
		ft_dprintf(g_allocator.config.file_fd, "[MALLOC] free(%p) = %d bytes\n", 
			ptr, size);
}

void	realloc_stats(void *old_ptr, void *new_ptr, size_t old_size, size_t new_size)
{	
	if (!g_allocator.config.config_initialized)
		return;

    if (g_allocator.config.verbose)
		ft_dprintf(g_allocator.config.file_fd, 
			"[MALLOC] realloc(%p, %d) = %p [was %d bytes]\n", 
			old_ptr, new_size, new_ptr, old_size);
}