#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_indexer.h"
#include "message_queue.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <read_file.h>

#define PATH_MAX 4096

struct list_struct {

        ll_t *list;
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
	sleep(1);
	printf("Path: %s\n", l->file_path);
	strcpy(l->file_path, "/root/text_files-dir");
	listFilesRecursively((void *)l);
	return NULL;
}

void *extract_path_from_msg_queue(void *arg) {

	char file_path[PATH_MAX];
	int ret;
	struct list_struct *l = (struct list_struct *)arg;
	
	
	int temp = 0;
	while(temp < 50){
		sleep(2);
		ret = ll_get_first_element(l->list, file_path);
		
		if(ret == 0) {

			printf("Path after retreival %s\n", file_path);
			read_file(file_path);

		}

		temp++;
	}
	return NULL;
}

int main() {

	pthread_t thread_id1;
	pthread_t thread_id2;
	pthread_t thread_id3;

	struct list_struct *args = NULL;
	ll_t *list = ll_new(num_teardown);
	list->val_printer = num_printer;

	args = (struct list_struct *)malloc(sizeof(struct list_struct));
	args->list = list;

	strcpy(args->file_path, "/dummy123/path123");	
	
	pthread_create(&thread_id1, NULL, add_path_to_msg_queue, (void *)args);
	pthread_create(&thread_id2, NULL, extract_path_from_msg_queue, (void *)args);
	pthread_create(&thread_id3, NULL, extract_path_from_msg_queue, (void *)args);

	pthread_join(thread_id1, NULL);
	pthread_join(thread_id2, NULL);
	pthread_join(thread_id3, NULL);

	if(args != NULL)
		free(args);

	
}
