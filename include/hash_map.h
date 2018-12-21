#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define SIZE 10000
#define WORD_SIZE 1000


struct DataItem {
	int data;
	unsigned long key;
	char word[WORD_SIZE];
};

struct hash_map_struct {

	// hash map buckets
	struct DataItem* hashArray[SIZE];
	
	// mutex for thread safety
	pthread_rwlock_t m;	
};

//struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

typedef struct hash_map_struct hash_map_struct_t;


// Allocates a new hash map and initializes lock
hash_map_struct_t *hash_map_new() {

	hash_map_struct_t *h_map = (hash_map_struct_t *)malloc(sizeof(hash_map_struct_t));
	pthread_rwlock_init(&h_map->m, NULL);
	
	return h_map;	

}

int hashCode(int key) {
   return key % SIZE;
}


unsigned long hash(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }


/*struct DataItem *search(int key) {
   //get the hash 
   int hashIndex = hashCode(key);  
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
			
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}*/

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

void *myFuncOne(void *args) {

	hash_map_struct_t *h_map = args; 
	
	insert_modify(h_map, hash("Hello World"), "Hello World");
	insert_modify(h_map, hash("Hello World_2"), "Hello World_2");
	//insert_modify(h_map, hash("Hello World_2"), 1000, "Hello World_2");
	insert_modify(h_map, hash("Hello World_3"), "Hello World_3");
	//insert_modify(h_map, hash("Hello World_2"), 1000, "Hello World_2");

}
