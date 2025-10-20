/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 14:27:50 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/20 19:35:26 by tkara2           ###   ########.fr       */
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
	ft_putstr("0x");
	ft_puthex((size_t)zone);
	ft_putstr(" - ");
	ft_putstr("0x");
	ft_puthex((size_t)zone + zone->size);
	ft_putstr(" : ");
	ft_putnbr(zone->size);
	ft_putstr(" bytes\n");
}

void	show_alloc_mem(void)
{
	ft_putstr("LARGE: \n");
	t_zone	*current = g_allocator.large;

	while (current) {
		print_zone_info(current);
		current = current->next;
	}
}