
#include "files.h"

// Global vars for linkedlist
node* llh_search_dir = NULL;
node* llh_link_dir = NULL;

// Global vars for inotify
int wd[DIR_MAX];
int fd;

/***
 *	Notify when a new file is created in the search directory
 * @param args
 */
void search_file(args_t *args) {

	int length, i = 0;
	char buffer[BUF_LEN];

	fd = inotify_init();

	if ( fd < 0 ) {
		perror( "inotify_init" );
	}

	// Add new watcher
	process_files(args, 0);

	while (1){
		struct inotify_event *event;
		length = read(fd, buffer, BUF_LEN);
		if (length < 0) {
			perror( "read" );
		}
		event = (struct inotify_event *)&buffer[i];

		// When new even (file created)
		if (event->len) {

			remove_watcher();

			// Add new watcher
			process_files(args, 0);
		}
	}
	remove_watcher();
	(void)close( fd );
}

/***
 * Remove current watcher
 */
void remove_watcher(){
	int a;
	for(a = 0; a < DIR_MAX; a++){
		(void)inotify_rm_watch( fd, wd[0] );
		(void)inotify_rm_watch( fd, wd[1]);
	}
}

/***
 * Process each file from search directory
 * @param args
 * @param print => 0 = print result on stdout, 1 = add to the linkedlist
 */
void process_files(args_t *args, bool print){

	char *option[11] =  {"-size","-name","-atime","-ctime","-mtime","-amin","-cmin","-mmin","-user","-group","-perm"};
	bool return_val = false;
	int i = 0;

	//Init linkedlist
	init_linedlist();

	// List file from search directory
	list_files(args->search_dir, 0);

	// Walk through current list file
	node *cursor = llh_search_dir;
	while(cursor != NULL){

		// First element created
		if(i != 0){

			// Execute function asked for
			return_val = false;
			if (strcmp(option[0], args->pre_expression) == 0) {
				return_val = add_file_size(args->not, args->post_expression, cursor->file_path);
			}else if(strcmp(option[1], args->pre_expression) == 0){
				return_val = add_file_name(args->not,args->post_expression, cursor->file_path);
			}else if(strcmp(option[2], args->pre_expression) == 0){
				return_val = add_file_date_access_change_modify_hours_mins(args->not, args->post_expression, 1, false, cursor->file_path);
			}else if(strcmp(option[3], args->pre_expression) == 0){
				return_val = add_file_date_access_change_modify_hours_mins(args->not, args->post_expression, 2, false, cursor->file_path);
			}else if(strcmp(option[4], args->pre_expression) == 0){
				return_val = add_file_date_access_change_modify_hours_mins(args->not, args->post_expression, 3, false, cursor->file_path);
			}else if(strcmp(option[5], args->pre_expression) == 0){
				return_val = add_file_date_access_change_modify_hours_mins(args->not, args->post_expression, 1, true, cursor->file_path);
			}else if(strcmp(option[6], args->pre_expression) == 0){
				return_val = add_file_date_access_change_modify_hours_mins(args->not, args->post_expression, 2, true, cursor->file_path);
			}else if(strcmp(option[7], args->pre_expression) == 0){
				return_val = add_file_date_access_change_modify_hours_mins(args->not, args->post_expression, 3, true, cursor->file_path);
			}else if(strcmp(option[8], args->pre_expression) == 0){
				return_val = add_file_user_group_owner(args->not,args->post_expression, false, cursor->file_path);
			}else if(strcmp(option[9], args->pre_expression) == 0){
				return_val = add_file_user_group_owner(args->not,args->post_expression, true, cursor->file_path);
			}else if(strcmp(option[10], args->pre_expression) == 0){
				return_val = add_file_perm(args->not, args->post_expression,cursor->file_path);
			}else{
				perror("Pre argument error");
			}

			// Match expression and file
			if(return_val){

				// Add file to link list
				add_link(cursor, args->link_dir, print);

			}
		}
		cursor = cursor->next;
		i++;
	}

	// Empty linkedlist
	dispose(llh_search_dir);
	llh_search_dir = NULL;
}

