#include "main.h"

// prog link_dir search_dir expression1 expression2 expression3
int main(int argc, char **argv) {

	//Ini random with time seed
	srand(time(NULL));

	// Check if enough arguments
	if(argc < 3){
		perror("not enough arguments");
		exit(EXIT_FAILURE);
	}

	if(parse(argc, argv) == -1){
		perror("Parsing argument error");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
