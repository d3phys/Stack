#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <errno.h>
#include <string.h>

#include "include/stack.h"

#include "include/log.h"

static const item_t POISON = (item_t)0x00000BAD;

static const size_t INIT_CAP       = 2;
static const size_t CAP_FACTOR     = 2;
static const size_t CAP_MAX        = ~(SIZE_MAX >> 1);

static inline void set_error(int *const error, int value) 
{
        if (error)
                *error = value;
}

static int verify_empty_stack(const stack_t *const stk)
{
        assert(stk);

        if (stk->items)
                return 1;
        if (stk->size)
                return 1;
        if (stk->capacity)
                return 1;

        return 0;
}

int verify_stack(const stack_t *const stk)
{
        assert(stk);

        if (stk->capacity > CAP_MAX)
                goto fail;

        if (stk->capacity < INIT_CAP)
                goto fail;

        if (stk->items == nullptr) 
                goto fail;

        if (stk->size > stk->capacity) 
                goto fail;

        if (stk->size < 0) 
                goto fail;

        return 0;
fail:
        return -1;
}


const char *const dump_stack(const stack_t *const stk)
{
        assert(stk);
        static const size_t FIRST = 5;
        size_t size = stk->size + 2;

        while (size > stk->capacity)
                size--;

        log_buf("----------------------------------------------\n");
        log_buf("----------------------------------------------\n");
        log_buf("Status:   %8s \n", verify_stack(stk) ? "error" : "ok");
        log_buf("Size:     %8ld\n", stk->size);
        log_buf("Capacity: %8ld\n", stk->capacity);
        log_buf("----------------------------------------------\n");

        for (size_t i = 0; i < size; i++) {
                log_buf("0x%.4lX stack[%8ld] = %x\n", sizeof(*stk->items) * i, i, stk->items[i]);
        }
        log_buf("capacity: %ld\n", stk->capacity);
        log_buf("size:     %ld\n", stk->size);
        log_buf("----------------------------------------------\n");
        log_buf("----------------------------------------------\n");
        log_flush();

        return 0;
}

void construct_stack(stack_t *const stk, size_t capacity, int *const error)
{
        assert(stk);
        void *items = nullptr;

        int err = 0;
        err = verify_empty_stack(stk);
        if (err)
                goto fail;

        if (capacity > CAP_MAX) {
                log("Initial capacity is big: %ld\n", capacity);
                capacity = INIT_CAP;
                goto fail;
        }

        if (capacity < INIT_CAP) {
                log("Initial capacity is lower than minimal: %ld < %ld\n", capacity, INIT_CAP);
                capacity = INIT_CAP;
        }

        items = calloc(capacity, sizeof(item_t));
        if (!items) {
                log("Invalid stack memory allocation: %s\n", strerror(errno));
                err = STK_BAD_ALLOC;
                goto fail;
        }

        stk->capacity = capacity;
        stk->items    = (item_t *)items;
        stk->size     = 0;

        err = verify_stack(stk);
fail:
        if (err) {
                set_error(error, err);
                dump_stack(stk);
        }
}

static inline int expandable(const stack_t *const stk) 
{
        return stk->capacity == stk->size;
}

void push_stack(stack_t *const stk, const item_t item, int *const error) 
{
        assert(stk);
        int err = 0;

        err = verify_stack(stk);
        if (err)
                goto fail;

        if (expandable(stk)) {
                size_t capacity = stk->capacity;
                capacity *= CAP_FACTOR;

                void *items = realloc(stk->items, sizeof(item_t) * capacity);
                if (!items) {
                        log("Invalid stack expanding: %s\n", strerror(errno));
                        err = STK_BAD_ALLOC;
                        goto fail;
                }

                stk->capacity = capacity;
                stk->items    = (item_t *)items;
        }

        stk->items[stk->size++] = item;

        err = verify_stack(stk);
fail:
        if (err) {
                set_error(error, err);
                dump_stack(stk);
        }
}

static inline int shrinkable(const stack_t *const stk) 
{
        return stk->capacity / (CAP_FACTOR * CAP_FACTOR) >= stk->size && stk->capacity > INIT_CAP;
}

item_t pop_stack(stack_t *const stk, int *const error)
{
        assert(stk);

        int err = 0;
        size_t item = POISON;

        err = verify_stack(stk);
        if (err)
                goto fail;

        if (stk->size == 0) {
                log("Can't pop from an empty stack\n");
                err = STK_EMPTY_POP;
                goto fail;
        }


        if (shrinkable(stk)) {
                size_t capacity = stk->capacity;
                capacity /= CAP_FACTOR;

                void *items = realloc(stk->items, sizeof(item_t) * capacity);
                if (!items) {
                        log("Invalid stack shrinking: %s\n", strerror(errno));
                        err = STK_BAD_ALLOC;
                        goto fail;
                }

                stk->items = (item_t *)items;
                stk->capacity = capacity;
        }

        item = stk->items[--stk->size];
        stk->items[stk->size] = POISON;
fail:
        if (err) {
                set_error(error, err);
                log_dump(stk);
        }

        return item;
}

void destruct_stack(stack_t *const stk, int *const error) 
{
        assert(stk);
        free(stk->items);
}