/***
 * Check and add file from search directory
 * @param data
 * @param link_dir
 * @param print => 0 = print result on stdout, 1 = add to the linkedlist
 */
void add_link(node *data, char *link_dir, bool print){

	// Search path nd name in linked list
	node *search_path = search(llh_link_dir, data->file_path, 1);
	node *search_name = search(llh_link_dir, data->file_name, 2);

	// If same name already existe and is not the same path
	if((!search_path) && search_name){

		// Add linked file with [new pile path] and [file path]
		char *tmp_path = (char*)malloc(snprintf(NULL, 0, "%s/%s",link_dir, data->new_file_name) + 1);
		sprintf(tmp_path, "%s/%s", link_dir, data->new_file_name);

		// First call of function, parent call it so, display
		if(print){
			printf("%s\n", tmp_path);
		}else{

			// Add in the linkdlist the new path
			llh_link_dir = append(llh_link_dir, strdup(data->file_path), "", strdup(data->new_file_name));//search_path->file_path, "", search_path->new_file_name);

			// If linked file already existe
			if(!is_file_exist(tmp_path)){
				if (symlink(data->file_path, tmp_path) != 0) {
				  perror("symlink() error");
				}
			}
		}
	}else if(!search_path){ // If file do not already existe

		// Add linked file with [file path] and [file path]
		char *tmp_path = (char*)malloc(snprintf(NULL, 0, "%s/%s",link_dir, data->file_name) + 1);
		sprintf(tmp_path, "%s/%s", link_dir, data->file_name);

		// First call of function, parent call it so, display
		if(print){
			printf("%s\n", tmp_path);
		}else{

			// Add in the linkdlist the new path
			llh_link_dir = append(llh_link_dir,strdup(data->file_path), strdup(data->file_name), strdup(data->new_file_name) );

			// If linked file already existe
			if(!is_file_exist(tmp_path)){
				if (symlink(data->file_path, tmp_path) != 0) {
				  perror("symlink() error");
				}
			}
		}
	}
}

/***
 * Init the linked list with special argument
 */
void init_linedlist(){

	// Init linked list
	llh_search_dir = create("///path_z///","///name_z///", "///new_name_z///", NULL);
	if(llh_link_dir == NULL){

		// Random first element created
		llh_link_dir = create("///path_y///","///name_y///", "///new_name_y///", NULL);
	}
}

/***
 * Check if the specified argument (file name) match
 * @param not
 * @param exp
 * @param file_name
 * @return
 */
bool add_file_name(bool not, char *exp, char *file_name){
	if(not){
		if(!find_ocurence_in_path(file_name, exp)){
			return true;
		}
	}else{
		if(find_ocurence_in_path(file_name, exp)){
			return true;
		}
	}
	return false;
}

/***
 * Check if the specified argument (file size) match
 * @param not
 * @param exp
 * @param file_name
 * @return
 */
bool add_file_size(bool not, char *exp, char *file_name){

	long letter = 0;
	long size = 0;
	long file_size = 0;

	// exp = +10M so we wanna remove + and the M to get the size
	char *tmp_size = strdup(exp);
	remove_all_not_number(tmp_size);

	// Convert to long
	size = atol(tmp_size);

	// Check if the last char is an letter
	if(isalpha(exp[(strlen(exp)-1)])){
		letter = tolower(exp[(strlen(exp)-1)]);
		size = size * get_size_letter(letter);
	}

	//Get file size in long
	file_size = get_file_size(file_name);

	if((!isdigit(exp[0])) && (exp[0] == '+')){
		if(not){
			if(file_size < size || file_size == size){
				return true;
			}
		}else{
			if(file_size > size){
				return true;
			}
		}
	}else if((!isdigit(exp[0])) && (exp[0] == '-')){
		if(not){
			if(file_size > size || file_size == size){
				return true;
			}
		}else{
			if(file_size < size){
				return true;
			}
		}
	}else{ // "="
		if(not){
			if(file_size != size){
				return true;
			}
		}else{
			if(file_size == size){
				return true;
			}
		}
	}
	free(tmp_size);
	return false;
}

