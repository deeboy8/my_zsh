#include "dsh.h"

// maintain newly created envp/ copy of envp
typedef struct {
  char *name;
  char *value;
} dsh_env_var_t;

// maintain
typedef struct {
  size_t count;
  dsh_env_var_t variables[];
} dsh_environment_t;

static dsh_environment_t *g_dsh_environment = NULL;

char *my_strchr(const char *s, int c) {
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

static int get_variable_count(char *env[]) {
  assert(env);
  int count = 0;
  while (*env++)
    count++;

  return count;
}

//will check each individual executable name within PATH and assign it the appropriate value
const char *dsh_enumerate_env_var(const char *name, const char *delimiter) {
  static char *env_var_name = NULL;
  char *value = NULL;
  if (env_var_name == NULL || name != NULL) {
    free(env_var_name);
    //get value of PATH env var name
    env_var_name = dsh_getenv(name);
    assert(env_var_name);
    //allocate memory for value of name returned from dsh_getenv fx
    env_var_name = strdup(env_var_name);
    assert(env_var_name);
    //obtain value of env_var_name by parsing using strtok and return value
    //https://man7.org/linux/man-pages/man3/strtok.3.html
    value = strtok(env_var_name, delimiter);
  } else {
        value = strtok(NULL, delimiter);
    }

  return value;
}

//copy each directory path from PATH envp variable
//will be accessible via global variable g_dsh_enviorment
bool dsh_allocate_environment(char *env[]) {
  assert(g_dsh_environment == NULL);
  //determine number of variables found in PATH envp variable
  int env_var_count = get_variable_count(env);
  assert(env_var_count > 0);
  g_dsh_environment = malloc(sizeof(dsh_environment_t) +
                             (sizeof(dsh_env_var_t) * (env_var_count + 1)));
  assert(g_dsh_environment);

  // initiailize
  g_dsh_environment->count = env_var_count;
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    char* equal_sep = my_strchr(env[i], '=');
    assert(equal_sep);
    // terminate name string, and point ptr to value string
    *equal_sep++ = '\0';
    // copy name
    g_dsh_environment->variables[i].name = my_strdup(env[i]);
    assert(g_dsh_environment->variables[i].name);
    // copy value
    g_dsh_environment->variables[i].value = my_strdup(equal_sep);
    assert(g_dsh_environment->variables[i].value);
  }
  return true;
}

bool check_for_env_var(char *name) {
    bool is_present = false;
    
    for (size_t i = 0; i < g_dsh_environment->count; i++) {
        if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
            is_present = true;
            break;
        }
    }
    return is_present;
}

bool check_for_env(char* name) {
    bool is_present = false;

    for (size_t i = 0; i < g_dsh_environment->count - 1; i++) {
        if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
            is_present = true;
            break;
        }
    }
    // printf("%d\n", is_present);
    return is_present;
}

//will walk along envp variables to find the variable that matches name
char *dsh_getenv(const char *name) {
  assert(g_dsh_environment);
  assert(name);
  char *value = NULL;
  //loop over each name of envp variable comparing it to name passed as parameter
  //on success, will set the corresponding value of name to value 
  //will return value
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    if (my_strcmp(name, g_dsh_environment->variables[i].name) == 0) {
      value = g_dsh_environment->variables[i].value;
      break;
    }
  }
  return value;
}

// implement the 'env' command
bool dsh_env() {
  assert(g_dsh_environment);
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    my_printf("%s=%s\n", g_dsh_environment->variables[i].name,
              g_dsh_environment->variables[i].value);
  }
  return true;
}

bool grow() {
  size_t size = sizeof(dsh_environment_t) +
                (sizeof(dsh_env_var_t) * g_dsh_environment->count +
                 sizeof(dsh_env_var_t));
  dsh_environment_t *new_env = malloc(size);
  assert(new_env);
  memset(new_env, 0, size);
  memcpy(new_env, g_dsh_environment, size - sizeof(dsh_env_var_t));
  free(g_dsh_environment);
  g_dsh_environment = new_env;
  g_dsh_environment->count++;

  return true;
}

