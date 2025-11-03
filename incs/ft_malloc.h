/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:28:01 by tkara2            #+#    #+#             */
/*   Updated: 2025/11/03 17:39:44 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
#define FT_MALLOC_H

#include "malloc_internal.h"

void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);

#endif