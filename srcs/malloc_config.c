/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_config.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 10:26:17 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/03 12:26:05 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

__attribute__((constructor))
void	malloc_config_init(void)
{
	char	*stats = getenv("MALLOC_STATS");
	char	*verbose = getenv("MALLOC_VERBOSE");
	char	*log_file = getenv("MALLOC_TRACE");

	if (stats && !strcmp(stats, "true"))
		g_allocator.config.stats = true;
	if (verbose && !strcmp(verbose, "yes"))
		g_allocator.config.verbose = true;
	if (log_file) {
		g_allocator.config.file_fd = open(log_file, O_CREAT | O_WRONLY, 0644);
		if (g_allocator.config.file_fd < 0) {
			perror("Open failed for malloc trace config file using STDERR_FILENO as default fd");
			g_allocator.config.file_fd = STDERR_FILENO;
		}
	}
}

__attribute__((destructor))
void	malloc_config_clean(void)
{
	int	len;
	char	buffer[512];
	if (g_allocator.config.stats) {
		
		ft_putstr_fd(g_allocator.config.file_fd, "=== MALLOC STATISTICS ===\n");

		len = snprintf(buffer, sizeof(buffer), "Total allocations: %zu\n", g_allocator.config.total_allocs);
        write(g_allocator.config.file_fd, buffer, len);
		
		len = snprintf(buffer, sizeof(buffer), "Total frees: %zu\n", g_allocator.config.total_frees);
        write(g_allocator.config.file_fd, buffer, len);
		
		len = snprintf(buffer, sizeof(buffer), "Block allocation count: %zu\n", g_allocator.config.total_frees);
        write(g_allocator.config.file_fd, buffer, len);
		
		len = snprintf(buffer, sizeof(buffer), "Block allocated: %zu\n", g_allocator.config.bytes_allocated);
        write(g_allocator.config.file_fd, buffer, len);
		
		len = snprintf(buffer, sizeof(buffer), "Block freed: %zu\n", g_allocator.config.bytes_freed);
        write(g_allocator.config.file_fd, buffer, len);
		
		if (g_allocator.config.allocated_block_count > 0) {
			len = snprintf(buffer, sizeof(buffer),
                       "\nWARNING: %zu memory block(s) leaks detected!\n",
                       g_allocator.config.allocated_block_count);
        	write(g_allocator.config.file_fd, buffer, len);
		}
		else
			ft_putstr_fd(g_allocator.config.file_fd, "\nNo memory leaks detected\n");
		
		ft_putstr_fd(g_allocator.config.file_fd, "=========================\n");
	}

	if (g_allocator.config.file_fd != STDERR_FILENO && g_allocator.config.file_fd != -1)
		close(g_allocator.config.file_fd);
}

void	malloc_stats(void *ptr, size_t size)
{
	if (g_allocator.config.stats) {
		g_allocator.config.total_allocs++;
		g_allocator.config.allocated_block_count++;
		g_allocator.config.bytes_allocated += size;
	}

	if (g_allocator.config.verbose) {
		char	buffer[128];
		int	len = snprintf(buffer, sizeof(buffer), "[MALLOC] malloc(%zu) = %p\n", size, ptr);
		write(g_allocator.config.file_fd, buffer, len);
	}
}

void	free_stats(void *ptr, size_t size)
{
	if (g_allocator.config.stats) {
		g_allocator.config.total_frees++;
		g_allocator.config.allocated_block_count--;
		g_allocator.config.bytes_freed += size;
	}
	if (g_allocator.config.verbose) {
		char	buffer[128];
		int	len = snprintf(buffer, sizeof(buffer), "[MALLOC] free(%p) = %zu bytes\n", ptr, size);
		write(g_allocator.config.file_fd, buffer, len);
	}
}

void	realloc_stats(void *old_ptr, void *new_ptr, size_t old_size, size_t new_size)
{   
    if (g_allocator.config.verbose)
    {
        char buffer[256];
        int len = snprintf(buffer, sizeof(buffer), "[MALLOC] realloc(%p, %zu) = %p [was %zu bytes]\n",
						  old_ptr, new_size, new_ptr, old_size);
        write(g_allocator.config.file_fd, buffer, len);
    }
}