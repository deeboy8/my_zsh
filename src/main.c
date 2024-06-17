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

//parse the line passed by user to capture each command and associated args 
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
		// replace seperator with null char and terminate token
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

//check the appropriate internal command to run passed by user input
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
    } else if (is_internal_command(dsh_unsetenv_environment, command_line->command)) {
        run_result = dsh_unsetenv(command_line); 
    } else {
        return run_result;
    }
    return run_result;
}

/// run an external command()
bool run_external(command_line_t* command_line) {
    assert(command_line);
    char* argv[PATH_MAX] = {NULL};
    int i = 0;
    
    //filling dynamic argument array to be passed to child process
    argv[i++] = command_line->command;
    while((argv[i++] = my_strtok(NULL, ' ')) != NULL) {
    }

    //allocate memory space for and copy directories found in PATH envp variable
    //will be used to find external cmd passed by user
    bool run_result = false;
    char** dsh_envp = dsh_allocate_envp();
    assert(dsh_envp);
    
    //using fork(), create duplicate copy of currently running shell process
    //will run external command in child process while parent process waits
    pid_t pid = fork(); 
    if (pid > 0) {
        int status;
        //initialize parent process mem space with 0xDEADBEEF 
        pid_t wpid = 0xDEADBEEF;
        //obtaining status of child process on first portion of do-while loop
        //if successful, parent process will continue to check status of child process, waiting for it's completion
		do { 
            //checking status of child process
			wpid = waitpid(pid, &status, WUNTRACED); 
            //ensure parent process has been assigned a pid number
            assert((unsigned int)wpid != 0xDEADBEEF);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
        //if segfault observed, will print to stdout
        if (WIFSIGNALED(status)) { 
            // char err[] = "&&dsh: segmentation fault  ./main_with_segfault\n";
            // write(FILENO_STDERR????, err, sizeof(err));
            char* err = "&&dsh: segmentation fault  ./main_with_segfault\n"; //TODO
            write(2, err, 46);
        }
        //upon success, free each pointer to variables in environment
        bool success = dsh_free_envp(dsh_envp);
        assert(success);
        run_result = wpid != -1 && WEXITSTATUS(status) != EXIT_FAILURE;
    //child process
    } else if (pid == 0) {
        //execute user input as passed on cl
        execve(command_line->command, argv, dsh_envp); 
        //if user input doesn't work, will walk PATH trying each folder 
        const char* path = dsh_enumerate_env_var("PATH", ":");
        while (path) {
            char path_buffer[PATH_MAX] = {'\0'};
            my_strcpy(path_buffer, path);
            char* new_string = my_strjoin(path, command_line->command, true);
            execve(new_string, argv, dsh_envp);
            free(new_string);
            path = dsh_enumerate_env_var(NULL, ":");
        }
        bool success = dsh_free_envp(dsh_envp);
        assert(success);
        exit(EXIT_FAILURE);
    } else {
        perror("fork()");
    }

    return run_result;
}

void display_prompt(const char* prompt) {
    printf("%s", prompt ? prompt : default_prompt);
}

//program implements typical linux style shell
//reads commands from user in an infinite loop until command exit is passed
int main(int argc, char* argv[], char* envp[]) {
    // assert(argc);
    // assert(argv); //TODO asserting for a purpsoe or an unused check, pragma which turns it off, part of the build in makefile
    
    command_line_t command_line = {0};
    size_t cmd_line_buffer_length = 0; 
    char* command_line_buffer = NULL;
    
    //allocate memory for envp passed with initiation of main
    //will be passed to other fxs and thus accessible outside of main
    bool success = dsh_allocate_environment((char**)envp);
    assert(success);

    //loop forever prompting user for a cmd
    //will parse cmds to determine if cmd is an external or internal cmd
    while (true) {
        // display prompt to accept user commands 
        my_printf("[dsh]> ");
        // retrieve arguments passed by user
        //getline fx takes a ptr to a memory space retrieves line from stdin to store in memory space
        //getline will automatically calculate length of line 
        //https://man7.org/linux/man-pages/man3/getline.3.html
        ssize_t nread = getline(&command_line_buffer, &cmd_line_buffer_length, stdin);
    
        if (nread == -1) {
            continue; //TODO confirm this works
        }

        assert(nread != 0);
        assert(command_line_buffer);
        //overwrite trailing newline placed by getline fx
        //getline fx includes the newline char from users hitting return and null terminates the string
        //will replace newline char in second to last index with null terminating char
        command_line_buffer[strlen(command_line_buffer) - 1] = '\0';
        
        //begin parsing of line using custom function passing entire line and delimiter
        command_line.command = my_strtok(command_line_buffer, ' ');
        assert(command_line.command);
    
        // if nothing passed by user or return key hit, will return to top of loop and print prompt to stdout
        if (!command_line.command || is_equal(command_line.command, "\n")) {
            continue;
        }
        assert(command_line.command);
        //cmd to exit program
        if (my_strcmp(dsh_exit_command, command_line.command) == 0) {
            break;
        }
        //check cmd passed is an internal or external command
        //otherwise print invalid error msg
        if (!run_internal(&command_line) && !run_external(&command_line)) {
            my_printf("%s: invalid command\n", command_line.command);
        }
        command_line.command = NULL;
    }
    assert(command_line_buffer);
    free(command_line_buffer);
    success = g_dsh_free_environment();
    assert(success);
        
    return 0;
}
