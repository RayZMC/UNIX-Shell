#define _POSIX_C_SOURCE 200809L
#include "../include/msgs.h"
#include "history.h"
#include <ctype.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_exit(char *tkinput[]) {
  if (tkinput[1] != NULL) {
    char *errormsg = FORMAT_MSG("exit", TMA_MSG);
    write(STDERR_FILENO, errormsg, strlen(errormsg));
    return;
  } else {
    exit(0);
  }
}

void handle_pwd(char *tkinput[]) {
  if (tkinput[1] != NULL) {
    char *errormsg = FORMAT_MSG("pwd", TMA_MSG);
    write(STDERR_FILENO, errormsg, strlen(errormsg));
    return;
  } else {
    char cur_dir[BUFFER_SIZE] = {0};
    if (getcwd(cur_dir, sizeof(cur_dir)) != NULL) {
      write(STDOUT_FILENO, cur_dir, strlen(cur_dir));
      write(STDOUT_FILENO, "\n", 1);
    } else {
      char *errormsg2 = FORMAT_MSG("pwd", GETCWD_ERROR_MSG);
      write(STDERR_FILENO, errormsg2, strlen(errormsg2));
      write(STDOUT_FILENO, "\n", 1);
    }
  }
}

void handle_cd(char *tkinput[], char *prev_dir) {
  char *tar_dir = NULL;
  struct passwd *user_info = NULL;
  char cur_dir[BUFFER_SIZE] = {0};
  char temp_dir[BUFFER_SIZE] = {0};
  char path[BUFFER_SIZE] = {0};

  if (tkinput[2] != NULL) {
    write(STDERR_FILENO, FORMAT_MSG("cd", TMA_MSG),
          strlen(FORMAT_MSG("cd", TMA_MSG)));
    return;
  }

  if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
    write(STDERR_FILENO, FORMAT_MSG("cd", GETCWD_ERROR_MSG),
          strlen(FORMAT_MSG("cd", GETCWD_ERROR_MSG)));
    return;
  }

  strncpy(temp_dir, cur_dir, BUFFER_SIZE - 1);
  temp_dir[sizeof(temp_dir) - 1] = '\0';

  if (tkinput[1] == NULL || strcmp(tkinput[1], "~") == 0) {
    user_info = getpwuid(getuid());
    if (user_info != NULL) {
      strncpy(path, user_info->pw_dir, sizeof(path) - 1);
      path[sizeof(path) - 1] = '\0';
      tar_dir = path;
    }
  } else if (strncmp(tkinput[1], "~", 1) == 0) {
    user_info = getpwuid(getuid());
    if (user_info != NULL) {
      snprintf(path, sizeof(path), "%s%s", user_info->pw_dir, tkinput[1] + 1);
      tar_dir = path;
    }
  } else if (strcmp(tkinput[1], "-") == 0) {
    if (strlen(prev_dir) == 0) {
      write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
            strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
      return;
    }
    tar_dir = prev_dir;
  } else {
    tar_dir = tkinput[1];
  }

  if (tar_dir != NULL) {
    if (chdir(tar_dir) == 0) {
      strncpy(prev_dir, temp_dir, BUFFER_SIZE - 1);
      prev_dir[BUFFER_SIZE - 1] = '\0';

      if (tkinput[1] != NULL && strcmp(tkinput[1], "-") == 0) {
        char new_dir[BUFFER_SIZE] = {0};
        if (getcwd(new_dir, sizeof(new_dir)) != NULL) {
          write(STDOUT_FILENO, new_dir, strlen(new_dir));
          write(STDOUT_FILENO, "\n", 1);
        }
      }
    } else {
      write(STDERR_FILENO, FORMAT_MSG("cd", CHDIR_ERROR_MSG),
            strlen(FORMAT_MSG("cd", CHDIR_ERROR_MSG)));
    }
  }
}

void handle_help(char *tkinput[]) {
  if (tkinput[1] == NULL) {
    write(STDOUT_FILENO, FORMAT_MSG("cd", CD_HELP_MSG),
          strlen(FORMAT_MSG("cd", CD_HELP_MSG)));
    write(STDOUT_FILENO, FORMAT_MSG("exit", EXIT_HELP_MSG),
          strlen(FORMAT_MSG("exit", EXIT_HELP_MSG)));
    write(STDOUT_FILENO, FORMAT_MSG("pwd", PWD_HELP_MSG),
          strlen(FORMAT_MSG("pwd", PWD_HELP_MSG)));
    write(STDOUT_FILENO, FORMAT_MSG("history", HISTORY_HELP_MSG),
          strlen(FORMAT_MSG("history", HISTORY_HELP_MSG)));
    write(STDOUT_FILENO, FORMAT_MSG("help", HELP_HELP_MSG),
          strlen(FORMAT_MSG("help", HELP_HELP_MSG)));
  } else if (tkinput[2] != NULL) {
    write(STDERR_FILENO, FORMAT_MSG("help", TMA_MSG),
          strlen(FORMAT_MSG("help", TMA_MSG)));
  } else {
    if (tkinput[1] != NULL && strcmp(tkinput[1], "cd") == 0) {
      write(STDOUT_FILENO, FORMAT_MSG("cd", CD_HELP_MSG),
            strlen(FORMAT_MSG("cd", CD_HELP_MSG)));
    } else if (tkinput[1] != NULL && strcmp(tkinput[1], "exit") == 0) {
      write(STDOUT_FILENO, FORMAT_MSG("exit", EXIT_HELP_MSG),
            strlen(FORMAT_MSG("exit", EXIT_HELP_MSG)));
    } else if (tkinput[1] != NULL && strcmp(tkinput[1], "pwd") == 0) {
      write(STDOUT_FILENO, FORMAT_MSG("pwd", PWD_HELP_MSG),
            strlen(FORMAT_MSG("pwd", PWD_HELP_MSG)));
    } else if (tkinput[1] != NULL && strcmp(tkinput[1], "history") == 0) {
      write(STDOUT_FILENO, FORMAT_MSG("history", HISTORY_HELP_MSG),
            strlen(FORMAT_MSG("history", HISTORY_HELP_MSG)));
    } else if (tkinput[1] != NULL && strcmp(tkinput[1], "help") == 0) {
      write(STDOUT_FILENO, FORMAT_MSG("help", HELP_HELP_MSG),
            strlen(FORMAT_MSG("help", HELP_HELP_MSG)));
    } else {
      char errormsg[BUFFER_SIZE];
      snprintf(errormsg, sizeof(errormsg), "%s: %s\n", tkinput[1],
               EXTERN_HELP_MSG);
      write(STDOUT_FILENO, errormsg, strlen(errormsg));
    }
  }
}