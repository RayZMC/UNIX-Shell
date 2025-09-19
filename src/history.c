#define _POSIX_C_SOURCE 200809L
#include "history.h"
#include "../include/msgs.h"
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

char history[HISTORY_SIZE][BUFFER_SIZE] = {{0}};
int total_cmds = 0;
int cmd_nums[HISTORY_SIZE] = {0};

void add_to_history(char *cmd) {
  int i = total_cmds % HISTORY_SIZE;

  strncpy(history[i], cmd, BUFFER_SIZE - 1);
  history[i][BUFFER_SIZE - 1] = '\0';

  cmd_nums[i] = total_cmds;
  ++total_cmds;
}

void display_history() {
  int start;
  char cmd_history[BUFFER_SIZE] = {0};

  if (total_cmds > HISTORY_SIZE) {
    start = total_cmds - HISTORY_SIZE;
  } else {
    start = 0;
  }

  for (int i = total_cmds - 1; i >= start; i--) { // iterating reverse order
    int history_index = i % HISTORY_SIZE;
    snprintf(cmd_history, BUFFER_SIZE, "%d\t%s\n", i, history[history_index]);
    write(STDOUT_FILENO, cmd_history, strlen(cmd_history));
  }
}

bool history_command_handler(char *buffer) {
  // handle using strcmp
  char cmd_history[BUFFER_SIZE] = {0};
  if (strcmp(buffer, "!!") == 0) {
    if (total_cmds == 0) {
      char *errormsg1 = FORMAT_MSG("history", HISTORY_NO_LAST_MSG);
      write(STDERR_FILENO, errormsg1, strlen(errormsg1));
      return false;
    } else {
      int i = (total_cmds - 1) % HISTORY_SIZE;
      snprintf(cmd_history, BUFFER_SIZE, "%s\n", history[i]);
      write(STDOUT_FILENO, cmd_history, strlen(cmd_history));
      snprintf(buffer, BUFFER_SIZE, "%s", history[i]);
      add_to_history(buffer);
      return true;
    }

  } else if (buffer[0] == '!' && isdigit(buffer[1])) {
    int cmd_num = atoi(&buffer[1]);
    if (cmd_num >= total_cmds || cmd_num < 0) {
      char *errormsg2 = FORMAT_MSG("history", HISTORY_INVALID_MSG);
      write(STDERR_FILENO, errormsg2, strlen(errormsg2));
      return false;
    }

    else {
      int i = cmd_num % HISTORY_SIZE;
      snprintf(cmd_history, BUFFER_SIZE, "%s\n", history[i]);
      write(STDOUT_FILENO, cmd_history, strlen(cmd_history));
      snprintf(buffer, BUFFER_SIZE, "%s", history[i]);
      add_to_history(buffer);
      return true;
    }
  } else if (buffer[0] == '!') {
    char *errormsg3 = FORMAT_MSG("history", HISTORY_INVALID_MSG);
    write(STDERR_FILENO, errormsg3, strlen(errormsg3));
    return false;
  }
  return false;
}

void process_command(char *buffer) {
  char buffer_copy[BUFFER_SIZE] = {0};
  char cmd[BUFFER_SIZE] = {0};
  // copy
  snprintf(buffer_copy, BUFFER_SIZE, "%s", buffer);

  bool is_history_cmd = history_command_handler(buffer);

  if (is_history_cmd) {
    return;
  }

  strncpy(cmd, buffer, BUFFER_SIZE - 1);
  cmd[BUFFER_SIZE - 1] = '\0';

  if (strcmp(buffer_copy, "!!") != 0 && buffer_copy[0] != '!') {
    add_to_history(cmd);
  }
}