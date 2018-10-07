#include "daemon.h"

char *daemon_file_name = "daemon.dm";


/***
 * Daemon creation and store the link directory path, name and the pid in the daemon.dm file
 * @param args entree by the user
 */
void start_deamon(args_t *args){

	// Check if daemon file created
	if(!create_daemon_file()){
		perror("Daemon file error");
		exit(EXIT_FAILURE);
	}

	// Print the search result on command stdout
	if(DEBUG){
		process_files(args,1);
	}

	pid_t process_id = 0;
	pid_t sid = 0;

	// Create child process
	process_id = fork();

	// Indication of fork() failure
	if (process_id < 0){
		perror("fork failed");

		// Return failure in exit status
		exit(EXIT_FAILURE);
	}

	// PARENT PROCESS. Need to kill it.
	if (process_id > 0){

		if(DEBUG){
			printf("process_id of child process %d \n", process_id);
		}

		// return success in exit status
		exit(EXIT_SUCCESS);
	}

	//set new session
	sid = setsid();
	if(sid < 0){

		// Return failure
		exit(EXIT_FAILURE);
	}

	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);

	if(DEBUG){
		close(STDERR_FILENO);
	}

	// Cat child id and link directory
	char *line = malloc(sizeof(char) * MAX_SIZE);
	if(line == NULL){
		perror("line malloc");
        exit(EXIT_FAILURE);
	}
	sprintf(line,"%s;%d;%s", basename(args->link_dir), getpid(), realpath(args->link_dir, NULL));

	// Write child pid and link directory
	write_daemon_file(line);
	free(line);

	search_file(args);

/*	while (1){
		process_files(args,0);
		sleep(1);
	}*/
}

/***
 * Get the pid and link directory path to kill the daemon process
 * @param link_dir
 */
void stop_deamon(char *link_dir){

	// Get pid
	char *pid = get_daemon_pid_path(link_dir, true);

	// If pid found
	if(pid != NULL){

		// Get link directory path
		char *path = get_daemon_pid_path(pid, false);

		// Remove "\n"
		strtok(path, "\n");

		// Remove the line of the actual pid
		remove_daemon_from_file(link_dir);

		// Kill daemon process
		kill(atoi(pid), SIGKILL);

		// Delete all file from link directory
		remove_all_files_from_dir(path);

		// Remove link directory
		remove(path);

		free(pid);
		free(path);
	}else{
		perror("Daemon dosen't exist");
	}
}


/***
 * Create the daemon file
 * @return
 */
bool create_daemon_file(){

	// Check if file dosen't exist
	if(!is_file_exist(daemon_file_name)){

		// Get date time
		char *date_time = get_date_time();

		// Write date time at file start
		FILE *file = fopen(daemon_file_name, "w");
		if (file == NULL){
			perror("Cannot open file");
			return false;
		}
		fprintf(file, "%s\n", date_time);
		fclose(file);
		free(date_time);
	}
	return true;
}


/***
 * Write in the daemon file
 * @param text
 */
void write_daemon_file(char *text){

	// Check if file dosen't exist
	if(is_file_exist(daemon_file_name)){

		// Write append in the file
		FILE *file = fopen(daemon_file_name, "a");
		if (file == NULL){
			perror("Cannot open file");
		}
		fprintf(file, "%s\n", text);
		fclose(file);
	}else{
		perror("File dosen't exist");
	}
}

/***
 * Get the pid or link directory path from the daemon file
 * @param value
 * @param pid => 1 = get pid, 2 = get path
 * @return
 */
char *get_daemon_pid_path(char *value, bool pid){
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int num_tokens = 0;
	char **tokens = NULL;
	char *token = NULL;

	// Read line from file
	FILE *file = fopen(daemon_file_name, "r");
	if (file == NULL){
		perror("Cannot open file");
	}

	// Walk through the file by line
	while ((read = getline(&line, &len, file)) != -1) {

		// Split the line
		tokens = split(line, ";",&num_tokens);

		// Check number of token (must be 3)
		if(num_tokens == 3){

			// basename() may modify the contents of path, so it may be desirable to pass a copy when calling one of those functions
			char *dup_value = strdup(value);


			// Check if we wanna get pid or full path
			if(pid){

				// Compare given directory with directory in the line
				if(strcmp(tokens[0], basename(dup_value)) == 0){
					token = strdup(tokens[1]);

					// Free tokens memory
					free_split(tokens, num_tokens);
					free(dup_value);

					// Return pid
					return token;
				}
			}else{

				// Compare given pid with pid in the line
				if(strcmp(tokens[1], dup_value) == 0){
					token = strdup(tokens[2]);

					// Free tokens memory
					free_split(tokens, num_tokens);
					free(dup_value);

					// Return full path
					return token;
				}
			}

			// Free tokens memory
			free_split(tokens, num_tokens);
			free(dup_value);
		}
	}
	fclose(file);

	// Free memory
	free(line);

	return NULL;
}

/***
 * Remove the line of the daemon process
 * @param link_dir
 */
void remove_daemon_from_file(char *link_dir){
	struct stat st;
	if (stat(daemon_file_name, &st) != -1){

		// Open the file in binary format
		FILE* fp = fopen(daemon_file_name, "rb" );
		if ( fp != NULL ){

			// Allocate memory to hold file
			char* buffer = malloc(st.st_size);
			if(buffer == NULL){
				perror("buffer malloc");
		        exit(EXIT_FAILURE);
			}
			// Read the file into a buffer
			if (fread(buffer, 1, st.st_size, fp) == st.st_size){
				fclose(fp);

				size_t newSize = remove_line_file( buffer, st.st_size, basename(link_dir));

				fp = fopen( daemon_file_name, "wb" );
				if ( fp != NULL ){
					fwrite(buffer, 1, newSize, fp);
					fclose(fp);
				}
				else{
					perror(daemon_file_name);
				}
			}
			free(buffer);
		}
		else{
			perror(daemon_file_name);
		}
	}
	else{
		printf("did not find %s", daemon_file_name);
	}
}
