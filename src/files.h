
#ifndef FILES_H_
#define FILES_H_

// Inotify define
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * ( EVENT_SIZE + 16 ))
#define DIR_MAX		100000

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <libgen.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <math.h>
#include <sys/inotify.h>

#include "linkedlist.h"
#include "function.h"
#include "parser.h"

char *get_name(char *path);
long get_file_size(char *path);
int get_link_nb(char *path);
char *get_num_permission(char *path, bool format);
char *get_str_permission(char *path, int perm_type);
char *get_group_owner(char *path, bool format);
char *get_owner(char *path, bool format);
time_t get_last_access_time(char *path);
time_t get_last_modification_time(char *path);
time_t get_last_status_change_time(char *path);
void process_files(args_t *args, bool print);
void list_files(const char *name, int id);
bool find_ocurence_in_path(char *path, char *value);
void remove_all_not_number(char *str);
bool is_perm_here(char *group, char *type);
long get_size_letter(char letter);
bool add_file_name(bool not, char *exp, char *file_name);
bool add_file_size(bool not, char *exp, char *file_name);
bool add_file_date_access_change_modify_hours_mins(bool not, char *exp, int last_xxxxxx, bool minutes, char *file_name);
bool add_file_user_group_owner(bool not, char *exp, bool group, char *file_name);
bool add_file_perm(bool not, char *exp, char *file_name);
void init_linedlist();
void add_link(node *data, char *link_dir, bool print);
void search_file(args_t *args);
void remove_watcher();

#endif /* FILES_H_ */