/***
 * Check if the specified argument (file date) match
 * @param not
 * @param exp
 * @param last_xxxxxx
 * @param minutes
 * @param file_name
 * @return
 */
bool add_file_date_access_change_modify_hours_mins(bool not, char *exp, int last_xxxxxx, bool minutes, char *file_name){

	// Copy expression cause we wanna delete the first occurrence
	char *new_exp = strdup(exp);

	// Delete first occurrence
	if ((exp[0] == '+') || (exp[0] == '-')){
	    memmove(new_exp, new_exp+1, strlen(new_exp));
	}

	// Check if add hours or minutes
	int times = 0;
	if(minutes){
		times = (atoi(new_exp) * 60);
		if(times > 60){
			times = (60 * 60);
		}else if(times < 0){
			times = (1 * 60);
		}
	}else{
		times = ((atoi(new_exp) * 3600) + 3600);
	}

	// Set date from where to start to search
	time_t exp_time;
	time(&exp_time);
	exp_time += -times;
	localtime(&exp_time);
	time_t file_time = 0;

	// Last access => 1
	if(last_xxxxxx == 1){
		file_time = get_last_access_time(file_name);
	}else if (last_xxxxxx == 2){ // Last change => 2
		file_time = get_last_status_change_time(file_name);
	}else if(last_xxxxxx == 3){ // Last modify => 3
		file_time = get_last_modification_time(file_name);
	}

	// file_time is older than exp_time
	if(exp[0] == '-'){
		if(not){
			if((file_time > exp_time) || (file_time == exp_time)){
				return true;
			}
		}else{
			if(file_time < exp_time){
				return true;
			}
		}
	}else if (exp[0] == '+'){ // file_time is newer than exp_time
		if(not){
			if((file_time < exp_time) || (file_time == exp_time)){
				return true;
			}
		}else{
			if(file_time > exp_time){
				return true;
			}
		}
	}else{ // file_time is the same age as exp_time
		if(not){
			if((file_time < exp_time) || (file_time > exp_time)){
				return true;
			}
		}else{
			if(file_time == exp_time){
				return true;
			}
		}
	}
	free(new_exp);
	return false;
}

/***
 * Check if the specified argument (file owner) match
 * @param not
 * @param exp
 * @param group
 * @param file_name
 * @return
 */
bool add_file_user_group_owner(bool not, char *exp, bool group, char *file_name){
	char *name = "";

	// user file owner "string"
	if(isalpha(exp)){

		if(group){
			name = get_group_owner(file_name, false);
		}else{
			name = get_owner(file_name, false);
		}

		if(not){
			if(strcmp(name, exp) != 0){
				return true;
			}
		}else{
			if(strcmp(name, exp) == 0){
				return true;
			}
		}
	}else { // user file owner "int"

		if(group){
			name = get_group_owner(file_name, true);
		}else{
			name = get_owner(file_name, true);
		}

		if(not){
			if(strcmp(name, exp) != 0){
				return true;
			}
		}else{
			if(strcmp(name, exp) == 0){
				return true;
			}
		}
	}
	return false;
}

/***
 * Check if the specified argument (file perm) match
 * @param not
 * @param exp
 * @param file_name
 * @return
 */
