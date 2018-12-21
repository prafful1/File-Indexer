#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_indexer.h"
#include "message_queue.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <read_file.h>
#include "hash_map.h"

#define PATH_MAX 4096

int list_full_flag = 0;

struct list_struct {

        ll_t *list;
	hash_map_struct_t *h_map;
        int *val;
	char file_path[PATH_MAX];
};


void listFilesRecursively(void *arg);

int EndsWithtxt( char *string )
{
  char *string1 = strrchr(string, '.');

  if( string1 != NULL )
        if(!strcmp(string1, ".txt")) {
                return 0;
        }

  return( -1 );
}

int read_file(char *path, hash_map_struct_t *h_map) {

        FILE *fptr;
	char buffer[255];
        char *content;
        fptr = fopen(path, "r");

        if(fptr == NULL) {
                printf("Cannot open file \n");
                exit(0);
        }

        //content = fgets(fptr);

	while(fgets(buffer, 255, (FILE*) fptr)) {

		
		content = strtok(buffer, " ");

		while(content != NULL)
		{
			insert_modify(h_map, hash(content), content);
			//printf("'%s'\n", ptr);
			content = strtok(NULL, " ");
		}	
	}

        /*while(content != EOF) {
                //printf("%c", content);
		insert_modify(h_map, hash(content), content);
                content = fgets(fptr);
        }*/

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
	
	//printf("l->file_path %s\n", l->file_path);
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
				printf("Path %s\n", path);
				strcpy(l->file_path, path);
				ll_insert_last_2(l->list, l->file_path);
			}

			strcpy(l->file_path, path);
			listFilesRecursively((void *)l);
		}
	}

	closedir(dir);
}


void *add_path_to_msg_queue(void *arg) {

	struct list_struct *l = arg;	
	list_full_flag = 0;
	//sleep(1);
	printf("Path: %s\n", l->file_path);
	//strcpy(l->file_path, "/root/text_files-dir");
	listFilesRecursively((void *)l);
	list_full_flag = 1;
	return NULL;
}

void *extract_path_from_msg_queue(void *arg) {

	char file_path[PATH_MAX];
	int ret;
	struct list_struct *l = (struct list_struct *)arg;
	
	
	//int temp = 0;
	while((list_full_flag == 0) || (l->list->hd != NULL)){
		//sleep(2);
		ret = ll_get_first_element(l->list, file_path);

		//printf("Value of list_full_flag %d\n", list_full_flag);
		
		if(ret == 0) {

			printf("Path after retreival %s\n", file_path);
			read_file(file_path, l->h_map);

		}

		//temp++;
	}
	return NULL;
}

int main(int argc, char** argv) {

	pthread_t thread_id1;
	pthread_t thread_id2;
	pthread_t thread_id3;
	pthread_t thread_id4;
	pthread_t thread_id5;
	
	struct list_struct *args = NULL;

	ll_t *list = ll_new(num_teardown);

	hash_map_struct_t *h_map = hash_map_new();

	list->val_printer = num_printer;

	args = (struct list_struct *)malloc(sizeof(struct list_struct));
	args->list = list;
	args->h_map = h_map;

	strcpy(args->file_path, argv[1]);	
	
	pthread_create(&thread_id1, NULL, add_path_to_msg_queue, (void *)args);
	pthread_create(&thread_id2, NULL, extract_path_from_msg_queue, (void *)args);
	pthread_create(&thread_id3, NULL, extract_path_from_msg_queue, (void *)args);
	pthread_create(&thread_id4, NULL, extract_path_from_msg_queue, (void *)args);
	pthread_create(&thread_id5, NULL, extract_path_from_msg_queue, (void *)args);

	pthread_join(thread_id1, NULL);
	pthread_join(thread_id2, NULL);
	pthread_join(thread_id3, NULL);
	pthread_join(thread_id4, NULL);
	pthread_join(thread_id5, NULL);

	display_top_10(args->h_map);

	if(args != NULL)
		free(args);

	if(list!= NULL)
		free(list);

	if(h_map != NULL)
		free(h_map);
}
