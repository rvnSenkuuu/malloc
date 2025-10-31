#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <unistd.h>
	#include <stdlib.h>
#endif

// void	*test_ptr_allocation(size_t size)
// {
// 	void *ptr = malloc(size);
// 	if (!ptr)
// 		return NULL;

// 	unsigned char *addr = ptr;
// 	for (size_t i = 0; i < size / sizeof(*ptr); i++)
// 		addr[i] = i;

// 	return ptr;
// }

// int	main(void)
// {
// 	__attribute__((unused))int	*ptr1 = test_ptr_allocation(40);
// 	__attribute__((unused))int	*ptr2 = test_ptr_allocation(50);
// 	__attribute__((unused))int	*ptr3 = test_ptr_allocation(10);
// 	__attribute__((unused))int	*ptr4 = test_ptr_allocation(25);
// 	__attribute__((unused))int	*ptr5 = test_ptr_allocation(100);

// 	write(1, "===Before free===\n", 19);
// 	show_alloc_mem();
// 	free(ptr1);
// 	free(ptr2);
// 	free(ptr3);
// 	free(ptr4);
// 	free(ptr5);
// 	write(1, "===After free===\n", 18);
// 	show_alloc_mem();

// 	__attribute__((unused))int	*ptr6 = test_ptr_allocation(35);
// 	write(1, "===After===\n", 13);
// 	show_alloc_mem();

// 	free(ptr6);
	
// 	return 0;
// }

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
#include <stdio.h>

int	main(void)
{
	size_t	sz = 5;
	size_t	new_sz = 10;
	char	*s = malloc ((sz + 1) * sizeof(char));
	if (!s) {
		perror("malloc fail\n");
		return 1;
	}

	memset(s, 'c', sz);
	s[sz + 1] = '\0';
	printf("%s | %lu\n", s, strlen(s));

	realloc(s, new_sz);
	memset(s, 't', new_sz);
	printf("%s | %lu\n", s, strlen(s));

	free(s);
	return 0;
}