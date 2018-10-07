
#ifndef PARSER_H_
#define PARSER_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

typedef struct args_t{
	char *pre_expression;
	char *post_expression;
	char *search_dir;
	char *link_dir;
	bool not;
}args_t;

#include "function.h"
#include "daemon.h"

int parse(int argc, char **argv);
bool check_occ_in_array(char **array, char *occ, int array_size);
bool check_args_path(int argc, char **argv);
int check_args_exp(int argc, char **argv, char **operator, char **option);
char **split(const char* str, const char* delim, int* num_tokens);
void free_split(char **input, int size);

#endif /* PARSER_H_ */
