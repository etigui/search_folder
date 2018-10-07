#include "function.h"

/***
 * Get string date time
 * @return
 */
char *get_date_time(){
	char *buffer = malloc(sizeof(char) * MAX_SIZE);
	if(buffer == NULL){
		perror("buffer malloc");
        exit(EXIT_FAILURE);
	}
	time_t now = time(NULL);
	strftime (buffer, 100, "File created the %Y-%m-%d at %H:%M:%S.000", localtime (&now));
	return buffer;
}

/***
 * Remove all file from specific directory (no recursive)
 * @param path
 */
void remove_all_files_from_dir(char *path){
	struct dirent *files;
	char file[MAX_SIZE];
	DIR *dir = opendir(path);;
	if (dir){
		while ((files = readdir(dir)) != NULL){

			// Build the path for each file in the folder
			sprintf(file, "%s/%s", path, files->d_name);
			remove(file);
		}
		closedir(dir);
	}
}

/***
 * Remove a line from a file
 * @param buffer
 * @param size
 * @param link_dir
 * @return
 */
size_t remove_line_file( char* buffer, size_t size, const char* link_dir){
	// file format assumed to be as specified in the question i.e. name{space}somevalue{space}someothervalue\n
	// find link_dir
	char* p = buffer;
	bool done = false;
	size_t len = strlen(link_dir);
	size_t newSize = 0;
	do{

		// Look for first letter in link_dir
		char* q = strchr( p, *link_dir );
		if ( q != NULL ){

			// Found link_dir ?
			if ( strncmp( q, link_dir, len ) == 0 ){

				// Include \n already in line size
				size_t lineSize = 1;

				// Count number of characters the line has.
				for ( char* line = q; *line != '\n'; ++line){
					++lineSize;
				}

				// Calculate length left after line by subtracting offsets
				size_t restSize = (size_t)((buffer + size) - (q + lineSize));

				// Move block with next line forward
				memmove( q, q + lineSize, restSize );

				// Calculate new size
				newSize = size - lineSize;
				done = true;
			}
			else{

				// Continue search
				p = q + 1;
			}
		}
		else{
			puts( "no such name" );
			done = true;
		}
	}
	while (!done);

	return newSize;
}


/***
 * Check if directory
 * @param path
 * @return
 */
bool is_dir(const char* path) {
	struct stat info;
	if(stat(path, &info) < 0){
		//perror("Stattttttt");
	}
	return S_ISDIR(info.st_mode);
}

/***
 * Check if file exist
 * @param file_name
 * @return
 */
bool is_file_exist(char *file_name){
	if(access( file_name, F_OK ) != -1 ) {

		// File exists
		return true;
	} else {

		// File doesn't exist
		return false;
	}
}

/***
 * Check is symlink
 * @param path
 * @return
 */
bool is_symbolic_link(char *path){
	struct stat info;
	if(lstat(path, &info) < 0){
		perror("Lstat");
	}
	return S_ISLNK(info.st_mode);
}

/***
 * Get random string (new name)
 * @param len
 * @return
 */
char *get_random_string(int len) {

	char *rnd_string = malloc(len *sizeof(char));

    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < len; ++i) {
    	rnd_string[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    rnd_string[len] = 0;
    return rnd_string;
}
