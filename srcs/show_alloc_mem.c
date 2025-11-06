/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 14:27:50 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/06 10:38:46 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

void	print_zone_info(t_zone *zone, const char *zone_type)
{
	ft_putstr_fd(g_allocator.config.file_fd, zone_type);
	ft_putstr_fd(g_allocator.config.file_fd, " : ");
	ft_putstr_fd(g_allocator.config.file_fd, "0x");
	ft_puthex_fd(g_allocator.config.file_fd, (size_t)zone);
	ft_putstr_fd(g_allocator.config.file_fd, "\n");

	size_t total_bytes = 0;
	for (t_zone *current = zone; current; current = current->next) {
		for (t_block *blocks = current->blocks; blocks; blocks = blocks->next) {
			total_bytes += blocks->size;
			ft_putstr_fd(g_allocator.config.file_fd, "0x");
			ft_puthex_fd(g_allocator.config.file_fd, (size_t)blocks);
			ft_putstr_fd(g_allocator.config.file_fd, " - ");
			ft_putstr_fd(g_allocator.config.file_fd, "0x");
			ft_puthex_fd(g_allocator.config.file_fd, (size_t)blocks + blocks->size);
			ft_putstr_fd(g_allocator.config.file_fd, " : ");
			ft_putnbr_fd(g_allocator.config.file_fd, blocks->size);
			ft_putstr_fd(g_allocator.config.file_fd, " bytes\n");
		}
		ft_putstr_fd(g_allocator.config.file_fd, "Total bytes: ");
		ft_putnbr_fd(g_allocator.config.file_fd, total_bytes);
		ft_putstr_fd(g_allocator.config.file_fd, " bytes\n");
	}
}

void	show_alloc_mem(void)
{
	pthread_mutex_lock(&mutex);

	if (g_allocator.tiny)
		print_zone_info(g_allocator.tiny, "TINY");
	if (g_allocator.small)
		print_zone_info(g_allocator.small, "SMALL");
	if (g_allocator.large)
		print_zone_info(g_allocator.large, "LARGE");

	pthread_mutex_unlock(&mutex);
}

void	zone_header(t_zone *zone, const char *zone_type)
{
	ft_dprintf(g_allocator.config.file_fd, 
		"\n--------------------------------------------------\n");

	ft_dprintf(g_allocator.config.file_fd, " Zone Type: %s | Zone Addr: %p\n Zone Used Size: %d bytes | Next Zone Addr: %p\n",
		zone_type, zone, zone->used_size, zone->next);

	ft_dprintf(g_allocator.config.file_fd, 
		"--------------------------------------------------\n");
}

void	block_header(t_block *block, int block_id)
{
	char	*block_status = block->free ? "FREE" : "ALLOCATED";
	
	ft_dprintf(g_allocator.config.file_fd, 
		"=================================================================\n");

	ft_dprintf(g_allocator.config.file_fd, 
		"| Block #%d | Status: %s | Size: %d bytes\n",
		block_id, block_status, block->size);

	ft_dprintf(g_allocator.config.file_fd, 
		"| Address: %p | Prev Block: %p | Next Block: %p\n",block, block->prev, block->next);

	ft_dprintf(g_allocator.config.file_fd, 
		"| User pointer: %p\n", GET_PTR_FROM_BLOCKS(block));

	ft_dprintf(g_allocator.config.file_fd, 
		"=================================================================\n");
}

void	block_hex_dump(void *ptr, size_t size)
{
	size_t	i, j;
	char ascii[17] = {0};
	unsigned char	*data = (unsigned char *)ptr;
	char	*base = "0123456789abcdef";

	ft_dprintf(g_allocator.config.file_fd, "Block Data: \n");
	for (i = 0; i < size; ++i) {

		char	hex[4] = {0};
		hex[0] = base[(data[i] >> 4) & 0x0F];
		hex[1] = base[data[i] & 0x0F];
		hex[2] = ' ';
		ft_dprintf(g_allocator.config.file_fd, "%s", hex);

		if (data[i] >= ' ' && data[i] <= '~')
			ascii[i % 16] = data[i];
		else
			ascii[i % 16] = '.';

		if ((i + 1) % 8 == 0 || i + 1 == size) {
			ft_dprintf(g_allocator.config.file_fd, " ");

			if ((i + 1) % 16 == 0)
				ft_dprintf(g_allocator.config.file_fd, "| %s \n", ascii);

			else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				
				if ((i + 1) % 16 <= 8)
					ft_dprintf(g_allocator.config.file_fd, " ");

				for (j = (i + 1) % 16; j < 16; ++j)
					ft_dprintf(g_allocator.config.file_fd, "   ");

				ft_dprintf(g_allocator.config.file_fd, "|  %s \n", ascii);
			}
		}
	}
}

void	print_zone_ex(t_zone *zone, const char *zone_type)
{
	int	block_id = 0;
	for (t_zone *current = zone; current; current = current->next) {

		zone_header(current, zone_type);
		for (t_block *block = current->blocks; block; block = block->next) {

			++block_id;
			block_header(block, block_id);

			size_t	dump_size = block->size;
			if (dump_size > 256)
				dump_size = 256;
			
			block_hex_dump(GET_PTR_FROM_BLOCKS(block), dump_size);
			if (block->size > 256)
				ft_dprintf(g_allocator.config.file_fd, "... (showing first 256 of %zu bytes)\n", block->size);
			ft_dprintf(g_allocator.config.file_fd, "=================================================================\n");

			if (block->next) {
				ft_dprintf(g_allocator.config.file_fd, "                            |                   \n");
				ft_dprintf(g_allocator.config.file_fd, "                            v                   \n");
			}
		}
	}
}

void	show_alloc_mem_ex(void)
{
	pthread_mutex_lock(&mutex);
	
	if (g_allocator.tiny)
		print_zone_ex(g_allocator.tiny, "TINY");
	if (g_allocator.small)
		print_zone_ex(g_allocator.small, "SMALL");
	if (g_allocator.large)
		print_zone_ex(g_allocator.large, "LARGE");

	pthread_mutex_unlock(&mutex);
}