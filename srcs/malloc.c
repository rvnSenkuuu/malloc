/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:05:28 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/10 18:50:58 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc_internal.h"

t_zone	g_zone = {0};

void	*malloc(size_t size)
{
	if (size <= 0) return NULL;

	return NULL;
}

void	free(void *ptr)
{
	(void)ptr;
}

void	*realloc(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	return NULL;
}