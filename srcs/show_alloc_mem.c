/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 14:27:50 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/30 16:53:10 by tkara2           ###   ########.fr       */
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

void	ft_puthex(size_t addr)
{
	char	*base = "0123456789ABCDEF";

	if (addr >= 16)
		ft_puthex(addr / 16);
	write(STDOUT_FILENO, &base[addr % 16], sizeof(char));
}

void	ft_putnbr(size_t n)
{
	int	i = 19;
	char	buf[21] = {0};

	while (n > 0) {
		buf[i] = (n % 10) + '0';
		--i;
		n /= 10;
	}
	
	write(STDOUT_FILENO, buf, sizeof(buf));
}

void	print_zone_info(t_zone *zone)
{
	char	*buf;
	switch (zone->type) {
		case TINY:
			buf = "TINY";
			break;
		case SMALL:
			buf = "SMALL";
			break;
		case LARGE:
			buf = "LARGE";
			break;
		default: break;
	}
	ft_putstr(buf);
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
			ft_putnbr(blocks->size);
			ft_putstr(" bytes\n");
			blocks->free ? ft_putstr("Block is free\n") : ft_putstr("Block is not free\n");
		}
		ft_putstr("Zone size: ");
		ft_putnbr(zone->size);
		ft_putstr(" Bytes\n");
		ft_putstr("Total bytes: ");
		ft_putnbr(total_bytes);
		ft_putstr(" bytes\n");
		ft_putstr("\n");
	}
}

void	show_alloc_mem(void)
{
	if (g_allocator.tiny)
		print_zone_info(g_allocator.tiny);
	if (g_allocator.small)
		print_zone_info(g_allocator.small);
	if (g_allocator.large)
		print_zone_info(g_allocator.large);
}