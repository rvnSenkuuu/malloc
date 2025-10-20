#include <stdio.h>

#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <unistd.h>
	#include <stdlib.h>
#endif

int	main(void)
{
	int	*ptr = malloc(2048);

	for (size_t i = 0; i < 2048 / sizeof(int); i++)
		ptr[i] = i;
		
	int	*ptr1 = malloc(3000);
		
	for (size_t i = 0; i < 3000 / sizeof(int); i++)
		ptr1[i] = i;
		
	show_alloc_mem();
	// free(ptr);
	return 0;
}