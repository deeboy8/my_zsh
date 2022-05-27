#include "dsh.h"

char* my_strcpy(char* dest, const char* src) {
	int index;

	index = 0;
	while (src[index] != '\0') {
		dest[index] = src[index];
		index++;
	}
	dest[index] = '\0';
	return (dest);
}

size_t	my_strlen(const char* str) {
	int index;

	index = 0;
	while (str[index] != '\0')
		index++;
	return (index);
}

char* my_strdup(const char* s1) {
	char* dup;

	dup = (char*)malloc(sizeof(*dup) * ((my_strlen(s1)) + 1));
	if (dup == NULL)
		return (NULL);
	else
		my_strcpy(dup, s1);
	return (dup);
}

void my_puts(char const* s) {
	while (*s)
		write(1, s++, 1);
	write(1, "\n", 2);
}

/*size_t	my_strlen(const char* str) {
	int index;

	index = 0;
	while (str[index] != '\0')
		index++;
	return (index);
}

char* my_strcpy(char* dest, const char* src) {
	int index;

	index = 0;
	while (src[index] != '\0') {
		dest[index] = src[index];
		index++;
	}
	dest[index] = '\0';
	return (dest);
}*/

char* my_strcat_zsh(char* s1, const char* s2, bool flag) {
	int i = 0, j = 0;

	// loop to end of first string
	while (s1[i] != '\0') {
		++i;
	}

    if (flag == true) {
        s1[i++] = '/';
    } else {
        s1[i++] = '=';
    }
	
	// add s2 to end of string 1
	while (s2[j] != '\0') {
		s1[i] = s2[j];
		j++, i++;
	}
	s1[i] = '\0';

	return (s1);
}

char* my_strjoin(char const* s1, char const* s2, bool flag) {
	if (s1 == NULL) {
		return NULL;
	}
	int len = 0;
	len = my_strlen(s1) + my_strlen(s2);
	char* p = malloc(sizeof(char) * (len + 1) + 1);
	if (p == NULL)
		return NULL;
	if (s1)
		p = my_strcpy(p, s1);
	// else
		// p = my_strcpy(p, s2);
	if (s1 && s2)
		p = my_strcat_zsh(p, s2, flag);

	return (p);
}