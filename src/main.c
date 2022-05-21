#include "dsh.h"

// #define PROMPT "dsh>"
#define SPACE ' '
// #define ARG_MAX 128


bool print_prompt() {
    /*int myprintf =*/ my_printf("dsh>");
    return true;
}

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

int	my_strcmp(const char *s1, const char *s2)
{
	int index;

	index = 0;
	while (s1[index] != '\0' && s2[index] != '\0')
	{
		if (s1[index] != s2[index])
			return (s1[index] - s2[index]);
		++index;
	}
	return ((unsigned char)s1[index] - (unsigned char)s2[index]);
}

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
		// loop across until captured_path hits delimiter
		while (*captured_path != '\0' && *captured_path != seperator) {
			captured_path++;
		}

		assert(*captured_path == seperator || *captured_path == '\0');
		// replace seperator w/ null char and terminate token
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

// compare if command passed (command) is same as #define command (dsh_command)
bool is_internal_command(const char* dsh_command, const char* command) {
    assert(command);
    assert(dsh_command);
    return strcmp(dsh_command, command) == 0;
}

bool run_internal(command_line_t* command_line) {
    assert(command_line);
    bool run_result = false;
    
    if (is_internal_command(dsh_echo_command, command_line->command)) {
        run_result = dsh_echo(command_line);
    } else if (is_internal_command(dsh_print_working_directory_command, command_line->command)) {
        run_result = dsh_pwd(command_line);
    } else if (is_internal_command(dsh_change_working_directory_command, command_line->command)) {
        run_result = dsh_cd(command_line);
    } else if (is_internal_command(dsh_which_command, command_line->command)) {
        run_result = dsh_which(command_line);
    } else if (is_internal_command(dsh_display_environment_command, command_line->command)) {
        run_result = dsh_env();
    } else if (is_internal_command(dsh_set_environment, command_line->command)) {
        run_result = dsh_setenv(command_line); 
    // } else if (is_internal_command(dsh_unsetenv_environment, command_line->command)) {
    //     run_result = dsh_unsetenv(command_line); 
    } else {
        assert(false);
    }
    return run_result;
}

// run an external command()
/*bool run_external(command_line_t* command_line) {
    ASSERT_COMMAND_LINE(command_line);
    char* argv[ARG_MAX] = {NULL};
    int i = 0;
    // init argv
    argv[i++] = command_line->command;
    while((argv[i++] = strtok_r(NULL, " \t\r\n\f", &command_line->save_ptr)) != NULL) {
    }
    // get an envp[]
    bool run_result = false;
    char** dsh_envp = dsh_allocate_envp();
    assert(dsh_envp);
    
    pid_t pid = fork();
    if(pid > 0) {
        // parent - wait and clean-up
        int status = 0xDEADBEEF;
		do { // wait for child to terminate before moving on
			pid_t wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
        bool success = dsh_free_envp(dsh_envp);
        assert(success);
        run_result = true;
    } else if(pid == 0) {
        // child - run external command
        const char* path = dsh_enumerate_env_var("PATH", ":");
        while(path) {
            char path_buffer[PATH_MAX] = {'\0'};
            sprintf(path_buffer, "%s/%s", path, command_line->command);
            execve(path_buffer, argv, dsh_envp);
            path = dsh_enumerate_env_var(NULL, ":");
        }
    } else {
        perror("fork()");
    }
    return run_result;
}*/

void display_prompt(const char* prompt) {
    printf("%s", prompt ? prompt : default_prompt);
}

bool set_args(command_line_t* command, char** string_split) {
    int string_array_index = 1;
    int arg_index = 0;

    while (arg_index < command->wordcount) {
        command->args[arg_index] = string_split[string_array_index];
        arg_index++, string_array_index++;
    }

    return true;
}

int main(int argc, char* argv[], char* envp[]) {
    assert(argc);
    assert(argv);
    command_line_t command_line = {0};
    size_t command_line_n = 0;
    char* command_line_buffer = NULL;
    // create copy of envp for use OUTSIDE of main fx
    bool success = dsh_allocate_environment((char**)envp);
    assert(success);

    while (true) {
        // display_prompt(NULL);
        my_printf("dsh> ");
        // retrieve arguments passed by user
        ssize_t nread = getline(&command_line_buffer, &command_line_n, stdin);
        assert(nread != 0);
        assert(command_line_buffer);
        command_line_buffer[strlen(command_line_buffer) - 1] = '\0';
        command_line.command = my_strtok(command_line_buffer, ' ');
        assert(command_line.command);
      
        // if nothing passed/return key hit, return to top of prompt
        if (!command_line.command || is_equal(command_line.command, "\n")) {
            continue;
        }
        assert(command_line.command);
        // string_tolower(command_line.command);
        if (my_strcmp(dsh_exit_command, command_line.command) == 0) {
            break;
        }
        if (!run_internal(&command_line)) { //} && !run_external(&command_line)) {
            my_printf("%s: invalid command\n", command_line.command);
        }
        command_line.command = NULL;
    }
    assert(command_line_buffer);
    free(command_line_buffer);
    // success = dsh_free_environment();
    assert(success);
        
    return 0;
}