bool add_file_perm(bool not, char *exp, char *file_name){

	// Copy expression cause we wanna delete the first occurrence
	char *new_exp = strdup(exp);

	// Delete first occurrence
	if ((exp[0] == '-') || (exp[0] == '/')){
	    memmove(new_exp, new_exp+1, strlen(new_exp));
	}

	// Replace - by 0
	int i;
	for(i = 0; i < strlen(new_exp); i++){
		if(new_exp[0] == '-'){
			new_exp[0] = '0';
		}
	}


	// If one of the given perm are present
	if(exp[0] == '-'){
		char *perm = get_num_permission(file_name, false);
		if(not){
			if(!is_perm_here(new_exp,perm)){
				return true;
			}
		}else{
			if(is_perm_here(new_exp,perm)){
				return true;
			}
		}
		free(perm);
	}else if(exp[0] == '/'){ // If one of the perm match (all perm given for user for exemple)
		char *perm = get_num_permission(file_name, true);
		int i;
		for(i = 0; i < 3; i++){
			if(not){
				if(perm[0] != exp[0] || perm[1] != exp[1] || perm[2] != exp[2]){
					free(perm);
					return true;
				}
			}else{
				if(perm[0] == exp[0] || perm[1] == exp[1] || perm[2] == exp[2]){
					free(perm);
					return true;
				}
			}
		}
		free(perm);
	}else{ // Exact perm
		char *perm = get_num_permission(file_name, true);
		if(not){
			if(strcmp(perm, exp) != 0){
				free(perm);
				return true;
			}
		}else{
			if(strcmp(perm, exp) == 0){
				free(perm);
				return true;
			}
		}
		free(perm);
	}
	free(new_exp);
	return false;
}

/***
 * List all files and add watcher for inotify (one for each directory)
 * @param name
 * @param id
 */
void list_files(const char *name, int id){
	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(name))){
		return;
	}
	if (!(entry = readdir(dir))){
		return;
	}

	wd[id] = inotify_add_watch( fd, name, IN_CREATE);

	do {
		// If directory
		if (entry->d_type == DT_DIR) {

			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
				continue;
			}

			char *path = (char*)malloc(snprintf(NULL, 0, "%s/%s", name, entry->d_name) + 1);
			sprintf(path, "%s/%s", name, entry->d_name);

			list_files(path, id++);

		}
		else{

			// Generate new file name if double
			char *rnd = get_random_string(5);
			char *new_file_name = (char*)malloc(snprintf(NULL, 0, "%s_%s", entry->d_name, rnd) + 1);
			sprintf(new_file_name, "%s_%s", entry->d_name, rnd);

			// Merge directory and filename
			char *file_path = (char*)malloc(snprintf(NULL, 0, "%s/%s", name, entry->d_name) + 1);
			sprintf(file_path, "%s/%s", name, entry->d_name);

			// Check if sym link
			if(is_symbolic_link(file_path)){

				// Get file from sym link and follow sym link
				char real_path[PATH_MAX + 1];
				if(realpath(file_path, real_path) == NULL){
					//perror("sym link error. This error could be a loop detected");
				}else{

					char *rp = (char*)malloc(sizeof(real_path) + 1);
					strcpy(rp, real_path);
					rp[strlen(rp) + 1] = '\0';

					// Get base name of file from sym link
					char *sym_file_name = basename(rp);

					if(sym_file_name != NULL){

						// Generate new file name if double
						char *new_sym_file_name = (char*)malloc(snprintf(NULL, 0, "%s_%s", sym_file_name, rnd) + 1);
						sprintf(new_sym_file_name, "%s_%s", sym_file_name, rnd);

						// Add to the current list
						llh_search_dir = append(llh_search_dir, rp, sym_file_name, new_sym_file_name);
					}
				}
			}else{
				char *d_name = (char*)malloc(sizeof(entry->d_name) + 1);
				strcpy(d_name, entry->d_name);
				d_name[strlen(d_name) + 1] = '\0';

				// Add to the current list
				llh_search_dir = append(llh_search_dir, file_path, d_name, new_file_name);
			}
		}
	} while ((entry = readdir(dir)));
	closedir(dir);
}

/***
 * Check if a specific string is in the path
 * @param path
 * @param value
 * @return
 */
bool find_ocurence_in_path(char *path, char *value){

	// Get name
	char *name = get_name(path);

	// If value found in name => return true
	if(strstr(name,value)){
		free(name);
		return true;
	}
	free(name);
	return false;
}

/**
 * Get file name from path
 * @param path
 * @return
 */
