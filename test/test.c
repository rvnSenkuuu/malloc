#include <stdio.h>

#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <unistd.h>
	#include <stdlib.h>
#endif

void	*test_ptr_allocation(void *ptr, size_t size)
{
	ptr = malloc(size);
	if (!ptr)
		return NULL;
	for (size_t i = 0; i < size / sizeof(*ptr); i++)
		*(char *)(ptr + i) = i;

	return ptr;
}

int	main(void)
{
	char	*ptr = NULL;
	int	*ptr2 = NULL;
	int	*ptr3 = NULL;
	int	*ptr4 = NULL;
	test_ptr_allocation(ptr, 100);
	test_ptr_allocation(ptr2, 10);
	test_ptr_allocation(ptr3, 10);
	test_ptr_allocation(ptr4, 1024);

	show_alloc_mem();
	// free(ptr);
	return 0;
}