#include <stdio.h>

#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <unistd.h>
	#include <stdlib.h>
#endif

void	*test_ptr_allocation(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
		return NULL;

	unsigned char *addr = ptr;
	for (size_t i = 0; i < size / sizeof(*ptr); i++)
		addr[i] = i;

	return ptr;
}

int	main(void)
{
	int	*ptr2 = test_ptr_allocation(25);
	int	*ptr3 = test_ptr_allocation(15);
	int	*ptr4 = test_ptr_allocation(5);

	(void)ptr2;
	(void)ptr3;
	(void)ptr4;

	show_alloc_mem();
	free(ptr2);
	return 0;
}