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

// #include <unistd.h>
// #include <string.h>
// #include <stdio.h>

// int	main(void)
// {
// 	size_t	sz = 5;
// 	size_t	new_sz = 10;
// 	char	*s = malloc ((sz + 1) * sizeof(char));
// 	if (!s) {
// 		perror("malloc fail\n");
// 		return 1;
// 	}

// 	__attribute__((unused))int	*ptr3 = malloc(28);

// 	memset(s, 'c', sz);
// 	s[sz] = '\0';
// 	printf("%s | %lu\n", s, strlen(s));

// 	free(ptr3);
// 	show_alloc_mem();

// 	write(1, "-------\n", 8);

// 	s = realloc(s, new_sz);
// 	memset(s, 't', new_sz);
// 	printf("%s | %lu\n", s, strlen(s));

	
// 	free(s);
// 	show_alloc_mem();
// 	return 0;
// }

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Ton allocateur custom ici
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

int main(void)
{
    printf("=== Test realloc custom ===\n");

    // 1️⃣ Allocation simple
    char *a = malloc(10);
    strcpy(a, "Hello");
    printf("Initial: %s (ptr=%p)\n", a, (void*)a);

    // 2️⃣ Agrandir (sans merge possible)
    char *b = realloc(a, 20);
    printf("Agrandir (sans merge): %s (new ptr=%p)\n", b, (void*)b);

    // 3️⃣ Réduire la taille
    char *c = realloc(b, 5);
    printf("Réduire la taille: %s (ptr=%p)\n", c, (void*)c);

    // 4️⃣ Cas NULL
    char *d = realloc(NULL, 15);
    printf("Realloc(NULL): ptr=%p\n", (void*)d);

    // 5️⃣ Cas size == 0
    char *e = malloc(8);
    char *f = realloc(e, 0);
    printf("Realloc(size=0): ptr=%p (doit être NULL)\n", (void*)f);

    // 6️⃣ Test fusion (merge)
    // ⚠️ Dépend de ton implémentation interne (zones libres consécutives)
    // Simule une libération de bloc contigu pour forcer un merge
    char *m1 = malloc(32);
    char *m2 = malloc(32);
    strcpy(m1, "MergeTest");
    free(m2); // Libère le bloc suivant -> merge possible
    char *m3 = realloc(m1, 60); // Doit tenter un merge avec m2
    printf("Essai merge: %s (ptr=%p)\n", m3, (void*)m3);

    free(m3);
    free(d);
    printf("=== Fin des tests ===\n");

    return 0;
}
