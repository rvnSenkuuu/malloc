/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 14:27:50 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/04 15:26:05 by tkara2           ###   ########.fr       */
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

void	block_header(t_block *block, int block_id, char *buffer, size_t buffer_size)
{
	size_t	offset = ft_strlen(buffer);
	char	*block_status = block->free ? "FREE" : "ALLOCATED";
	
	offset += snprintf(buffer + offset, buffer_size - offset, 
		"=================================================================\n");
	
	offset += snprintf(buffer + offset, buffer_size - offset, 
		"| Block #%d | Status: %s | Size: %zu | Address: %p\n",
		block_id, block_status, block->size, block);

	offset += snprintf(buffer + offset, buffer_size - offset, 
		"| Prev Block: %p | Next Block: %p\n", block->prev, block->next);

	offset += snprintf(buffer + offset, buffer_size - offset, 
		"| User pointer: %p\n", GET_PTR_FROM_BLOCKS(block));

	offset += snprintf(buffer + offset, buffer_size - offset, 
		"=================================================================\n");
}

void	block_hex_dump(void *ptr, size_t size, char *buffer, size_t buffer_size)
{
	size_t	i, j;
	char ascii[17] = {0};
	size_t	offset = ft_strlen(buffer);
	unsigned char	*data = (unsigned char *)ptr;

	offset += snprintf(buffer + offset, buffer_size - offset, "Block Data: \n");
	for (i = 0; i < size; ++i) {
		offset += snprintf(buffer + offset, buffer_size - offset, "%02x ", data[i]);

		if (data[i] >= ' ' && data[i] <= '~')
			ascii[i % 16] = data[i];
		else
			ascii[i % 16] = '.';

		if ((i + 1) % 8 == 0 || i + 1 == size) {
			offset += snprintf(buffer + offset, buffer_size - offset, " ");

			if ((i + 1) % 16 == 0)
				offset += snprintf(buffer + offset, buffer_size - offset, "|  %s \n", ascii);

			else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				
				if ((i + 1) % 16 <= 8)
					offset += snprintf(buffer + offset, buffer_size - offset, " ");

				for (j = (i + 1) % 16; j < 16; ++j)
					offset += snprintf(buffer + offset, buffer_size - offset, "   ");
				offset += snprintf(buffer + offset, buffer_size - offset, "|  %s \n", ascii);
			}
		}
	}
}

void	print_zone_ex(t_zone *zone, const char *zone_type)
{
	int	block_id = 0;
	char	buffer[BUFFER_SIZE] = {0};
	for (t_zone *current = zone; current; current = current->next) {
		size_t	offset = strlen(buffer);

		offset += snprintf(buffer + offset, BUFFER_SIZE - offset, 
			"\n--------------------------------------------------\n");

		offset += snprintf(buffer + offset, BUFFER_SIZE - offset, " Zone Type: %s | Zone Addr: %p\n Zone Used Size: %.1f%% | Next Zone Addr: %p\n",
			zone_type, current, (current->used_size * 100.0) / current->size, current->next);

		offset += snprintf(buffer + offset, BUFFER_SIZE - offset, 
			"--------------------------------------------------\n");

		write(g_allocator.config.file_fd, buffer, ft_strlen(buffer));

		for (t_block *block = current->blocks; block; block = block->next) {
			memset(buffer, 0, BUFFER_SIZE);
			offset = 0;

			++block_id;
			block_header(block, block_id, buffer, BUFFER_SIZE);

			size_t	dump_size = block->size;
			if (dump_size > 256)
				dump_size = 256;
			
			block_hex_dump(GET_PTR_FROM_BLOCKS(block), dump_size, buffer, BUFFER_SIZE);
			if (block->size > 256) {
				offset = strlen(buffer);
				offset += snprintf(buffer + offset, BUFFER_SIZE - offset,
					"... (showing first 256 of %zu bytes)\n", block->size);
			}
			offset += snprintf(buffer + offset, BUFFER_SIZE - offset, 
				"=================================================================\n");

			if (block->next) {
				offset = ft_strlen(buffer);
				offset += snprintf(buffer + offset, BUFFER_SIZE - offset, 
					"                            |                   \n");
				offset += snprintf(buffer + offset, BUFFER_SIZE - offset, 
					"                            v                   \n");
			}
		}
		write(g_allocator.config.file_fd, buffer, ft_strlen(buffer));
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