#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <unistd.h>
	#include <stdlib.h>
#endif

// #include <pthread.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>

// #define NUM_THREADS 10
// #define OPERATIONS 10

// void *test_simple(void *arg)
// {
// 	int	id = *(int *)arg;
    
// 	for (int i = 0; i < OPERATIONS; i++) {
// 		void *ptr = malloc(100);
// 		if (ptr) {
// 			memset(ptr, id, 100);
// 			free(ptr);
// 		}
// 	}
    
// 	printf("Thread %d terminé\n", id);
// 	return NULL;
// }

// int main(void)
// {
// 	printf("=== TEST THREAD-SAFETY MALLOC ===\n\n");
// 	printf("Lancement de %d threads...\n", NUM_THREADS);
    
// 	int	ids[NUM_THREADS];
// 	pthread_t	threads[NUM_THREADS];
    
// 	for (int i = 0; i < NUM_THREADS; i++) {
// 		ids[i] = i;
// 		pthread_create(&threads[i], NULL, test_simple, &ids[i]);
// 	}

// 	for (int i = 0; i < NUM_THREADS; i++)
// 		pthread_join(threads[i], NULL);
    
// 	printf("\n✓ TEST RÉUSSI : Pas de crash !\n");
// 	printf("Si le programme se termine normalement, ton malloc est thread-safe.\n");
    
// 	return 0;
// }

#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define PUTS(s) write(STDOUT_FILENO, s, strlen(s));

// bool	test_basic_allocation(bool display)
// {
// 	srand(time(NULL));
	
// 	int	upper_bound, lower_bound;
// 	size_t	sz;
	
// 	sz = rand() % 32;
// 	char	*tiny_ptr = malloc(sz);
// 	if (!tiny_ptr)
// 		return false;
// 	memset(tiny_ptr, 0x2a, sz);

// 	lower_bound = 33;
// 	upper_bound = 512;
// 	sz = rand() % (upper_bound - lower_bound + 1) + lower_bound;
// 	int	*small_ptr = malloc(sz);
// 	if (!small_ptr)
// 		return false;
// 	memset(small_ptr, 0x2f, sz);

// 	lower_bound = 513;
// 	upper_bound = 8096;
// 	sz = rand() % (upper_bound - lower_bound + 1) + lower_bound;
// 	void	*large_ptr = malloc(sz);
// 	if (!large_ptr)
// 		return false;
// 	memset(large_ptr, 0x40, sz);

// 	if (display)
// 		show_alloc_mem_ex();

// 	free(tiny_ptr);
// 	free(small_ptr);
// 	free(large_ptr);
// 	return true;
// }

// bool	test_reuse_allocated_block(bool display)
// {

// 	size_t	sz = 128;
// 	char	*ptr = malloc(sz);
// 	if (!ptr)
// 		return false;
// 	memset(ptr, 0x42, sz);

// 	if (display)
// 		show_alloc_mem_ex();

// 	free(ptr);

// 	PUTS("=====AFTER FREE=====");

// 	if (display)
// 		show_alloc_mem_ex();

// 	char	*ptr2 = malloc(sz);
// 	if (!ptr2)
// 		return false;
// 	memset(ptr2, 0x43, sz);

// 	char	*ptr3 = malloc(sz);
// 	if (!ptr3)
// 		return false;
// 	memset(ptr3, 0x44, sz);

// 	PUTS("=====AFTER MALLOC=====");

// 	if (display)
// 		show_alloc_mem_ex();	

// 	return true;
// }

// int	main(void)
// {
// 	if (!test_basic_allocation(false))
// 		PUTS("Basic allocation test failed")

// 	if (!test_reuse_allocated_block(true))
// 		PUTS("Reuse allocated block test failed")

// 	PUTS("All test passed\n")
// 	return 0;
// }