bool dsh_setenv(command_line_t *command_line) {
  assert(command_line);
  // retrieve name from command line
  char *name = my_strtok(NULL, ' ');
  assert(name);
  char *value = my_strtok(NULL, ' ');
  assert(value);
  bool updated = false;

  // check if variable already exists
  // dsh_env_var_t* empty = NULL;
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
      free(g_dsh_environment->variables[i].value);
      g_dsh_environment->variables[i].value = strdup(value);
      updated = true;
      break;
    }
  }

  // if adding a new key:value pair to environment variable list
  if (!updated) {
    // TGN: djg - unused - comment so build works
    //   bool empty_slot = false;
    size_t null_index = 0;

    updated = grow();
    for (null_index = 0; null_index < g_dsh_environment->count; null_index++) {
      if (g_dsh_environment->variables[null_index].name == NULL) {
        // TGN: djg - unused - comment so build works
        //   empty_slot = true;
        break;
      }
    }
    g_dsh_environment->variables[null_index].name = strdup(name);
    g_dsh_environment->variables[null_index].value = strdup(value);
  }

  return updated;
}

bool dsh_unsetenv(command_line_t *command_line) {
  assert(g_dsh_environment);
  assert(command_line);

  bool found = false;
  char *dsh_var_name = my_strtok(NULL, ' ');
  // search for var name --> free key:value pair upon discovery
  for (size_t index = 0; index < g_dsh_environment->count; index++) {
    if (my_strcmp(dsh_var_name, g_dsh_environment->variables[index].name) ==
        0) {
      free(g_dsh_environment->variables[index].name);
      g_dsh_environment->variables[index].name = NULL;
      free(g_dsh_environment->variables[index].value);
      g_dsh_environment->variables[index].value = NULL;
      found = true;
      break;
    }
  }

  if (!found) {
    my_printf("no %s found\n", dsh_var_name);
  }

  return found;
}

bool g_dsh_free_environment() {
  assert(g_dsh_environment);

  for (size_t index = 0; index < g_dsh_environment->count; index++) {
    if (g_dsh_environment->variables[index].name) {
      free(g_dsh_environment->variables[index].name);
    }
    if (g_dsh_environment->variables[index].value) {
      free(g_dsh_environment->variables[index].value);
    }
  }
  free(g_dsh_environment);
  g_dsh_environment = NULL;
  return true;
}

// free a 'char* envp[]' (for execve())
bool dsh_free_envp(char *dsh_envp[]) {
  assert(dsh_envp);
  char **p = dsh_envp;
  while (*p) {
    free(*p);
    p++;
  }
  free(dsh_envp);
  return true;
}

//
char **dsh_allocate_envp() {
    //global variable
    assert(g_dsh_environment);
    //determine size of mem to allocate on heap
    //will create an array of ptrs pointing to each envp variable of the system environment
    //https://man7.org/linux/man-pages/man7/environ.7.html
    size_t size = sizeof(char *) * g_dsh_environment->count + 1;
    char **envp = malloc(size);
    assert(envp);
    //initialize newly allocated mem with 0
    memset(envp, 0, size);

    for (size_t i = 0; i < g_dsh_environment->count - 1; i++) {
        //using custome fx, seperate the key and value of the env variable
        //place each into appropriate memory location(key = name, value = value)
        envp[i] = my_strjoin(g_dsh_environment->variables[i].name,
                                g_dsh_environment->variables[i].value, false);
        assert(envp[i]);
    }

    return envp;
}

bool update_variable_value(char* name, char* value) {
  assert(value);
  bool updated = false;
  
  // find OLDPWD variable and replace value
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
      free(g_dsh_environment->variables[i].value);
      g_dsh_environment->variables[i].value = strdup(value);
      updated = true;
      break;
    }
  }

  return updated;
}

char* get_oldpwd_value() {
    char* value = NULL;
    // find variable and replace value
    for (size_t i = 0; i < g_dsh_environment->count; i++) {
        if (STRING_EQUAL("OLDPWD", g_dsh_environment->variables[i].name)) {
            value = strdup(g_dsh_environment->variables[i].value);
            break;
        }   
    }

    return value;
}