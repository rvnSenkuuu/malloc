#ifdef USE_FT_LIB
	#include "ft_malloc.h"
#else
	#include <unistd.h>
	#include <stdlib.h>
#endif

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

int	main()
{
	srand(time(NULL));

	PUTS("=====TEST BASIC ALLOCATION=====\n")
	
	int	upper_bound, lower_bound;
	size_t	sz;
	
	sz = rand() % 32;
	char	*tiny_ptr = malloc(sz);
	if (tiny_ptr)
		memset(tiny_ptr, 0x2a, sz);

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

	PUTS("=====TEST FT_SPLIT=====\n")
	char	*str = "Test str for split function";
	char	**ptrs = ft_split(str, ' ');
	if (!ptrs)
		PUTS("ft_split failed\n")

	PUTS("=====TEST REALLOC=====\n")

	size_t	old_sz = 256;
	char	*p = malloc(old_sz);
	if (p) {
		for (size_t i = 0; i < old_sz; i++)
			p[i] = i;
	}

	size_t	new_sz = old_sz * 2;
	p = realloc(p, new_sz);
	if (p) {
		memset(p, 0, new_sz);
		char	*to_fill = "realloc";
		size_t	i = 0;
		size_t	len = strlen(to_fill);
		size_t	count = new_sz / len;
		while (count--) {
			strncat(&p[i], to_fill, len);
			i += len;
		}
	}
	show_alloc_mem_ex();

	free_ptrs((void **)ptrs);
	free(tiny_ptr);
	free(small_ptr);
	free(large_ptr);
	free(p);
	return 0;
}