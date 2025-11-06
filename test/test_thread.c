#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define PUTS(s) write(STDOUT_FILENO, s, strlen(s));

#define NUM_THREADS 10
#define OPERATIONS 10

void *simple_test(void *arg)
{
	int	id = *(int *)arg;
	for (int i = 0; i < OPERATIONS; i++) {
		void	*ptr = malloc(100);
		if (ptr) {
			memset(ptr, id, 100);
			free(ptr);
		}
	}

	printf("Thread %d finish\n", id);
	return NULL;
}

int	main(void)
{
	PUTS("=== TEST THREAD-SAFETY MALLOC ===\n\n")

	int	ids[NUM_THREADS];
	pthread_t	threads[NUM_THREADS];

	for (int i = 0; i < NUM_THREADS; i++) {
		ids[i] = i;
		pthread_create(&threads[i], NULL, simple_test, &ids[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);

	PUTS("\n TEST THREAD SUCCESS\n")
	return 0;
}
