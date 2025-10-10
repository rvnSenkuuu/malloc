/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 14:27:50 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/10 18:52:21 by tkara2           ###   ########.fr       */
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

void	show_alloc_mem(void)
{
	ft_putstr("TODO : show_alloc_mem");
}