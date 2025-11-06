/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 10:32:45 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/06 10:40:41 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

void	ft_memcpy(void *d, const void *s, size_t n)
{
	unsigned char	*src = (unsigned char *)s;
	unsigned char	*dest = d;

	while (n--)
		*dest++ = *src++;
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	while (s1 && s2 && *s1 == *s2 && n--) {
		s1++;
		s2++;
	}

	return (unsigned char)*s1 - (unsigned char)*s2;
}

size_t	ft_strlen(const char *s)
{
	size_t	len = 0;
	while (s[len])
		len++;
	return len;
}

void	ft_putstr_fd(int fd, const char *s)
{
	write(fd, s, ft_strlen(s));
}

void	ft_puthex_fd(int fd, size_t addr)
{
	char	*base = "0123456789ABCDEF";

	if (addr >= 16)
		ft_puthex_fd(fd, addr / 16);
	write(fd, &base[addr % 16], sizeof(char));
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