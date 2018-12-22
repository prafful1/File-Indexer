#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// Maximum Number of Words allowed in Hash Map.
#define SIZE 1000000 

// Allowed length of each word.
#define WORD_SIZE 1000


// Hash Map Element.
struct DataItem {
	int data;
	unsigned long key;
	char word[WORD_SIZE];
};

// Thread-safe Hash Map Structure to store map elements.
struct hash_map_struct {

	// hash map buckets
	struct DataItem* hashArray[SIZE];
	
	// mutex for thread safety
	pthread_rwlock_t m;	
};

struct DataItem* item;

typedef struct hash_map_struct hash_map_struct_t;

// Allocates a new hash map and initializes lock
hash_map_struct_t *hash_map_new() {

	hash_map_struct_t *h_map = (hash_map_struct_t *)malloc(sizeof(hash_map_struct_t));

	if(h_map == NULL)                     
	{
		printf("Error! memory not allocated.");
		exit(0);
	}

	pthread_rwlock_init(&h_map->m, NULL);
	
	return h_map;	

}

// Hash function djb2 by Dan Bernstein
unsigned long hash(unsigned char *str) {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;

}


// Inserts a new word in hash map if word does not exist
// Increments count of a word in map, if word exists
void insert_modify(hash_map_struct_t *h_map, unsigned long key, char *word) {

	struct DataItem *item;

	unsigned long bucket_number = key;
	
	while(bucket_number > SIZE) {
		
		bucket_number = bucket_number % SIZE;
		
	}

	RWLOCK(l_write, h_map->m);
	while(h_map->hashArray[(int)bucket_number] != NULL) {
	
		if((h_map->hashArray[(int)bucket_number])->key == key) {
			
			(h_map->hashArray[(int)bucket_number])->data += 1;
			RWUNLOCK(h_map->m);
			return;			

		}
		
		bucket_number = bucket_number + 1;
		bucket_number %= SIZE;

	}

	if(h_map->hashArray[(int)bucket_number] == NULL) {
		
		item = (struct DataItem*) malloc(sizeof(struct DataItem));
		item->data = 1;
		item->key = key;
		strcpy(item->word, word);
		h_map->hashArray[(int)bucket_number] = item;
		
	}

	RWUNLOCK(h_map->m);
	return;
}

// Display Top 10 words with highest frequency from Hash Map. 
void display_top_10(hash_map_struct_t *h_map) {
	
	struct DataItem *item = NULL;
	int i = 0;
	int j = 0;
	int max_freq = 0;
	
	for (j = 0; j<10; j++) {

		for(i = 0; i<SIZE; i++) {

			if(h_map->hashArray[i] != NULL) {
			
				if(h_map->hashArray[i]->data > max_freq)
				{

					max_freq = h_map->hashArray[i]->data;
					item = h_map->hashArray[i];
				}
			}
	
		}
		
		if(item != NULL) {
						
			printf("Word: %s, Frequency: %d\n", item->word, item->data);		
			item->data = 0;
			max_freq = 0;
		}
	}
}


// Print complete Haah Map. 
void display(hash_map_struct_t *h_map) {
   int i = 0;
	
   for(i = 0; i<SIZE; i++) {
	
      if(h_map->hashArray[i] != NULL)
         printf(" (%lu,%d, %s)",h_map->hashArray[i]->key,h_map->hashArray[i]->data,h_map->hashArray[i]->word);
      else
         printf(" ~~ ");
   }
	
   printf("\n");
}

