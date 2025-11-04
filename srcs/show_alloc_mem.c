/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 14:27:50 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/04 11:03:15 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

static size_t	ft_strlen(const char *s)
{
	size_t	len = 0;
	while (s[len])
		len++;
	return len;
}

static void	ft_putstr(const char *s)
{
	write(STDOUT_FILENO, s, ft_strlen(s));
}

void	ft_putstr_fd(int fd, const char *s)
{
	write(fd, s, ft_strlen(s));
}

void	ft_puthex(size_t addr)
{
	char	*base = "0123456789ABCDEF";

	if (addr >= 16)
		ft_puthex(addr / 16);
	write(STDOUT_FILENO, &base[addr % 16], sizeof(char));
}

void	ft_putnbr_fd(int fd, size_t n)
{
	int	i = 19;
	char	buf[21] = {0};

	while (n > 0) {
		buf[i] = (n % 10) + '0';
		--i;
		n /= 10;
	}
	
	write(fd, buf, sizeof(buf));
}

void	print_zone_info(t_zone *zone, const char *zone_type)
{
	ft_putstr(zone_type);
	ft_putstr(" : ");
	ft_putstr("0x");
	ft_puthex((size_t)zone);
	ft_putstr("\n");

	size_t total_bytes = 0;
	for (t_zone *current = zone; current; current = current->next) {
		for (t_block *blocks = current->blocks; blocks; blocks = blocks->next) {
			total_bytes += blocks->size;
			ft_putstr("0x");
			ft_puthex((size_t)blocks);
			ft_putstr(" - ");
			ft_putstr("0x");
			ft_puthex((size_t)blocks + blocks->size);
			ft_putstr(" : ");
			ft_putnbr_fd(STDOUT_FILENO, blocks->size);
			ft_putstr(" bytes\n");
		}
		ft_putstr("Total bytes: ");
		ft_putnbr_fd(STDOUT_FILENO, total_bytes);
		ft_putstr(" bytes\n");
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

void	print_block_header(t_block *block, int block_id)
{
	char	*block_status = block->free ? "FREE" : "ALLOCATED";
	dprintf(STDOUT_FILENO, "=================================================================\n");
	dprintf(STDOUT_FILENO, "| Block #%d | Status: %s | Size: %zu | Address: %p\n",
			block_id, block_status, block->size, block);
	dprintf(STDOUT_FILENO, "| Prev Block: %p | Next Block: %p\n", block->prev, block->next);
	dprintf(STDOUT_FILENO, "| User pointer: %p\n", GET_PTR_FROM_BLOCKS(block));
	dprintf(STDOUT_FILENO, "=================================================================\n");
}

void	print_block_hex_dump(void *ptr, size_t size)
{
	dprintf(STDOUT_FILENO, "Block Data: \n");

	size_t	i, j;
	char ascii[17] = {0};
	unsigned char	*data = (unsigned char *)ptr;
	
	for (i = 0; i < size; ++i) {
		dprintf(STDOUT_FILENO, "%02x ", data[i]);

		if (data[i] >= ' ' && data[i] <= '~')
			ascii[i % 16] = data[i];
		else
			ascii[i % 16] = '.';

		if ((i + 1) % 8 == 0 || i + 1 == size) {
			dprintf(STDOUT_FILENO, " ");

			if ((i+1) % 16 == 0)
				dprintf(STDOUT_FILENO, "|  %s \n", ascii);
			else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				
				if ((i + 1) % 16 <= 8)
					dprintf(STDOUT_FILENO, " ");

				for (j = (i + 1) % 16; j < 16; ++j)
					dprintf(STDOUT_FILENO, "   ");

				dprintf(STDOUT_FILENO, "|  %s \n", ascii);
			}
		}
	}
	dprintf(STDOUT_FILENO, "=================================================================\n");
}

void	print_zone_ex(t_zone *zone, const char *zone_type)
{
	int	block_id = 0;
	for (t_zone *current = zone; current; current = current->next) {
		dprintf(STDOUT_FILENO, "--------------------------------------------------\n");
		dprintf(STDOUT_FILENO, " Zone Type: %s | Zone Addr: %p\n Zone Used Size: %.1f%% | Next Zone Addr: %p\n",
			zone_type, current, (current->used_size * 100.0) / current->size, current->next);
		dprintf(STDOUT_FILENO, "--------------------------------------------------\n");

		for (t_block *block = zone->blocks; block; block = block->next) {
			++block_id;
			print_block_header(block, block_id);
			print_block_hex_dump(GET_PTR_FROM_BLOCKS(block), block->size);
			if (block->next) {
				dprintf(STDOUT_FILENO, "                            |                   \n");
				dprintf(STDOUT_FILENO, "                            v                   \n");
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