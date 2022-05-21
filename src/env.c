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

const char* dsh_enumerate_env_var(const char* name, const char* delimiter) {
    static char* env_var_name = NULL;
    char* value = NULL;
    if (env_var_name == NULL || name != NULL) {
        free(env_var_name);
        // get value of PATH env var
        env_var_name = dsh_getenv(name);
        assert(env_var_name);
        env_var_name = strdup(env_var_name);
        assert(env_var_name);
        value = strtok(env_var_name, delimiter);
    } else {
        value = strtok(NULL, delimiter);
    }

    return value;
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

// implement the 'env' command
bool dsh_env() { //command_line_t* command_line) {
    assert(dsh_environment);
    for(size_t i = 0; i < dsh_environment->count; i++) {
        // if(!is_null_present(&dsh_environment->variables[i])) {
            my_printf("%s=%s\n", dsh_environment->variables[i].name, dsh_environment->variables[i].value);
        //}
    }
    return true;
}

bool grow(dsh_environment_t* dsh_environment, int count) {
    assert(dsh_environment);
    dsh_environment_t* new_env = malloc(sizeof(dsh_environment) + (sizeof(dsh_env_var_t) * count + sizeof(dsh_env_var_t)));
    dsh_environment->count++;
    memcpy(new_env, dsh_environment, (sizeof(dsh_environment) + (sizeof(dsh_env_var_t) * dsh_environment->count)));
    free(dsh_environment);
    dsh_environment = new_env;

    return dsh_environment;
}

bool dsh_setenv(command_line_t* command_line) {
    assert(command_line);
    // retrieve name from command line
    char* name = my_strtok(NULL, ' ');
    assert(name);
    char* value = my_strtok(NULL, ' ');
    assert(value);
    bool updated = false;

    // check if variable already exists
    // dsh_env_var_t* empty = NULL;
    for (size_t i = 0; i < dsh_environment->count; i++) {
        if (STRING_EQUAL(name, dsh_environment->variables[i].name)) {
            free(dsh_environment->variables[i].value);
            dsh_environment->variables[i].value = strdup(value);
            updated = true;
            break;
        }
    }
    printf("before---> count: %zu\n", dsh_environment->count);
    // if adding a new key:value pair to environment variable list
    if (!updated) {
        bool empty_slot = false;
        size_t null_index = 0;
        
        updated = grow(dsh_environment, dsh_environment->count);
        dsh_env();
        puts("\n\n");
        dsh_environment->count++;
        for (null_index = 0; null_index < dsh_environment->count + 1; null_index++) {
            if (dsh_environment->variables[null_index].name == NULL && 
                dsh_environment->variables[null_index].value == NULL) {
                    empty_slot = true;
                    printf("empty slot foudn at %zu\n", null_index);
                    break;
                }
        }
        printf("null_index: %zu\n", null_index);
        // dsh_environment->variables[null_index].name = strdup(name);
        // dsh_environment->variables[null_index].value = strdup(value);
    }

    return updated;
}