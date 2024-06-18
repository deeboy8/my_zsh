#include "dsh.h"

bool print_prompt() {
    my_printf("dsh>");
    return true;
}

//custom function taking capitalized text passed by user and changing to lowercase
char* string_tolower(char* string) {
    int len = strlen(string);
    char* string_tolower = malloc(sizeof(char) * len + 1);
    if (!string_tolower)
            return NULL;
    int index = 0;
    while (string[index]) {
        if (string[index] > 64 && string[index] < 91)
            string_tolower[index] = string[index] + 32;
        index++;
    }
    string_tolower[index] = '\0';
    return string_tolower;
}

bool is_equal(char* string1, char* string2) {
    assert(string1);
    assert(string2);
    return strcmp(string1, string2) == 0;
}

//recreation of c runtime libary fx strcmp()
int	my_strcmp(const char *s1, const char *s2)
{
	int index;
	index = 0;

    //loop over both strings checking each char
    //returns a numeric value reflective of difference or 0 if exact same
	while (s1[index] != '\0' && s2[index] != '\0')
	{
		if (s1[index] != s2[index])
			return (s1[index] - s2[index]);
		++index;
	}
	return ((unsigned char)s1[index] - (unsigned char)s2[index]);
}

//parses a line passed by user and tokenize 
char* my_strtok(char* path, char seperator) {
	static char* current_path = NULL;
	static char* captured_path = NULL;

	if (path != NULL) {
		current_path = path; // trailing pointer
		captured_path = path; // leading pointer
	}

	if (captured_path == NULL) {
		current_path = NULL;
	} else {
		current_path = captured_path;
		// loop across until captured_path hits seperator (delimiter)
		while (*captured_path != '\0' && *captured_path != seperator) {
			captured_path++;
		}
		assert(*captured_path == seperator || *captured_path == '\0');
		// replace seperator with null terminating char to tokenize portion between captured and current ptrs
		if (*captured_path == seperator) {
			*captured_path = '\0';
			captured_path++;
		} else if (*captured_path == '\0') {
			captured_path = NULL;
		} else {
			assert(false);
		}
	}
	return current_path;
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
	if (s1 && s2)
		p = my_strcat_zsh(p, s2, flag);

	return (p);
}