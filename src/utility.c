#include "dsh.h"

//print shell prompt to stdout
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

//using C run time libary strcmp()
//comparing two strings for equivalency
bool is_equal(char* string1, char* string2) {
    assert(string1);
    assert(string2);
    return strcmp(string1, string2) == 0;
}

//custom string comparison function similar to C runtime libary fx strcmp()
int	my_strcmp(const char *s1, const char *s2) {
	int index;
	index = 0;

    //loop over both strings checking each char
    //returns a numeric value reflective of difference or 0 if exact same
	while (s1[index] != '\0' && s2[index] != '\0') {
		if (s1[index] != s2[index])
			return (s1[index] - s2[index]);
		++index;
	}

	return ((unsigned char)s1[index] - (unsigned char)s2[index]);
}

//parses a line passed by user and tokenizing each portion identified by delimiter/seperator
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

//custom function recreating C run time libary fx strcpy()
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

//custom function recreating C run time libary fx strlen()
size_t	my_strlen(const char* str) {
	int index;

	index = 0;
	while (str[index] != '\0')
		index++;
	return (index);
}

//custom function recreating C run time libary fx strdup()  
char* my_strdup(const char* s1) {
	char* dup;

    //invoke malloc to allocated strlen of string 1 onto heap
    //+1 references adding 1 byte of memory space for null terminating char
	dup = (char*)malloc(sizeof(*dup) * ((my_strlen(s1)) + 1));
	if (dup == NULL)
		return (NULL);
	else
		my_strcpy(dup, s1);
	return (dup);
}

//custom function recreating C run time libary fx puts()
void my_puts(char const* s) {
	while (*s)
		write(1, s++, 1);
	write(1, "\n", 2);
}

//custom function recreating use of strcat() found in C runtime library
char* my_strcat_zsh(char* s1, const char* s2, bool flag) {
	int i = 0, j = 0;

	// loop to end of first string 1
	while (s1[i] != '\0') {
		++i;
	}

    //based on flag passed, will either concatenate two strings with a '/' or '='
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

//custom fx combining two strings similar to C runtime libary fx strjoin()
//implements unique feature of accepting a flag to dictate what char will join two strings
char* my_strjoin(char const* s1, char const* s2, bool flag) {
	if (s1 == NULL) {
		return NULL;
	}
	int len = 0;
	len = my_strlen(s1) + my_strlen(s2);
    //use malloc() to allocate heap spae for final string
    //
	char* p = malloc(sizeof(char) * (len + 1) + 1);
	if (p == NULL)
		return NULL;
	if (s1)
		p = my_strcpy(p, s1);
	if (s1 && s2)
		p = my_strcat_zsh(p, s2, flag);

	return (p);
}