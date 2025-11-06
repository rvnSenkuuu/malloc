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

void	free_ptrs(void **ptr)
{
	size_t	i = 0;
	while (ptr && ptr[i])
	{
		free(ptr[i]);
		ptr[i] = NULL;
		i++;
	}
	free(ptr);
	ptr = NULL;
}

static size_t	ft_issep(char c, char sep)
{
	if (c == sep || c == '\0')
		return 1;
	return 0;
}

static size_t	countword(char const *s, char c)
{
	size_t	i = 0;
	size_t	count = 0;

	while (s[i] && ft_issep(s[i], c))
		i++;
	while (s[i])
	{
		if ((!ft_issep(s[i], c) && ft_issep(s[i + 1], c))
			|| (!s[i + 1] && !ft_issep(s[i], c)))
			count++;
		i++;
	}
	return count;
}

static char	*ft_strdupsep(char const *s, char c)
{
	size_t	i = 0;
	size_t	j = 0;
	char	*dup;

	while (!ft_issep(s[j], c))
		j++;
	dup = malloc(sizeof(char) * (j + 1));
	if (!dup)
		return NULL;
	while (i < j)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return dup;
}

char	**ft_split(char const *s, char c)
{
	size_t	i = 0;
	size_t	j = 0;
	char	**output;

	if (!s)
		return NULL;
	output = malloc(sizeof(char *) * (countword(s, c) + 1));
	if (!output)
		return NULL;
	while (s[i])
	{
		if (ft_issep(s[i], c))
			i++;
		else {
			output[j] = ft_strdupsep(&s[i], c);
			if (!output[j]) {
				free_ptrs((void **)output);
				return NULL;
			}
			i += ft_strlen(output[j]);
			j++;
		}
	}
	output[j] = NULL;
	return output;
}

int	main(void)
{
	srand(time(NULL));

	PUTS("=====TEST BASIC ALLOCATION=====\n")
	
	int	upper_bound, lower_bound;
	size_t	sz;
	
	sz = rand() % 32;
	char	*tiny_ptr = malloc(sz);
	if (tiny_ptr)
		memset(tiny_ptr, 0x2a, sz);

	PUTS("=====TEST FT_SPLIT=====\n")
	char	*str = "Test str for split function";
	char	**ptrs = ft_split(str, ' ');
	if (!ptrs)
		PUTS("ft_split failed\n")

	lower_bound = 33;
	upper_bound = 512;
	sz = rand() % (upper_bound - lower_bound + 1) + lower_bound;
	int	*small_ptr = malloc(sz);
	if (small_ptr)
		memset(small_ptr, 0x2f, sz);

	lower_bound = 513;
	upper_bound = 8096;
	sz = rand() % (upper_bound - lower_bound + 1) + lower_bound;
	void	*large_ptr = malloc(sz);
	if (large_ptr)
		memset(large_ptr, 0x40, sz);

	show_alloc_mem_ex();

	free_ptrs((void **)ptrs);
	free(tiny_ptr);
	free(small_ptr);
	free(large_ptr);
	return 0;
}