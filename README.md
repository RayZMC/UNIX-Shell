# UNIX Shell Project

## Overview

This project implements a custom UNIX shell that supports basic shell functionalities such as command history, navigation (via `cd`), printing the working directory (`pwd`), and running external commands. The shell also handles basic error management and supports background execution of commands.

The shell implements both internal commands (like `cd`, `pwd`, `exit`, `history`, and `help`) and external programs by forking and executing them in child processes. Additionally, the shell maintains a command history to allow users to recall and reuse previously executed commands using history manipulation commands like `!!` (last command) and `!n` (where `n` is the command number).

## Features

### 1. **Command History**
- Tracks the last 10 commands entered by the user.
- Users can recall previous commands using:
  - `!!`: Executes the last command.
  - `!n`: Executes the command corresponding to number `n` from the history.

### 2. **Internal Commands**
The shell supports the following internal commands:
- `cd <dir>`: Changes the current directory.
- `pwd`: Displays the current working directory.
- `exit`: Exits the shell.
- `history`: Displays the command history.
- `help`: Displays usage information for the internal commands.

### 3. **External Command Execution**
- Executes external programs using `fork()` and `execvp()`.

### 4. **Background Execution**
- Commands can be run in the background by appending `&` to the command.

### 5. **Signal Handling**
- Custom `SIGINT` handler to cleanly handle interrupts and display helpful messages.

## How It Works

1. **Command Input and Parsing**
   - The user enters a command in the shell prompt.
   - The command is tokenized into individual words (separated by spaces).
   - The shell checks if the command is an internal command (e.g., `cd`, `exit`, `pwd`).

2. **Command History**
   - Every executed command is added to the history, with a maximum size of 10 commands.
   - History commands (`!!` and `!n`) are handled separately for recalling previous commands.

3. **Internal Commands**
   - `cd`: Changes the current directory.
   - `pwd`: Prints the current directory.
   - `exit`: Exits the shell.
   - `history`: Displays the last 10 commands executed.
   - `help`: Displays usage information for the internal commands.

4. **External Command Execution**
   - If the command is not internal, the shell attempts to execute it as an external program using `fork()` to create a child process and `execvp()` to replace the child process with the desired executable.

5. **Background Execution**
   - Commands followed by `&` are executed in the background (shell does not wait for their completion before accepting new input).

6. **Signal Handling**
   - The shell catches `SIGINT` (interrupt signal) and prints a message when the user presses `Ctrl+C`, then re-displays the prompt.

## File Structure

### `main.c`
Contains the main shell loop, overall control flow, signal handling, and execution of internal and external commands, along with the history management.

### `history.c`
Handles the functionality for the command history:
- Adds commands to the history.
- Displays the history of commands.
- Handles history commands like `!!` and `!n`.

### `internalcmds.c`
Contains the implementation of internal commands:
- `cd`: Changes the current directory.
- `pwd`: Prints the current directory.
- `exit`: Exits the shell.
- `help`: Displays help for internal commands.


