#include <pthread.h>

// useful for casting
typedef void (*gen_fun_t)(void *);

// linked list
typedef struct ll ll_t;

// linked list node
typedef struct ll_node ll_node_t;

// linked list structure

struct ll {
	//running length
	int len;

	//pointer to the first node
	ll_node_t *hd;

	//pointer to tail node
	ll_node_t *tail_node;
	
	//mutex for thread safety
	pthread_rwlock_t m;

    	//a function that is called every time a value is deleted
	//with a pointer to that value
	gen_fun_t val_teardown;

	//a function that can print the values in a linked list
	gen_fun_t val_printer;
};

// returns a pointer to an allocated linked list.
// needs a taredown function that is called with
// a pointer to the value when it is being deleted.
ll_t *ll_new(gen_fun_t val_teardown);

int ll_insert_n(ll_t *list, void *val, int n);

int ll_insert_n_2(ll_t *list, void *val, int n);

int ll_insert_first(ll_t *list, void *val);

int ll_insert_last(ll_t *list, void *val);

int ll_insert_last_2(ll_t *list, void *val);

void *ll_get_n(ll_t *list, int n);

int ll_remove_first(ll_t *list);

int ll_remove_n(ll_t *list, int n);

void *ll_get_first(ll_t *list);

int ll_get_first_element(ll_t *list);

void ll_print(ll_t list);

void ll_no_teardown(void *n);
