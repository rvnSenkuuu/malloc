#include <stdio.h>

#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <stdlib.h>
#endif

int	main(void)
{
	int	*ptr = malloc(5);
	(void)ptr;

	show_alloc_mem();

	// for (int i = 0; i < 5; i++)
	// 	ptr[i] = i;

	// for (int i = 0; i < 5; i++)
	// 	printf("%d\n", ptr[i]);

	// free(ptr);
	return 0;
}