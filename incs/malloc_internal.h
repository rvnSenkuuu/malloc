/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkara2 <tkara2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:06:25 by tkara2            #+#    #+#             */
/*   Updated: 2025/10/10 18:50:41 by tkara2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_H
#define MALLOC_INTERNAL_H

#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

#define ALIGNMENT 16
#define ALIGN(x, to_align) ((x) + (to_align) - 1) & ~((to_align) - 1)

#define TINY_BLOCK_SIZE 256
#define SMALL_BLOCK_SIZE 1024

typedef enum {
	TINY,
	SMALL,
	LARGE,
} zone_type;

typedef struct __attribute__((aligned(ALIGNMENT))) {
	bool	free;
	size_t	size;
	struct t_block	*next;
} t_block;

typedef struct __attribute__((aligned(ALIGNMENT))) {
	zone_type	type;
	size_t	size;
	void	*start;
	t_block	*blocks;
	struct t_zone	*next;
} t_zone;

#endif