# my_zsh

## About

I wanted to build my own version of the command line interpreter found in the Unix system Terminal program.

## Description

`my_zsh` is an API mimicking the standard command line interpeter using the fork function to create parent and child processes. It implements built-in commands: echo, cd, setenv, unsetenv, env, exit, pwd and which. 

To parse the commands and arguments, the C function strtok() was reconstructed. The program initially reads the command and checks to see if it is a built-in function; if so, it executes the command directly within the program. If not, it will then use the execve() to check for the commands binary file along the PATH environment variable. It calls the fork() to create two processes, allowing for the continual running of the program so upon execution and ending of the child process, the parent process is returned with no exiting of the program.

## How to compile

Compile by running `make` in the root directory

## Usage

`./zsh [string command] [command arguments]`
