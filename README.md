# my_zsh

## About

I wanted to build my own version of the command line interpreter found in the Unix system Terminal program.

## Description

`my_zsh` is an API mimicking the standard command line interpeter using the fork function to create parent and child processes. It implements built-in commands: echo, cd, setenv, unsetenv, env, exit, pwd and which, which are defined as internal functions. These functions are directly defined in the my_zsh program and therefore run directly from the my_zsh program when called. External commands are commands that are not defined in the my_zsh program and require the program to find the command exeuctable via the PATH variable command to execute.

To parse the commands and arguments, the C function strtok() was reconstructed. The program initially reads the command and checks to see if it is an internal function; if so, it executes the command directly within the program. If not, it will then use the execve() to check for the commands binary file along the PATH environment variable. It calls the fork() to create two processes, allowing for the continual running of the program so upon execution and ending of the child process, the parent process is returned with no exiting of the program.

## How to compile

Compile by running `make` in the root directory

## Usage

`./zsh [string command] [command arguments]`
