#include "parser.h"

/***
 * Parse user agrument
 * @param argc
 * @param argv
 * @return
 */
int parse(int argc, char **argv){
	bool not = false;
	char *operator[3] = {"-not","-and","-or"};
	char *option[11] =  {"-size","-name","-atime","-ctime","-mtime","-amin","-cmin","-mmin","-user","-group","-perm"};
	char *pre_expression = "";
	char *post_expression = "";

	// Check if delete or create searching directory
	if(argc == 3){
		if(strcmp(argv[1], "-d") != 0){
			perror("wrong command for stop searching");
			return -1;
		}
		stop_deamon(argv[2]);
	}else{
		if(argc < 5){
			perror("not enough arguments");
			return -1;
		}

		// Check if link_dir and search_dir path are ok
		bool path_dir = check_args_path(argc, argv);
		if(!path_dir){
			perror("path arguments error");
			return -1;
		}

		// Check if expression are ok
		int type = check_args_exp(argc, argv, operator, option);
		if(type == -1){
			perror("expression arguments error");
			return -1;
		}else if(type == 1){
			pre_expression = argv[3];
			post_expression = argv[4];
		}else if(type == 2){
			pre_expression = argv[4];
			post_expression = argv[5];
			not = true;
		}

		char link_dir_path[PATH_MAX + 1];
		char search_dir_path[PATH_MAX + 1];

		// !Absolute
		if(argv[1][0] != '/'){
			if(realpath(argv[1], link_dir_path) == NULL){
				perror("link_dir path error");
				return -1;
			}else{
				argv[1] = link_dir_path;
			}
		}

		// !Absolute
		if(argv[2][0] != '/'){
			if(realpath(argv[2], search_dir_path) == NULL){
				perror("search_dir path error");
				return -1;
			}else{
				argv[2] = search_dir_path;
			}
		}

		//Malloc arg struct
		args_t *args = malloc (sizeof (struct args_t));
		if (args == NULL){
			perror("Malloc error");
			return -1;
		}
		args->pre_expression = strdup(pre_expression);
		args->post_expression = strdup(post_expression);
		args->link_dir = strdup(argv[1]);
		args->search_dir = strdup(argv[2]);
		args->not = malloc(1 * sizeof(bool));
		args->not = not;

		start_deamon(args);
	}
	return 1;
}

/***
 * Check link and search directory's
 * @param argc
 * @param argv
 * @return
 */
bool check_args_path(int argc, char **argv){

	char *link_dir = argv[1];
	char *search_dir = argv[2];

	//If directory dont exist
	if(!is_dir(link_dir)){

		// Create link directory
		if (mkdir(link_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
			perror("error creating directory");
			return 0;
		}
	}

	// Check if both directory exist
	if(!is_dir(link_dir) || !is_dir(search_dir)){
		perror("link or search directory dosen't exist");
		return 0;
	}
	return 1;
}

/***
 * Get arguments from expression
 * @param argc
 * @param argv
 * @param operator
 * @param option
 * @return
 */
int check_args_exp(int argc, char **argv, char **operator, char **option){

	// => 0 - 2 => prog, path1, path2
	// => 3 - argc[max] => expression =>>>> -not -atime +60
	// 5 arguments => prog path1 path2 -atime +60
	// 6 arguments => prog path1 path2 -not -atime +60

	if(argc == 5){ // prog path1 path2 -atime +60

		// arg not in the option array
		if(!check_occ_in_array(option, argv[3], 11)){
			return -1;
		}
		return 1;

	}else if(argc == 6){ // prog path1 path2 -not -atime +60

		// if -not
		if((strcmp(argv[3], operator[0])) != 0){
			return -1;
		}

		// arg not in the option array
		if(!check_occ_in_array(option, argv[4], 11)){
			return -1;
		}
		return 2;

	}else{
		return -1;
	}
}

/***
 * Check an occurrence in array exist
 * @param array
 * @param occ
 * @param array_size
 * @return
 */
bool check_occ_in_array(char **array, char *occ, int array_size){

	int  i;
	for(i = 0; i < array_size; i++){
		if(strstr(occ, array[i])){
			return true;
		}
	}
	return false;
}

/***
 * Split string
 * @param str
 * @param delim
 * @param num_tokens
 * @return
 */
char **split(const char* str, const char* delim, int* num_tokens){

	// copy the original string so that we don't overwrite parts of it
	// (don't do this if you don't need to keep the old line,
	// as this is less efficient)
	char *s = strdup(str);

	// these three variables are part of a very common idiom to
	// implement a dynamically-growing array
	int tokens_alloc = 1;
	int tokens_used = 0;
	char **tokens = calloc(tokens_alloc, sizeof(char*));
	char *token, *strtok_ctx;
	for (token = strtok_r(s, delim, &strtok_ctx);
			token != NULL;
			token = strtok_r(NULL, delim, &strtok_ctx)) {

		// check if we need to allocate more space for tokens
		if (tokens_used == tokens_alloc) {
			tokens_alloc *= 2;
			tokens = realloc(tokens, tokens_alloc * sizeof(char*));
		}
		tokens[tokens_used++] = strdup(token);
	}

	// cleanup
	if (tokens_used == 0) {
		free(tokens);
		tokens = NULL;
	} else {
		tokens = realloc(tokens, tokens_used * sizeof(char*));
	}
	*num_tokens = tokens_used;
	free(s);
	return tokens;
}

/***
 * Free spited string
 * @param input
 * @param size
 */
void free_split(char **input, int size){
	int i;
	for(i = 0; i < size; i++){
		free(input[i]);
	}
	free(input);
}


