#ifndef INTERNALCMDS_H
#define INTERNALCMDS_H

#include <stdbool.h>

void handle_exit(char *tkinput[]);
void handle_pwd(char *tkinput[]);
void handle_cd(char *tkinput[], char *prev_dir);
void handle_help(char *tkinput[]);

#endif