#ifndef DSH_H
#define DSH_H

static const char default_prompt[] = "dsh> ";

#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static const char dsh_exit_command[] = "exit";
static const char dsh_echo_command[] = "echo";
static const char dsh_print_working_directory_command[] = "pwd";
static const char dsh_change_working_directory_command[] = "cd";
static const char dsh_which_command[] = "which";
static const char dsh_display_environment_command[] = "env";
static const char dsh_set_environment[] = "setenv";
static const char dsh_unsetenv_environment[] = "unsetenv";
#define STRING_EQUAL(s1, s2) my_strcmp(s1, s2) == 0

#define PROMPT "dsh$>"
#define MAX_DSH_ARGS 10

// hold command line information
typedef struct {
  int wordcount; 
  char *command;
  char *args[];
} command_line_t;

typedef struct nodelist {
  char *string;
  struct nodelist *next;
} nodelist;

// pwd
void pwd(void);

// cd
bool is_directory(const char *string);
void cd(const char *string);
bool dsh_cd(command_line_t *command_line); //, nodelist *head);

// echo
bool dsh_echo(command_line_t *command_line);
bool dsh_pwd(command_line_t *command_line);
char *dsh_getenv(const char *name);
bool dsh_which(command_line_t *command_line);
const char *dsh_enumerate_env_var(const char *name, const char *delimiter);
bool dsh_env();
bool dsh_setenv(command_line_t *command_line);
bool dsh_unsetenv(command_line_t *command_line);
bool g_dsh_free_environment();
char **dsh_allocate_envp();
bool dsh_free_envp(char *dsh_envp[]);

// my_printf
int my_printf(char *restrict format, ...);

// readline.c
char *my_readline(int fd);

// which
bool is_executable(char *folder);

// utility
int my_strcmp(const char *s1, const char *s2);
char *my_strcat(char *s1, const char *s2);
char *my_strtok(char *path, char seperator);
int get_count(char *env[]);
char *my_strjoin(char const *s1, char const *s2, bool flag);
void *my_realloc(void *mem_ptr, int count);
char *my_strcpy(char *dest, const char *src);
size_t my_strlen(const char *str);
void my_puts(char const *s);

// new
bool dsh_allocate_environment(char *env[]);
char *my_strdup(const char *s1);
int wordlen(char const *s, char c);
bool check_for_env_var(char *name);
bool update_variable_value(char* name, char* value);
char* get_oldpwd_value();
bool check_for_env(char* name);

#endif