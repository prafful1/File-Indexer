#include <pthread.h>

// useful for casting
typedef void (*gen_fun_t)(void *);

// linked list
typedef struct ll ll_t;

// linked list node
typedef struct ll_node ll_node_t;

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

	// data value in node
	int data;

	//pointer to the next node
	ll_node_t *nxt;

	//rw mutex
	pthread_rwlock_t m;
};

//linked list structure

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


ll_t *ll_new(gen_fun_t val_teardown);

int ll_insert_last_2(ll_t *list, void *val);

int ll_get_first_element(ll_t *list);

void ll_print(ll_t list);

void ll_no_teardown(void *n);

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
        list->tail_node = NULL;
        list->len = 0;
        list->val_teardown = val_teardown;
        pthread_rwlock_init(&list->m, NULL);

        return list;

}

// Makes a new node with the given value.
ll_node_t *ll_new_node(void *val) {

        ll_node_t *node = (ll_node_t *)malloc(sizeof(ll_node_t));
        node->data = *(int *)val;
        node->nxt = NULL;
        pthread_rwlock_init(&node->m, NULL);

        return node;
}

int ll_insert_last_2(ll_t *list, void *val) {
        ll_node_t *new_node = ll_new_node(val);
        ll_node_t *last_node = NULL;
        RWLOCK(l_write, list->m);

        if(list->hd == NULL) {
                new_node->nxt = list->hd;
                list->hd = new_node;
                list->tail_node = new_node;
        }else {
                last_node = list->tail_node;
                last_node->nxt = new_node;
                list->tail_node = new_node;
        }

        (list->len)++;
        printf("A new node added. New length of list is %d\n", list->len);
        RWUNLOCK(list->m);

        return list->len;
}

int ll_get_first_element(ll_t *list) {

        int null_val = -1;
        int ret;

        ll_node_t *node = NULL;

        RWLOCK(l_write, list->m);
        node = list->hd;

        if(node == NULL) {
                RWUNLOCK(list->m);
                return null_val;
        }

        list->hd = node->nxt;

        if(list->hd == NULL)
                list->tail_node = NULL;

        (list->len)--;

        printf("A node is removed. Length of list is %d\n", list->len);

        printf("Value returned after removing a node %d\n", node->data);
        RWUNLOCK(list->m);

        ret = node->data;

        free(node);

        return ret;
}