char *get_name(char *path){

	// basename() may modify the contents of path, so it may be desirable to pass a copy when calling that functions
	char *dup_path = strdup(path);

	if(path == NULL){
		perror("File name null");
	}

	char *tmp_path = strdup(basename(dup_path));
	free(dup_path);
	return tmp_path;
}

/**
 *	Get file size
 * @param path: file or directory path
 * @return: size path in bytes
 */
long get_file_size(char *path){
	struct stat info;
	if(stat(path, &info) != 0){
		perror("Stat");
	}
	return info.st_size;
}

/***
 * Get the number of symlink from file
 * @param path
 * @return number of link for the file corresponding
 */
int get_link_nb(char *path){
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}
	return info.st_nlink;
}

/***
 * Get perm from file
 * @param path file or directory path
 * @param format => 0 = 077, 1 = 000000000
 * @return
 */
char *get_num_permission(char *path, bool format){

	int user = 0;
	int group = 0;
	int other = 0;
	char *str = NULL;

	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}
	str = malloc(15 * sizeof(char) + 1);
	if(str == NULL){
		perror("str malloc");
        exit(EXIT_FAILURE);
	}

	// Exact perm
	if(format){
		user = (info.st_mode & S_IRUSR) ? user + 1 : user + 0;
		user = (info.st_mode & S_IWUSR) ? user + 2 : user + 0;
		user = (info.st_mode & S_IXUSR) ? user + 4 : user + 0;
		group = (info.st_mode & S_IRGRP) ? group + 1 : group + 0;
		group = (info.st_mode & S_IWGRP) ? group + 2 : group + 0;
		group = (info.st_mode & S_IXGRP) ? group + 4 : group + 0;
		other = (info.st_mode & S_IROTH) ? other + 1 : other + 0;
		other = (info.st_mode & S_IWOTH) ? other + 2 : other + 0;
		other = (info.st_mode & S_IXOTH) ? other + 4 : other + 0;
		sprintf(str, "%d%d%d", user, group, other);
	}else{

		strcpy(str, (info.st_mode & S_IRUSR) ? "4" : "0");
		strcat(str, (info.st_mode & S_IWUSR) ? "2" : "0");
		strcat(str, (info.st_mode & S_IXUSR) ? "1" : "0");
		strcat(str, (info.st_mode & S_IRGRP) ? "4" : "0");
		strcat(str, (info.st_mode & S_IWGRP) ? "2" : "0");
		strcat(str, (info.st_mode & S_IXGRP) ? "1" : "0");
		strcat(str, (info.st_mode & S_IROTH) ? "4" : "0");
		strcat(str, (info.st_mode & S_IWOTH) ? "2" : "0");
		strcat(str, (info.st_mode & S_IXOTH) ? "1" : "0");
	}
	str[strlen(str) + 1] = '\0';
	return str;
}

/***
 * Get perm from file
 * @param path
 * @param perm_type => 1 = user perm, 2 = group perm, 3 = all user perm
 * @return like rwxrwxrwx
 */
char *get_str_permission(char *path, int perm_type){

	char *permission = NULL;
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}

	permission = malloc(5 * sizeof(char) + 1);
	if(permission == NULL){
		perror("str malloc");
        exit(EXIT_FAILURE);
	}

	// User perm => 1
	if(perm_type == 1){
		strcpy(permission, (info.st_mode & S_IRUSR) ? "r" : "-");
		strcat(permission, (info.st_mode & S_IWUSR) ? "w" : "-");
		strcat(permission, (info.st_mode & S_IXUSR) ? "x" : "-");
	}else if(perm_type == 2){ // Group perm => 2
		strcpy(permission, (info.st_mode & S_IRGRP) ? "r" : "-");
		strcat(permission, (info.st_mode & S_IWGRP) ? "w" : "-");
		strcat(permission, (info.st_mode & S_IXGRP) ? "x" : "-");
	}else if( perm_type == 3){ // All user perm => 3
		strcpy(permission, (info.st_mode & S_IROTH) ? "r" : "-");
		strcat(permission, (info.st_mode & S_IWOTH) ? "w" : "-");
		strcat(permission, (info.st_mode & S_IXOTH) ? "x" : "-");
	}
	permission[strlen(permission) + 1] = '\0';
	return permission;
}

