#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_indexer.h"
#include "message_queue.h"


struct list_struct {

        ll_t *list;
        int *val;

};

int file_number;

void *add_path_to_msg_queue(void *arg) {


	struct list_struct *l = (struct list_struct *)arg;	

	int temp = 0;
	while(temp < 20) {
		sleep(1);
		file_number = rand() % 20;

		printf("Input Value %d\n", file_number);

		l->val = &file_number;

		ll_insert_last_2(l->list, l->val);
		temp++;
	}
	return NULL;
}

void *extract_path_from_msg_queue(void *arg) {

	struct list_struct *l = (struct list_struct *)arg;

	
	int temp = 0;
	while(temp < 50){
		sleep(2);
		ll_get_first_element(l->list);
		temp++;
	}
	return NULL;
}

int main() {

	int c = 3;

	pthread_t thread_id1;
	pthread_t thread_id2;

	struct list_struct *args = NULL;
	ll_t *list = ll_new(num_teardown);
	list->val_printer = num_printer;

	args = (struct list_struct *)malloc(sizeof(struct list_struct *));
	args->list = list;

	args->val = &c; 
	
	pthread_create(&thread_id1, NULL, add_path_to_msg_queue, (void *)args);
	pthread_create(&thread_id2, NULL, extract_path_from_msg_queue, (void *)args);
	
	pthread_join(thread_id1, NULL);
	pthread_join(thread_id2, NULL);
}
