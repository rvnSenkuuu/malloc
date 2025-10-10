/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:06:25 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/10 11:34:58 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_H
#define MALLOC_INTERNAL_H

#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>

#define ALIGN 16

typedef enum {
	TINY,
	SMALL,
	LARGE,
} zone_type;

typedef struct __attribute__((aligned(ALIGN))) {
	bool	free;
	size_t	size;
	struct t_block	*next;
} t_block;

typedef struct __attribute__((aligned(ALIGN))) {
	zone_type	type;
	size_t	size;
	void	*start;
	t_block	*blocks;
	struct t_zone	*next;
} t_zone;

typedef struct __attribute__((aligned(ALIGN))) {
	t_zone	*tiny_zone;
	t_zone	*small_zone;
	t_zone	*large_zone;
} t_allocator;

#endif