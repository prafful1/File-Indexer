#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "message_queue.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "hash_map.h"

#define PATH_MAX 4096

// Flag is set to 1 when scanner thread is done with 
// searching all ".txt" files and added to message queue.
int list_full_flag = 0;

// Main structure points to message queue and hash map.
struct list_struct {

        ll_t *list;
	hash_map_struct_t *h_map;
        int *val;
	char file_path[PATH_MAX];
};


void *add_path_to_msg_queue(void *arg);
void *extract_path_from_msg_queue(void *arg);
void listFilesRecursively(void *arg);

// Determine if a file ends with ".txt"
int EndsWithtxt( char *string )
{
  char *string1 = strrchr(string, '.');

  if( string1 != NULL )
        if(!strcmp(string1, ".txt")) {
                return 0;
        }

  return( -1 );
}

// Reads a file.
// Break file's contents into tokens.
// Add each token into hash map.
int read_file(char *path, hash_map_struct_t *h_map) {

        FILE *fptr;
	char buffer[255];
        char *content;
	char *rest;
        fptr = fopen(path, "r");

        if(fptr == NULL) {
                printf("Cannot open file \n");
                exit(0);
        }


	while(fgets(buffer, 255, (FILE*) fptr)) {


		rest = buffer;

		while((content = strtok_r(rest, " ,.-", &rest))) {

			insert_modify(h_map, hash(content), content);

		}
	}

        fclose(fptr);
        return 0;
}


/**
 *  *  * Lists all files and sub-directories recursively
 *   *   * considering path as base path.
 *    *    */
void listFilesRecursively(void *arg)
{
	char path[PATH_MAX];
	struct dirent *dp;
	struct list_struct *l = arg;
	char basePath[PATH_MAX];
	
	DIR *dir;
	
	strcpy(basePath, l->file_path);
	dir  = opendir(basePath);

	if (!dir)
		return;

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			int p = EndsWithtxt(dp->d_name);

			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			if(p == 0) {
				//printf("Path %s\n", path);
				strcpy(l->file_path, path);
				ll_insert_last_2(l->list, l->file_path);
			}

			strcpy(l->file_path, path);
			listFilesRecursively((void *)l);
		}
	}

	closedir(dir);
}

// Adds a file path to message queue.
void *add_path_to_msg_queue(void *arg) {

	struct list_struct *l = arg;	
	list_full_flag = 0;
	//printf("Path: %s\n", l->file_path);
	listFilesRecursively((void *)l);
	list_full_flag = 1;
	return NULL;
}

// Extracts path from message queue by worker thread.
// Calls read_file on each file path.
void *extract_path_from_msg_queue(void *arg) {

	char file_path[PATH_MAX];
	int ret;
	struct list_struct *l = (struct list_struct *)arg;
	
	
	while((list_full_flag == 0) || (l->list->hd != NULL)){
		ret = ll_get_first_element(l->list, file_path);
		
		if(ret == 0) {

			printf("File Path extracted from message queue %s\n", file_path);
			read_file(file_path, l->h_map);

		}

	}
	return NULL;
}

void help() {

	printf("--help: ./file_indexer <Directory_Path> \n");
}

void main(int argc, char** argv) {

	// Scanner thread
	pthread_t thread_id1;

	// Worker Threads
	pthread_t thread_id2;
	pthread_t thread_id3;
	pthread_t thread_id4;
	pthread_t thread_id5;

	DIR *dir;
	struct list_struct *args = NULL;

	if(argc < 2 || argc > 2) {
		
		help();
		return;		
	}

	dir  = opendir(argv[1]);

	if(!dir) {
		printf("Directory path is invalid \n");
		help();
		return;
	}

	ll_t *list = ll_new();

	hash_map_struct_t *h_map = hash_map_new();

	args = (struct list_struct *)malloc(sizeof(struct list_struct));
	
	if(args == NULL)
        {
                printf("Error! memory not allocated structure that points to hash map and linked list. \n");
                exit(0);
        }
	
	args->list = list;
	args->h_map = h_map;

	strcpy(args->file_path, argv[1]);	

	printf("######################################################\n");
	
	if (pthread_create(&thread_id1, NULL, add_path_to_msg_queue, (void *)args) != 0) {
		perror("Error: Scanner thread could not be created \n");
		exit(1);
	}else {
		printf("Scanner Thread started \n");
	}

	printf("######################################################\n");

	if (pthread_create(&thread_id2, NULL, extract_path_from_msg_queue, (void *)args) != 0) {
		perror("Error: Worker thread 1 could not be created \n");
		exit(1);
	}else {
		printf("Worker Thread 1 started \n");
	}

	printf("######################################################\n");

	if (pthread_create(&thread_id3, NULL, extract_path_from_msg_queue, (void *)args) != 0) {
		perror("Error: Worker thread 2 could not be created \n");
		exit(1);
	}else {
		printf("Worker Thread 2 started \n");
	}

	printf("######################################################\n");


	if(pthread_create(&thread_id4, NULL, extract_path_from_msg_queue, (void *)args) != 0) {
		perror("Error: Worker thread 3 could not be created \n");
		exit(1);
	}else {
		
		printf("Worker Thread 3 started \n");
	}
	
	printf("######################################################\n");

	
	if(pthread_create(&thread_id5, NULL, extract_path_from_msg_queue, (void *)args) != 0) {
		perror("Error: Worker thread 3 could not be created \n");
		exit(1);
	}else {
		printf("Worker Thread 4 started\n");
	}

	printf("######################################################\n");

	pthread_join(thread_id1, NULL);
	pthread_join(thread_id2, NULL);
	pthread_join(thread_id3, NULL);
	pthread_join(thread_id4, NULL);
	pthread_join(thread_id5, NULL);

	printf("Printing Top 10 words \n");

	display_top_10(args->h_map);

	if(args != NULL)
		free(args);

	if(list!= NULL)
		free(list);

	if(h_map != NULL)
		free(h_map);
	
	return;
}
