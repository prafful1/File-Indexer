#include <stdio.h>
#include <stdlib.h>

#include "file_indexer.hh"


// for locking and unlocking rwlocks along with `locktype_t`
#define RWLOCK(lt, lk) ((lt) == l_read)                   \
				? pthread_rwlock_rdlock(&(lk)) \
				: pthread_rwlock_wrlock(&(lk))
#define RWUNLOCK(lk) pthread_rwlock_unlock(&(lk));

typedef enum locktype locktype_t;

enum locktype {
    l_read,
    l_write
};

struct ll_node {
	
	// pointer to the value at the node
	void *val;

	//pointer to the next node
	ll_node_t *nxt;
   
	//rw mutex
	pthread_rwlock_t m;

};

void num_teardown(void *n) {
	*(int *)n *= -1; // just so we can visually inspect removals afterwards
}


void num_printer(void *n) {
	printf(" %d", *(int *)n);
}

// Allocates a new linked list and initiates its values.

ll_t *ll_new(gen_fun_t val_teardown) {

	ll_t *list = (ll_t *)malloc(sizeof(ll_t));
	list->hd = NULL;
	list->len = 0;
	list->val_teardown = val_teardown;
	pthread_rwlock_init(&list->m, NULL);

	return list;

}

// Just a wrapper for `ll_insert_n` called with 0.

int ll_insert_first(ll_t *list, void *val) {

	return ll_insert_n(list, val, 0);

}

// Makes a new node with the given value.
ll_node_t *ll_new_node(void *val) {
	
	ll_node_t *node = (ll_node_t *)malloc(sizeof(ll_node_t));
	node->val = val;
	node->nxt = NULL;
	pthread_rwlock_init(&node->m, NULL);

	return node;
}


// Inserts a value at the nth position of a linked list.

int ll_insert_n(ll_t *list, void *val, int n) {

	
	printf("Value is inserted at position %d\n", n);
	
	ll_node_t *new_node = ll_new_node(val);
	RWLOCK(l_write, list->m);
	new_node->nxt = list->hd;
	list->hd = new_node;
	(list->len)++;
	RWUNLOCK(list->m);

	return list->len;
}


void *ll_get_first(ll_t *list) {
	
	return ll_get_n(list, 0);

}

// Actually selects the n - 1th element. Inserting and deleting at the front of a
// list do NOT really depend on this.

int ll_select_n_min_1(ll_t *list, ll_node_t **node, int n, locktype_t lt) {

	if (n < 0) // don't check against list->len because threads can add length
		return -1;

	if (n == 0)
		return 0;

	*node = list->hd;
	if (*node == NULL) // if head is NULL, but we're trying to go past it,
		return -1;     // we have a problem

	printf("list->hd is not null \n");

	RWLOCK(lt, (*node)->m);

	ll_node_t *last;

	for (; n > 1; n--) {
		last = *node;
		*node = last->nxt;

		if (*node == NULL) { // happens when another thread deletes the end of a list		
			RWUNLOCK(last->m);
			return -1;
		
		}

		RWLOCK(lt, (*node)->m);
		RWUNLOCK(last->m);

	}

	return 0;

}

// Just a wrapper for `ll_insert_n` called with the index being the length of the linked list

int ll_insert_last(ll_t *list, void *val) {
    return ll_insert_n_2(list, val, list->len);
}

// Inserts a value at the nth position of a linked list.

int ll_insert_n_2(ll_t *list, void *val, int n) {
	
	ll_node_t *new_node = ll_new_node(val);

	ll_node_t *nth_node = NULL;
	
	if (ll_select_n_min_1(list, &nth_node, n, l_write)) {
		free(new_node);
		return -1;
	}
        
	new_node->nxt = nth_node->nxt;
	nth_node->nxt = new_node;
	RWUNLOCK(nth_node->m);

	RWLOCK(l_write, list->m);
	(list->len)++;
	RWUNLOCK(list->m);

	return list->len;
}


// Gets the value of the nth element of a linked list.
void *ll_get_n(ll_t *list, int n) {
	ll_node_t *node = NULL;

	if (ll_select_n_min_1(list, &node, n + 1, l_read))
		return NULL;

	RWUNLOCK(node->m);
    return node->val;
}


// Wrapper for `ll_remove_n`.
int ll_remove_first(ll_t *list) {
    return ll_remove_n(list, 0);
}

// Removes the nth element of the linked list.

int ll_remove_n(ll_t *list, int n) {
	ll_node_t *tmp;
	
	n = 0;
	printf("Value in n %d\n", n);

	RWLOCK(l_write, list->m);
	tmp = list->hd;

	printf("Before NULL check \n");

	if(tmp == NULL) {
		RWUNLOCK(list->m);
		return -1;	
	}

	printf("After NULL check \n");

	if(tmp->nxt == NULL)
		printf("tmp->nxt is null \n");		

	list->hd = tmp->nxt;

	(list->len)--;
	RWUNLOCK(list->m);

	list->val_teardown(tmp->val);
	free(tmp);

	return list->len;
}


int main() {

	int *_n;
	int c = 2;
	int d = 100;

	int test_count = 1;
	int fail_count = 0;

	ll_t *list = ll_new(num_teardown);
	list->val_printer = num_printer;

	ll_insert_first(list, &c);
	_n = (int *)ll_get_first(list);

	if (!(*_n == c)) {
		fprintf(stderr, "FAIL Test %d: Expected %d, but got %d.\n", test_count, c, *_n);
		fail_count++;

	} else {
		
		fprintf(stderr, "PASS Test %d!\n", test_count);
		fprintf(stderr, "Value: %d\n", *_n);

	}

	test_count++;

	ll_insert_last(list, &d);
	
	ll_remove_first(list);
	_n = (int *)ll_get_first(list);

	if (!(*_n == d)) {
                fprintf(stderr, "FAIL Test %d: Expected %d, but got %d.\n", test_count, d, *_n);
                fail_count++;

        } else {

                fprintf(stderr, "PASS Test %d!\n", test_count);
                fprintf(stderr, "Value: %d\n", *_n);

        }

	ll_remove_first(list);

	printf("after ll_remove \n");

	//_n = (int *)ll_get_first(list);

	void *v = ll_get_first(list);

	if(v == NULL)
		printf("Value returned is NULL \n");

	//fprintf(stderr, "Value of _n: %d\n", *_n);

	ll_remove_first(list);
}
