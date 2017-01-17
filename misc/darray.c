#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pbc_darray.h"
#include "pbc_memory.h"

//#define NDEBUG
#include <assert.h>

enum {
    max_init = 8
};

void darray_init(darray_ptr a)
{
    a->max = max_init;
    a->count = 0;
    a->item = pbc_malloc(sizeof(void *) * a->max);
}

darray_ptr darray_new(void)
{
    darray_ptr res = pbc_malloc(sizeof(darray_t));
    darray_init(res);
    return res;
}

void darray_remove_all(darray_ptr a)
{
    a->max = max_init;
    a->count = 0;
    pbc_free(a->item);
    a->item = pbc_malloc(sizeof(void *) * a->max);
}

void darray_remove_last(darray_ptr a)
{
    assert(a->count > 0);
    a->count--;
}

void darray_realloc(darray_ptr a, int size)
{
    a->max = size;
    a->item = pbc_realloc(a->item, sizeof(void *) * a->max);
}

void darray_append(darray_ptr a, void *p)
{
    if (a->count == a->max) {
	if (!a->max) a->max = max_init;
	else a->max *= 2;
	a->item = pbc_realloc(a->item, sizeof(void *) * a->max);
    }
    a->item[a->count] = p;
    a->count++;
}

int darray_index_of(darray_ptr a, void *p)
{
    int i;
    for (i=0; i<a->count; i++) {
	if (a->item[i] == p) return i;
    }
    return -1;
}

void darray_clear(darray_t a)
{
    pbc_free(a->item);
    a->max = 0;
    a->count = 0;
}

void darray_show(darray_ptr a)
{
    int i;
    for (i=0;i<a->count;i++) {
	printf("%d: %p\n", i, a->item[i]);
    }
}

void darray_remove_index(darray_ptr a, int n)
{
    assert(a->count >= n-1);
    a->count--;
    memmove(&a->item[n], &a->item[n+1], sizeof(void *) * (a->count - n));
}

void darray_remove(darray_ptr a, void *p)
{
    int i;
    for (i=0; i<a->count; i++) {
	if (a->item[i] == p) {
	    a->count--;
	    memmove(&a->item[i], &a->item[i+1], sizeof(void *) * (a->count - i));
	    return;
	}
    }
    assert(0);
}

void darray_remove_with_test(darray_ptr a, int (*test)(void *))
{
    int i;
    for (i=0; i<a->count; i++) {
	if (test(a->item[i])) {
	    for (;i<a->count; i++) {
		a->item[i] = a->item[i+1];
	    }
	    a->count--;
	}
    }
}

void darray_copy(darray_ptr dst, darray_ptr src)
{
    darray_realloc(dst, src->count);
    memcpy(dst->item, src->item, src->count * sizeof(void *));
    dst->count = src->count;
}

void darray_forall(darray_t a, void (*func)(void *))
{
    int i, n = a->count;
    for (i=0; i<n; i++) {
	func(a->item[i]);
    }
}

/* TODO: uncomment, may be useful?
void *darray_exists(darray_t a, int (*func)(void *))
{
    int i, n = a->count;
    for (i=0; i<n; i++) {
	void *p = a->item[i];
	if (func(p)) return p;
    }
    return NULL;
}
*/
