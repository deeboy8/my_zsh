#include "dsh.h"

// static dsh_environment_t* dsh_environment = NULL;

char* my_strchr(const char *s, int c) {
	char cha = (char)c;
	while (*s) {
		if (*s == cha)
			return ((char *)s);
		s++;
	}
	if (cha == '\0')
		return ((char *)s);
	return (NULL);
}

static int get_variable_count(char* env[]) {
    assert(env);
    int count = 0;
    while (*env++) count++;

    return count;
}

// copy env to memory
// allows access to env outside of main function
bool dsh_allocate_environment(char* env[]) {
    assert(dsh_environment == NULL);
    // allocate 
    int env_var_count = get_variable_count(env);
    assert(env_var_count > 0);
    dsh_environment = malloc(sizeof(dsh_environment_t) + (sizeof(dsh_env_var_t) * (env_var_count + 1)));
    assert(dsh_environment);

    // initiailize
    dsh_environment->count = env_var_count;
    for (size_t i = 0; i < dsh_environment->count; i++) {
        char* equal_sep = my_strchr(env[i], '=');
        assert(equal_sep);
        // terminate name string, and point ptr to value string 
        *equal_sep++ = '\0';
        // copy name
        dsh_environment->variables[i].name = my_strdup(env[i]);
        assert(dsh_environment->variables[i].name);
        // copy value
        dsh_environment->variables[i].value = my_strdup(equal_sep);
        assert(dsh_environment->variables[i].value);
    }

    return true;
}

char* dsh_getenv(const char* name) {
    assert(dsh_environment);
    assert(name);
    char* value = NULL;
    for (size_t i = 0; i < dsh_environment->count; i++) {
        if (my_strcmp(name, dsh_environment->variables[i].name) == 0) {
            value = dsh_environment->variables[i].value;
            break;
        }
    }
    return value;
}