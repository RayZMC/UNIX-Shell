#ifndef HISTORY_H
#define HISTORY_H
#include <stdbool.h>

void add_to_history(char *command);
void display_history();
bool history_command_handler(char *buffer);
void process_command(char *buffer);

#endif