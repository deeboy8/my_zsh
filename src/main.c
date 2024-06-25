#include "dsh.h"


#define SPACE ' '

// bool print_prompt() {
//     my_printf("dsh>");
//     return true;
// }

// //custom function taking capitalized text passed by user and changing to lowercase
// char* string_tolower(char* string) {
//     int len = strlen(string);
//     char* string_tolower = malloc(sizeof(char) * len + 1);
//     if (!string_tolower)
//             return NULL;
//     int index = 0;
//     while (string[index]) {
//         if (string[index] > 64 && string[index] < 91)
//             string_tolower[index] = string[index] + 32;
//         index++;
//     }
//     string_tolower[index] = '\0';
//     return string_tolower;
// }

// bool is_equal(char* string1, char* string2) {
//     assert(string1);
//     assert(string2);
//     return strcmp(string1, string2) == 0;
// }

// //recreation of c runtime libary fx strcmp()
// int	my_strcmp(const char *s1, const char *s2)
// {
// 	int index;
// 	index = 0;

//     //loop over both strings checking each char
//     //returns a numeric value reflective of difference or 0 if exact same
// 	while (s1[index] != '\0' && s2[index] != '\0')
// 	{
// 		if (s1[index] != s2[index])
// 			return (s1[index] - s2[index]);
// 		++index;
// 	}
// 	return ((unsigned char)s1[index] - (unsigned char)s2[index]);
// }

// //parses a line passed by user and tokenize 
// char* my_strtok(char* path, char seperator) {
// 	static char* current_path = NULL;
// 	static char* captured_path = NULL;

// 	if (path != NULL) {
// 		current_path = path; // trailing pointer
// 		captured_path = path; // leading pointer
// 	}

// 	if (captured_path == NULL) {
// 		current_path = NULL;
// 	} else {
// 		current_path = captured_path;
// 		// loop across until captured_path hits seperator (delimiter)
// 		while (*captured_path != '\0' && *captured_path != seperator) {
// 			captured_path++;
// 		}
// 		assert(*captured_path == seperator || *captured_path == '\0');
// 		// replace seperator with null terminating char to tokenize portion between captured and current ptrs
// 		if (*captured_path == seperator) {
// 			*captured_path = '\0';
// 			captured_path++;
// 		} else if (*captured_path == '\0') {
// 			captured_path = NULL;
// 		} else {
// 			assert(false);
// 		}
// 	}
// 	return current_path;
// }

//compare cmd passed against macro names to run appropriate code
bool is_internal_command(const char* dsh_command, const char* command) {
    assert(command);
    assert(dsh_command);
    return strcmp(dsh_command, command) == 0;
}

//check if cmd passed is a cmd that is "internal" to the shell program
bool run_internal(command_line_t* command_line) {
    assert(command_line);
    bool run_result = false;

    //check if names passed match an internal cmd
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

//run an external command() passed on the command line
//command checked to be internal first
//will invoke fork() and exec() to walk PATH directory to find executable which matches users request 
bool run_external(command_line_t* command_line) {
    assert(command_line);
    //allocate array of buffer space with size equal to c runtime path length as defined by operating sys/file sys
    char* argv[PATH_MAX] = {NULL};
    int i = 0;
    
    //fill dynamic argument array to be passed to child process for holding PATH directories
    argv[i++] = command_line->command;
    //tokenization of string initiated prior to current call
    //therefore passing NULL in place of string as first parameter
    while((argv[i++] = my_strtok(NULL, ' ')) != NULL) {
    }

    //allocate memory space for and copy directories found in envp variable
    //will be used to find external cmd passed by user
    bool run_result = false;
    char** dsh_envp = dsh_allocate_envp();
    assert(dsh_envp);
    
    //using fork(), create duplicate copy of currently running shell process
    //will run external command in child process while parent process waits
    pid_t pid = fork(); 
    if (pid > 0) {
        int status;
        //set parent process id for easy identification (debugging purposes)
        pid_t wpid = 0xDEADBEEF;
        //requesting status of child process on first portion of do-while loop
        //if successful, parent process will continue to check status for child process, waiting for it's completion and termination
		do {
            //passing in pid from fork of child, ptr to status of what child returns, untraced flag used by wait
            //waiting until pid, child process, dies
            //inifiinte loop waiting for child shell to die/go away
            //must avoid spurious wake ups so must put in a inifinite while loop
			wpid = waitpid(pid, &status, WUNTRACED); //blocking call in this respect
            //ensure parent process has been assigned a pid number
            assert((unsigned int)wpid != 0xDEADBEEF);
        //ensured exited appropriately
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
        //if status returned due to uncaught signal, will print error msg to stdout
        //TODO best to identify what the actual error is and returning it, strerr() would be best, part of family of fx like perror
        if (WIFSIGNALED(status)) { 
            // char err[] = "&&dsh: segmentation fault  ./main_with_segfault\n";
            // write(FILENO_STDERR????, err, sizeof(err));
            char* err = "&&dsh: segmentation fault  ./main_with_segfault\n"; //TODO
            write(2, err, 46);
        }
        //free each pointer in array that holds address of each variable copied from envp
        bool success = dsh_free_envp(dsh_envp);
        assert(success);
        //check to ensure successful exit of child process
        run_result = (wpid != -1) && (WEXITSTATUS(status) != EXIT_FAILURE);
    //child process
    } else if (pid == 0) {
        //execute user input as passed on cl
        //success contingent upon providing proper pathway to cmd
        //exec will overwrite memory from parent and replace with code from executable it searched for 
        execve(command_line->command, argv, dsh_envp); 
        //if unsuccessful, will walk PATH appending cmd passed by user to end of directory
        //dsh_enumerate_env_var will take PATH as the initial name (first param)
        //after first call will pass NULL
        const char* path = dsh_enumerate_env_var("PATH", ":");
        while (path) {
            char path_buffer[PATH_MAX] = {'\0'};
            my_strcpy(path_buffer, path);
            //concatenate result of PATH directory with command passed by user
            char* new_string = my_strjoin(path, command_line->command, true);
            execve(new_string, argv, dsh_envp);
            free(new_string);
            path = dsh_enumerate_env_var(NULL, ":");
        }
        //upon completion, free allocated env variables
        bool success = dsh_free_envp(dsh_envp);
        assert(success);
        //will wake up parent and terminate child process
        exit(EXIT_FAILURE);
    } else {
        //return value of fork() is -1
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
    assert(argc);
    assert(argv); 
    //TODO asserting for a purpsoe or an unused check, pragma which turns it off, part of the build in makefile
        //communicate instrution to compiler (NOT CODE) 
            //#pragma (compiler specific) 
            
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
        // if (!command_line.command || is_equal(command_line.command, "\n")) {
        //     continue;
        // }
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
