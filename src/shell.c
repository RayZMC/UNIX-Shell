#define _POSIX_C_SOURCE 200809L
#include "../include/msgs.h"
#include "history.h"
#include "internalcmds.h"
#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define HISTORY_SIZE 10

void display_prompt() {
  char cwd[BUFFER_SIZE] = {0};

  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    char *errormsg = FORMAT_MSG("shell", GETCWD_ERROR_MSG);
    write(STDERR_FILENO, errormsg, strlen(errormsg));
    return;
  }
  // write out current dir
  write(STDOUT_FILENO, cwd, strlen(cwd));
  write(STDOUT_FILENO, "$ ", 2);
}

void sigint_handler(int sig) {
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, FORMAT_MSG("cd", CD_HELP_MSG),
        strlen(FORMAT_MSG("cd", CD_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("exit", EXIT_HELP_MSG),
        strlen(FORMAT_MSG("exit", EXIT_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("pwd", PWD_HELP_MSG),
        strlen(FORMAT_MSG("pwd", PWD_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("help", HELP_HELP_MSG),
        strlen(FORMAT_MSG("help", HELP_HELP_MSG)));
  write(STDOUT_FILENO, FORMAT_MSG("history", HISTORY_HELP_MSG),
        strlen(FORMAT_MSG("history", HISTORY_HELP_MSG)));
  display_prompt();
}

int main() {
  bool running = true;
  char buffer[BUFFER_SIZE] = {0};
  char *tkinput[BUFFER_SIZE] = {NULL};
  char prev_dir[BUFFER_SIZE] = {0};

  if (getcwd(prev_dir, sizeof(prev_dir)) == NULL) {
    char *errormsg = FORMAT_MSG("shell", GETCWD_ERROR_MSG);
    write(STDERR_FILENO, errormsg, strlen(errormsg));
    exit(EXIT_FAILURE);
  }

  struct sigaction sa;
  sa.sa_handler = sigint_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Sigaction() failed");
    exit(EXIT_FAILURE);
  }
  while (running) {
    display_prompt();

    // read user input
    ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bytes_read < 0) {
      if (errno == EINTR) {
        continue;
      } else {
        char *errormsg = FORMAT_MSG("shell", READ_ERROR_MSG);
        write(STDERR_FILENO, errormsg, strlen(errormsg));
        continue;
      }
    }

    if (bytes_read > 0 && buffer[bytes_read - 1] == '\n') {
      buffer[bytes_read - 1] = '\0';
    }

    // Handle history commands
    process_command(buffer);

    // ----- Tokenization -----
    char *delim = " ";
    char *saveptr = NULL;
    char *token = strtok_r(buffer, delim, &saveptr);
    if (token == NULL) {
      continue;
    }

    int i = 0;

    bool bg = false;

    while (token != NULL) {
      char *ampersand = strchr(token, '&');
      if (ampersand != NULL) {
        bg = true;
        *ampersand = '\0';
        if (strlen(token) > 0) {
          tkinput[i++] = token;
        }
      } else {
        tkinput[i++] = token;
      }
      token = strtok_r(NULL, delim, &saveptr);
    }

    if (i == 0) {
      continue;
    }

    tkinput[i] = NULL;

    if (tkinput[0] == NULL) {
      continue;
    }

    if (i > 0 && strcmp(tkinput[i - 1], "&") == 0) {
      bg = true;
      tkinput[i - 1] = NULL;
    }

    // ----- Internal Commands -----
    if (tkinput[0] != NULL && strcmp(tkinput[0], "exit") == 0) {
      handle_exit(tkinput);
      continue;
    }

    if (tkinput[0] != NULL && strcmp(tkinput[0], "history") == 0) {
      display_history();
      continue;
    }

    if (tkinput[0] != NULL && strcmp(tkinput[0], "pwd") == 0) {
      handle_pwd(tkinput);
      continue;
    }

    if (tkinput[0] != NULL && strcmp(tkinput[0], "cd") == 0) {
      handle_cd(tkinput, prev_dir);
      continue;
    }

    if (tkinput[0] != NULL && strcmp(tkinput[0], "help") == 0) {
      handle_help(tkinput);
      continue;
    }

    pid_t pid = fork();
    int wstatus = 0;

    if (pid < 0) {
      char *errormsg = FORMAT_MSG("shell", FORK_ERROR_MSG);
      write(STDERR_FILENO, errormsg, strlen(errormsg));
    } else if (pid == 0) {
      if (tkinput[0] == NULL) {
        exit(EXIT_FAILURE);
      }
      if (execvp(tkinput[0], tkinput) == -1) {
        char *errormsg = FORMAT_MSG("shell", EXEC_ERROR_MSG);
        write(STDERR_FILENO, errormsg, strlen(errormsg));
        exit(EXIT_FAILURE);
      }
    } else {
      if (!bg) {
        if (waitpid(pid, &wstatus, 0) == -1) {
          if (errno == EINTR) {
            continue;
          } else {
            char *errormsg = FORMAT_MSG("shell", WAIT_ERROR_MSG);
            write(STDERR_FILENO, errormsg, strlen(errormsg));
          }
        }
      }
      while (waitpid(-1, &wstatus, WNOHANG) > 0) {
      }
    }
  }
  return 0;
}