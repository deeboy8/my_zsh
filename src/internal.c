#include "dsh.h"

// echo remainder of stdin stream to stdout
bool dsh_echo(command_line_t* command_line) {
    assert(command_line);
    char* value = NULL;
    char* argument = my_strtok(NULL, '\n');
    
    if (argument[0] == '$') {
        size_t len = my_strlen(argument);
        char buffer[PATH_MAX] = {'\0'};
        argument++;
        strncpy(buffer, argument, (len - 1));
        if (check_for_env(buffer)) {
            value = dsh_getenv(buffer);
            return my_printf("%s\n", value);
        } else {
            return my_printf("%s: variable not found\n", buffer);
        }
    } else {
        return my_printf("%s\n", argument);  
    }
}

// print the working directory to stdout
bool dsh_pwd(command_line_t *command_line) {
  assert(command_line);
  char cwd[PATH_MAX] = {'\0'};
  char *pwd = getcwd(cwd, PATH_MAX);
  assert(pwd);
  assert(pwd == cwd);
  my_puts(pwd);
  return true;
}



// TBN: djg - I *think* 'cd -' works differently than you've interpreted it:
//  [cd(1p) - Linux manual page](https://tinyurl.com/2aoaap4b)
// In that I don't believe it needs a stack/linked-list but rather a single
// OLDPWD. Look at this interaction w/the real zsh:
//
//  gitpod /workspace/dsh (main) $ pwd
//  /workspace/dsh
//  gitpod /workspace/dsh (main) $ ls
//  dsh  makefile  src
//  gitpod /workspace/dsh (main) $ cd src
//  gitpod /workspace/dsh/src (main) $ pwd
//  /workspace/dsh/src
//  gitpod /workspace/dsh/src (main) $ cd -
//  /workspace/dsh
//  gitpod /workspace/dsh (main) $ cd -
//  /workspace/dsh/src
//  gitpod /workspace/dsh/src (main) $ cd -
//  /workspace/dsh
//  gitpod /workspace/dsh (main) $ cd -
//  /workspace/dsh/src
//  gitpod /workspace/dsh/src (main) $
//
// Notice how it doesn't maintain all previous dirs, but rather just the last.
// That is, 'cd -' toggles between the current and previous (OLDPWD) dir...I
// think😎.


// command to change directories
bool dsh_cd(command_line_t *command_line) { 
    assert(command_line);
    // collect argument from CLI
    char *path = my_strtok(NULL, ' ');

    // grab oldpwd value
    char* temp = get_oldpwd_value();
    // grab current pwd
    char cwd[PATH_MAX] = {'\0'};
    char *pwd = getcwd(cwd, PATH_MAX);
    assert(pwd);

    // set oldpwd to pwd
    bool upgrade = update_variable_value("OLDPWD", pwd);

    if (!path) {
        path = dsh_getenv("HOME");
    } else if (my_strcmp(path, "-") == 0) {
        char buffer[PATH_MAX] = {'\0'};
        my_strcpy(buffer, temp);
        my_printf("%s\n", buffer);
        path = buffer;
        upgrade = update_variable_value("PWD", path);
    }
    free(temp);
    
    return (chdir(path) == 0);
}

// determine which folder program located
bool dsh_which(command_line_t *command_line) {
  assert(command_line);
  char *external_command = NULL;
  bool external_command_found = false;
  char *new_string = NULL;
  if ((external_command = my_strtok(NULL, ' ')) != NULL) {
    const char *path = dsh_enumerate_env_var("PATH", ":");
    while (path) {
      char path_buffer[PATH_MAX] = {'\0'};
      strcpy(path_buffer, path);
      new_string = my_strjoin(path_buffer, external_command, true);
      struct stat sb = {0};
      if ((stat(new_string, &sb) == 0) && S_ISREG(sb.st_mode)) {
        external_command_found = true;
        break;
      }
      path = dsh_enumerate_env_var(NULL, ":");
    }
    my_printf("%s\n", new_string);
  }
  return external_command_found;
}
