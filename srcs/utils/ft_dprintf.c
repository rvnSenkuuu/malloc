/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 15:34:34 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/05 15:34:36 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

int	ft_dpf_putstr(int fd, const char *s)
{
	if (!s)
		return (write(fd, "(null)", 6));
	return (write(fd, s, ft_strlen(s)));
}

int	ft_dpf_putchar(int fd, char c)
{
	return (write(fd, &c, sizeof(char)));
}

int	ft_dpf_putaddr(int fd, unsigned long long int ptr)
{
	int	len;
	char	*base;

	base = "0123456789abcdef";
	len = 0;
	if (!ptr)
		return (ft_dpf_putstr(fd, "(nil)"), 5);
	if (ptr >= 16)
		len += ft_dpf_putaddr(fd, ptr / 16);
	len += ft_dpf_putchar(fd, base[ptr % 16]);
	return len;
}

int	ft_dpf_putnbr(int fd, size_t n)
{
	int	i = 19;
	char	buf[21] = {0};

	while (n > 0) {
		buf[i] = (n % 10) + '0';
		--i;
		n /= 10;
	}
	
	write(fd, buf, sizeof(buf));
	return sizeof(buf);
}

int	ft_dpf_putnbr_hex(int fd, unsigned long long int n, char format)
{
	int	len;
	char	*base;

	len = 0;
	if (format == 'X')
		base = "0123456789ABCDEF";
	else
		base = "0123456789abcdef";
	if (n >= 16)
		len += ft_dpf_putnbr_hex(fd, n / 16, format);
	len += ft_dpf_putchar(fd, base[n % 16]);
	return len;
}

int	ft_dpf_putnbr_uint(int fd, unsigned int n)
{
	unsigned int	nb;
	int	len;

	len = 0;
	nb = n;
	if (nb > 9)
		len += ft_dpf_putnbr_uint(fd, nb / 10);
	len += ft_dpf_putchar(fd, (nb % 10) + '0');
	return len;
}

static int	ft_dpf_format_checker(int fd, char format, va_list args)
{
	int	count = 0;
	unsigned long long int	ptr;

	if (format == 'c')
		count += ft_dpf_putchar(fd, va_arg(args, int));
	else if (format == 's')
		count += ft_dpf_putstr(fd, va_arg(args, const char *));
	else if (format == 'd' || format == 'i')
		count += ft_dpf_putnbr(fd, va_arg(args, int));
	else if (format == 'u')
		count += ft_dpf_putnbr_uint(fd, va_arg(args, unsigned int));
	else if (format == 'x' || format == 'X')
		count += ft_dpf_putnbr_hex(fd, va_arg(args, unsigned int), format);
	else if (format == 'p')
	{
		ptr = va_arg(args, unsigned long long int);
		if (ptr)
			count += ft_dpf_putstr(fd, "0x");
		count += ft_dpf_putaddr(fd, ptr);
	}
	else if (format == '%')
		count += ft_dpf_putchar(fd, '%');
	return count;
}

int	ft_dprintf(int fd, const char *format, ...)
{
	int		i = 0;
	int		return_value = 0;
	va_list	args;

	if (!format)
		return (-1);
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%')
		{
			return_value += ft_dpf_format_checker(fd, format[i + 1], args);
			i++;
		}
		else
			return_value += ft_dpf_putchar(fd, format[i]);
		i++;
	}
	va_end(args);
	return return_value;
}