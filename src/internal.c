#include "dsh.h"

#
// echo remainder of stdin stream to stdout
bool dsh_echo(command_line_t* command_line) {
    assert(command_line);
    char* argument = my_strtok(NULL, '\n');
    
    if (argument[0] == '$') {
        size_t len = my_strlen(argument);
        char buffer[PATH_MAX] = {'\0'};
        argument++;
        strncpy(buffer, argument, (len - 1));
        if (check_for_env_var(buffer)) {
            // printf("inside buffer is: %s\n", buffer);
            char* value = dsh_getenv(buffer);
            return my_printf("%s\n", value);
        } else {
            return my_printf("variable not found\n");
        }
    } else {
        return my_printf("%s\n", argument);  
    }
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

bool delete_linked_list(nodelist* head) {
    nodelist* temp = head;

    if (temp == NULL) {
        return false;
    }
    while (temp != NULL) {
        nodelist* next = temp->next;
        free(temp->string);
        free(temp);
        temp = next;
    }

    return true;
}

// create a single instance of a node in memory
nodelist* create_new_node() {
    // get current working directory
    char cwd[PATH_MAX] = {'\0'};
    char* pwd = getcwd(cwd, PATH_MAX);

    // get length of string
    size_t len = my_strlen(pwd);
    assert(len > 0);
    // allocate
    nodelist* new_node = malloc(sizeof(nodelist));
    assert(new_node);

    new_node->string = strdup(pwd); // need to add terminating char
    new_node->next = NULL;
   
    return new_node;
}

// link nodes together as they are made
bool link_nodes(nodelist* head, nodelist* new_node) {
    nodelist* temp;

    if (head == NULL) {
        new_node->next = head;
        head = new_node;
    } else {
        temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        new_node->next = temp->next;
        temp->next = new_node;
        // assert(new_node->next = NULL);
    }
    
    return true;
}

// print previous directory when '-' passed to cd command
bool print_previous_directory(nodelist* head) {
    // ptr to walk across linked list to the end
    nodelist* leading = head;
    assert(leading == head);
    // ptr to stop at second to last node before end
    // this will allow for printing the previous directory at trailing->string
    nodelist* trailing = head;
    assert(trailing == head);
   
    while (leading != NULL) {
        leading = leading->next;
    }

    while (trailing != leading) {
        trailing = trailing->next;
    }
    // size_t value = chdir(trailing->string);
   //  assert(value == 0);
    my_printf("%s\n", trailing->string);
    
    return true;    
}

// test function to see if properly making nodes
bool print_linked_list(nodelist* head) {
    nodelist* temp = head;
    assert(temp == head);

    if (head == NULL) {
        printf("%s\n", temp->string);
    } else {
        while (temp != NULL) {
            printf("%s\n", temp->string);
        } 
    }

    return true;
}

// command to change directories
bool dsh_cd(command_line_t* command_line, nodelist* head) {
    assert(command_line);
    // collect argument(s) from CLI 
    const char* path = my_strtok(NULL, ' ');
    /*printf("|%s|\n", path);
    if (!path) {
        path = dsh_getenv("HOME");
    } else if (my_strcmp(path, "-") == 0) {
        char cwd[PATH_MAX] = {'\0'};
        char* pwd = getcwd(cwd, PATH_MAX);
    }
    assert(path);*/

    //do/while
    // create a new "free-standing" node in mem
    nodelist* node = create_new_node();
    // link nodes
    bool link = link_nodes(head, node);
    assert(link);
    // print_linked_list(head);
   if (!path) {
        path = dsh_getenv("HOME");
    } else if (my_strcmp(path, "-") == 0) {
        print_previous_directory(head);
    }

    return (chdir(path) == 0);
}

/*bool dsh_cd(command_line_t* command_line) {
    assert(command_line);
    const char* path = my_strtok(NULL, ' ');
    if (!path) {
        path = dsh_getenv("HOME");
    }
    assert(path);
    return (chdir(path) == 0);
}*/

// determine which folder program located
bool dsh_which(command_line_t* command_line) {
    assert(command_line);
    char* external_command = NULL;
    bool external_command_found = false;
    char* new_string = NULL;
    if ((external_command = my_strtok(NULL, ' ')) != NULL) {
        const char* path = dsh_enumerate_env_var("PATH", ":");
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