/***
 * Get group owner
 * @param path
 * @param format => 1 = id, 0 = name
 * @return
 */
char *get_group_owner(char *path, bool format){
	char *str = NULL;
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}

	struct group  *gr = getgrgid(info.st_gid);

	// gr_gid eg: 1000
	if(format){
		str = malloc(20 * sizeof(char));
		if(str == NULL){
			perror("str malloc");
	        exit(EXIT_FAILURE);
		}
		sprintf(str, "%d", gr->gr_gid);
		return str;
	}else{ // gr_name eg: adminroot
		return gr->gr_name;
	}
	return NULL;
}

/***
 * Get user owner
 * @param path
 * @param format => 1 = id, 0 = name
 * @return
 */
char *get_owner(char *path, bool format){
	char *str = NULL;
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}

	struct passwd *pw = getpwuid(info.st_uid);

	// pw_gid eg: 1000
	if(format){
		str = malloc(20 * sizeof(char));
		if(str == NULL){
			perror("str malloc");
	        exit(EXIT_FAILURE);
		}
		sprintf(str, "%d", pw->pw_gid);
		return str;
	}else{ // pw_name eg: adminroot
		return pw->pw_name;
	}
	return NULL;
}

/***
 * Get last access time from file
 * @param path
 * @return
 */
time_t get_last_access_time(char *path){
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}
	return info.st_atim.tv_sec;
}

/***
 * Get last modification time from file
 * @param path
 * @return
 */
time_t get_last_modification_time(char *path){
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}
	return info.st_mtim.tv_sec;
}

time_t get_last_status_change_time(char *path){
	struct stat info;
	if(stat(path, &info) < 0){
		perror("Stat");
	}
	return info.st_ctim.tv_sec;
}

/***
 * Get size from usual letter size, like K,M,G,T
 * @param letter
 */
long get_size_letter(char letter){

	if(letter == 'k'){
		return powl(2,10);
	}else if(letter == 'm'){
		return powl(2,20);
	}else if(letter == 'g'){
		return powl(2,30);
	}else if(letter == 'd'){
		return powl(2,40);
	}else{
		return 1;
	}
}

/***
 * Remove all not number from string
 * @param str
 */
void remove_all_not_number(char *str){
  int source = 0; // index of copy source
  int dest = 0; // index of copy destination

  // loop until original end of str reached
  while (str[source] != '\0') {
    if (isdigit(str[source])) {

      // keep only chars matching isalpha()
      str[dest] = str[source];
      ++dest;
    }
    // advance source always, wether char was copied or not
    ++source;
  }
  // add new terminating 0 byte, in case string got shorter
  str[dest] = '\0';
}

/***
 * Get is one of the perm is setter
 * @param group
 * @param type
 * @return
 */
bool is_perm_here(char *group, char *type){

	// 0 = 0 + 0 + 0
	// 1 = 0 + 0 + 1
	// 2 = 0 + 2 + 0
	// 3 = 0 + 2 + 1
	// 4 = 4 + 0 + 0
	// 5 = 0 + 4 + 1
	// 6 = 2 + 4 + 0
	// 7 = 4 + 2 + 1
	// => rwx[user 0-2] rwx[group 3-5] rwx[all users 6-8]

	char *perm[8] = {"000","001","020","012","400","401","420","421"};

	int i, j;
	for(i = 0; i < 3; i++){ // Group
		for(j = 0; j < 3; j++){ // Type
			if(type[j] != '0'){
				char str_group[2] = {group[i] , '\0'};
				char str_type[2] = {type[j], '\0'};
				char *value = perm[atoi(str_group)];
				if(strstr(value, str_type) != NULL){
					return true;
				}
			}
		}
	}
	return false;
}
