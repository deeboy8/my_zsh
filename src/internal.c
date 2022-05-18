#include "dsh.h"

#
// echo remainder of stdin stream to stdout
bool dsh_echo(command_line_t* command_line) {
    assert(command_line);
    char* argument = my_strtok(NULL, '\n');
    return puts(argument);  
}

// print the working directory to stdout
bool dsh_pwd(command_line_t* command_line) {
    assert(command_line);
    char cwd[PATH_MAX] = {'\0'};
    char* pwd = getcwd(cwd, PATH_MAX);
    assert(pwd);
    assert(pwd == cwd);
    my_puts(pwd);
    return true;
}

bool dsh_cd(command_line_t* command_line) {
    assert(command_line);
    const char* path = my_strtok(NULL, ' ');
    if (!path) {
        path = dsh_getenv("HOME");
    }
    assert(path);
    return (chdir(path) == 0);
}

/*bool dsh_which(command_line_t* command_line) {
    assert(command_line);
    char* external_command = NULL;
    bool external_command_found = false;
    if ((external_command = my_strtok(NULL, ' ')) != NULL) {
        const char* path = dsh_enumerate_env_var("PATH", ":");
        while (path) {
            char path_buffer[PATH_MAX] = {'\0'};
            char* new_string = ft_strjoin(path_buffer, external_command);

            struct stat sb = {0};
            if (stat(new_string, &sb) == 0) && S_ISREG(sb.st_mode)) {
                my_puts(new_string);
                external_command_found = true;
            }
            path = dsh_enumerate_env_var()
        }
    }
}*/ 