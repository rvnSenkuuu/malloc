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
	__attribute__((unused))int	*ptr2 = test_ptr_allocation(50);
	__attribute__((unused))int	*ptr3 = test_ptr_allocation(10);
	__attribute__((unused))int	*ptr4 = test_ptr_allocation(25);

	write(1, "===Before free===\n", 19);
	show_alloc_mem();
	free(ptr2);
	free(ptr3);
	free(ptr4);
	write(1, "===After free===\n", 18);
	show_alloc_mem();
	return 0;
}