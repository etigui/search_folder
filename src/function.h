
#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_SIZE 2048

bool is_dir(const char* path);
bool is_file_exist(char *file_name);
char *get_date_time();
void remove_all_files_from_dir(char *path);
size_t remove_line_file( char* buffer, size_t size, const char* link_dir);
char *get_random_string(int len);
bool is_symbolic_link(char *path);

#endif /* FUNCTION_H_ */
