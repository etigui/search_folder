#ifndef DAEMON_H_
#define DAEMON_H_

#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <signal.h>

#include "function.h"
#include "files.h"

void start_deamon(args_t *args);
void stop_deamon(char *link_dir);
bool create_daemon_file();
void write_daemon_file(char *text);
char *get_daemon_pid_path(char *value, bool pid);
void remove_daemon_from_file(char *link_dir);


#endif /* DAEMON_H_ */
