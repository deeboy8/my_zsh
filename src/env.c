#include "dsh.h"

//hold the name and value of each envp variable
typedef struct {
  char *name;
  char *value;
} dsh_env_var_t;

//hold count of envp variables and an array of ptr address to each string
typedef struct {
  size_t count;
  dsh_env_var_t variables[];
} dsh_environment_t;

//global variable for accessing envp outside of main fx
static dsh_environment_t *g_dsh_environment = NULL;

//custom fx similar to strchr()
//https://www.man7.org/linux/man-pages/man3/strchr.3.html
//returns ptr to first occurance of char found in string
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
//loop over env array to determine count of variables 
static int get_variable_count(char *env[]) {
  assert(env);
  int count = 0;
  while (*env++)
    count++;

  return count;
}

//grab each directory based on delimiter found in PATH variable
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

//copy each variable the array of ptrs in environ ptr points to
//https://man7.org/linux/man-pages/man7/environ.7.html
//will allow for access of variables outside of main via global variable g_dsh_enviorment
bool dsh_allocate_environment(char *env[]) {
  assert(g_dsh_environment == NULL);
  //determine number of variables found in envp/environ variable
  int env_var_count = get_variable_count(env);
  assert(env_var_count > 0);
  //allocate mem space on the heap to hold the dsh_environment_t struct data (holds var count and an array of structs) and
  //the dsh_environement_t struct (holds the name of each name and its corresponding value)
  g_dsh_environment = malloc(sizeof(dsh_environment_t) +
                             (sizeof(dsh_env_var_t) * (env_var_count + 1)));
  assert(g_dsh_environment);

  //assign number of variables in envp 
  g_dsh_environment->count = env_var_count;
  //loop over envp adding name and value of each variable to element in struct
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    //seperate name and value based on delimiter
    char* equal_sep = my_strchr(env[i], '=');
    assert(equal_sep);
    //terminate name string with null terminating char by replacing '=' 
    *equal_sep++ = '\0';
    //allocate memory for name element of struct and assign 
    g_dsh_environment->variables[i].name = my_strdup(env[i]);
    assert(g_dsh_environment->variables[i].name);
    //allocate memory for value element of struct and assign 
    g_dsh_environment->variables[i].value = my_strdup(equal_sep);
    assert(g_dsh_environment->variables[i].value);
  }
  return true;
}


// bool check_for_env_var(char *name) {
//     bool is_present = false;
    
//     for (size_t i = 0; i < g_dsh_environment->count; i++) {
//         if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
//             is_present = true;
//             break;
//         }
//     }
//     return is_present;
// }

//detemine if env variable present 
bool check_for_env(char* name) {
    bool is_present = false;

    for (size_t i = 0; i < g_dsh_environment->count - 1; i++) {
        if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
            is_present = true;
            break;
        }
    }

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

//implement the printenv cmd (mac)
bool dsh_env() {
  assert(g_dsh_environment);
  //loop over each struct in dsh_env_var_t and concatenate name and value
  //print to stdout
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    my_printf("%s=%s\n", g_dsh_environment->variables[i].name,
              g_dsh_environment->variables[i].value);
  }
  return true;
}

//recreation of realloc adding additional memory to global g_dsh_environment
//will be called when user wishes to add an env variable 
bool grow() {
    //account for new size of memory necessary; adding an additional dsh_env_var_t 
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

//add an env variable like c runtime fx setenv
bool dsh_setenv(command_line_t *command_line) {
  assert(command_line);
  // retrieve name from command line
  char *name = my_strtok(NULL, ' ');
  assert(name);
  char *value = my_strtok(NULL, ' ');
  assert(value);
  bool updated = false;

  //check if variable already exists, set value to users perference
  for (size_t i = 0; i < g_dsh_environment->count; i++) {
    if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
    //free value associated with env var
    free(g_dsh_environment->variables[i].value);
    //allocate mem and assign new value as specified by user
    g_dsh_environment->variables[i].value = strdup(value);
    updated = true;
    break;
    }
  }

  //if there env var is not present in environment
  if (!updated) {
    // size_t null_index = 0; //TODO delete if no issues with running
    size_t index = 0;
    
    //allocate additional mem for newly added env var
    updated = grow();
    for (index = 0; index < g_dsh_environment->count; index++) {
      if (g_dsh_environment->variables[index].name == NULL) {
        break;
      }
    }
    //allocate mem and set name and value of new env var 
    g_dsh_environment->variables[index].name = strdup(name);
    g_dsh_environment->variables[index].value = strdup(value);
  }

  return updated;
}

//remove an env variable from env
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

//walk memory space of copied env and free each variable key:value pair
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

//free mem of entire env initally copied from main
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

//combine struct elements (name and value) of dsh_env_var_t into a string to mimic envp
char **dsh_allocate_envp() {
    //assert global variable is valid
    assert(g_dsh_environment);
    //determine size of mem to allocate on heap
    //will create an array of ptrs holding address of strings for each envp variable of the system environment
    //https://man7.org/linux/man-pages/man7/environ.7.html
    size_t size = sizeof(char *) * g_dsh_environment->count + 1;
    char **envp = malloc(size);
    assert(envp);
    //initialize newly allocated mem with zeros
    memset(envp, 0, size);
    //loop over each struct identifying the key and the value
    for (size_t i = 0; i < g_dsh_environment->count - 1; i++) {
        //using custom fx, join name and value into single string
        envp[i] = my_strjoin(g_dsh_environment->variables[i].name,
                                g_dsh_environment->variables[i].value, false);
        assert(envp[i]);
    }

    return envp;
}

//change the value associated with an env variable
bool update_variable_value(char* name, char* value) {
    assert(value);
    bool updated = false;

    //find OLDPWD variable and replace value
    for (size_t i = 0; i < g_dsh_environment->count; i++) {
    if (STRING_EQUAL(name, g_dsh_environment->variables[i].name)) {
        free(g_dsh_environment->variables[i].value);
        //use strdup to allocate mem for new value and assign to value
        g_dsh_environment->variables[i].value = strdup(value);
        updated = true;
        break;
        }
    }

    return updated;
}

//replace OLDPWD env var and update 
char* get_oldpwd_value() {
    char* value = NULL;
    //find variable and update with new old pwd value
    for (size_t i = 0; i < g_dsh_environment->count; i++) {
        if (STRING_EQUAL("OLDPWD", g_dsh_environment->variables[i].name)) {
            value = strdup(g_dsh_environment->variables[i].value);
            break;
        }   
    }

    return value;
}