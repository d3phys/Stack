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

#ifndef UNPROTECT
static const int FILL_BYTE = 'u';
#else
static const int FILL_BYTE = 0;
#endif /* UNPROTECT */

#ifndef UNPROTECT
static inline const item_t get_poison(const int byte);
static item_t POISON = get_poison(FILL_BYTE);

static inline const item_t get_poison(const int byte) 
{
        item_t poison = 0;
        memset((void *)&poison, byte, sizeof(item_t));
        return poison;
}

#ifdef CANARY_PROTECT
const u_int64_t CANARY = 0xCCCCCCCCCCCCCCCC;
#endif /* CANARY_PROTECT */

#endif /* UNPROTECT */

static const size_t INIT_CAP       = 2;
static const size_t CAP_FACTOR     = 2;
static const size_t CAP_MAX        = ~(SIZE_MAX >> 1);

static inline void set_error(int *const error, int value) 
{
        if (error)
                *error = value;
}


static inline canary_t *left_canary(const void *const items, const size_t capacity)
{
        return (canary_t *)((char *)items - sizeof(canary_t));
}

static inline canary_t *right_canary(const void *const items, const size_t capacity)
{
        return (canary_t *)((char *)items + 
                            (sizeof(item_t) * capacity) + sizeof(void *) -
                            (sizeof(item_t) * capacity) % sizeof(void *));
}

static int verify_empty_stack(const stack_t *const stk)
{
        assert(stk);

#ifndef UNPROTECT
        if (stk->items) {
                return 1;
        }
        if (stk->size) {
                return 1;
        }
        if (stk->capacity) {
                return 1;
        }
#endif /* UNPROTECT */

        return 0;
}

int verify_stack(const stack_t *const stk)
{
        assert(stk);

#ifndef UNPROTECT
        if (stk->capacity > CAP_MAX)
                goto finally;

        if (stk->capacity < INIT_CAP)
                goto finally;

        if (stk->items == nullptr) 
                goto finally;

        if (stk->size > stk->capacity) 
                goto finally;
#endif /* UNPROTECT */

        return 0;

finally:
        return -1;
}

static item_t *realloc_stack(const stack_t *const stk, const size_t capacity)
{
        size_t cap = capacity * sizeof(item_t);

#ifdef CANARY_PROTECT
        cap += sizeof(void *) - cap % sizeof(void *) + 2 * sizeof(canary_t);
#endif

        item_t *items = stk->items;
$       (items  = (item_t *)realloc(items, cap);)

        if (!items) {
                log("Invalid stack reallocation: %s\n", strerror(errno));
                return nullptr;
        }

$       (memset(items + stk->capacity, FILL_BYTE, cap - stk->capacity * sizeof(item_t));)

#ifdef CANARY_PROTECT
        *right_canary(items, capacity) = CANARY ^ (size_t)items;
        *left_canary(items, capacity)  = CANARY ^ (size_t)items;
#endif

        return items;
}

/*
 * @brief Weird and scary stack dump.
 * @param stk stack to dump.
 */
void dump_stack(const stack_t *const stk)
{
        assert(stk);

        log_buf("----------------------------------------------\n");
        log_buf(" Status: %s\n",  verify_stack(stk) ? 
                "<font color=\"red\"><b>error</b></font>" :
                "<font color=\"green\"><b>ok</b></font>");

        log_buf(" Size: %ld\n", stk->size);
        log_buf(" Capacity: %ld\n", stk->capacity);
        log_buf(" Address start: 0x%lx \n", (size_t)stk->items);
        log_buf(" Address   end: 0x%lx \n", (size_t)stk->items + 
                                          sizeof(item_t) * stk->capacity);
        log_buf("----------------------------------------------\n");

#ifdef CANARY_PROTECT
        log_buf(" Left  canary(hex) = %lx\n Address: 0x%lx %s\n", 
                *left_canary(stk->items, stk->capacity), 
                (size_t)left_canary(stk->items, stk->capacity), 
                *left_canary (stk->items, 
                              stk->capacity) == (CANARY ^ (size_t)stk->items)?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");

        log_buf("----------------------------------------------\n");

        log_buf(" Right canary(hex) = %lx\n Address: 0x%lx %s\n", 
                *right_canary(stk->items, stk->capacity), 
                (size_t)right_canary(stk->items, stk->capacity), 
                *right_canary(stk->items, 
                              stk->capacity) == (CANARY ^ (size_t)stk->items)?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");
        log_buf("----------------------------------------------\n");
#endif  /* CANARY_PROTECT */

if (stk->items) for (size_t i = 0; i < stk->capacity; i++) {
                        if (stk->items[i] == POISON)
                                log_buf("| 0x%.4lX stack[%7ld] = %18s |\n", 
                                        sizeof(*stk->items) * i, i, "poison");
                        else
                                log_buf("| 0x%.4lX stack[%7ld] = %18d |\n", 
                                        sizeof(*stk->items) * i, i, stk->items[i]);
                }

        log_buf("----------------------------------------------\n");


        log_buf("\n\n\n");
        log_flush();
}

void construct_stack(stack_t *const stk, size_t capacity, int *const error)
{
        assert(stk);
        item_t *items = nullptr;
        int err = 0;

#ifndef UNPROTECT
$       (err = verify_empty_stack(stk);)
#endif /* UNPROTECT */

        if (err) {
                log("Can't construct (stack is not empty)\n");
                goto finally;
        }

        if (capacity > CAP_MAX) { 
                log("Initial capacity is big: %ld\n", capacity);
                capacity = INIT_CAP;
                goto finally;
        }

        if (capacity < INIT_CAP)
                capacity = INIT_CAP;

        items = realloc_stack(stk, capacity);
        if (!items) {
                log("Invalid stack memory allocation\n");
                err = STK_BAD_ALLOC;
                goto finally;
        }

        stk->capacity = capacity;
        stk->items    = (item_t *)items;
        stk->size     = 0;


#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */

finally:
        if (err) {
                set_error(error, err);
                log_dump(stk);
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

#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */
        if (err) {
                log("Can't push to invalid stack\n");
                goto finally;
        }

        if (expandable(stk)) {
                size_t capacity = stk->capacity;
                capacity *= CAP_FACTOR;

        $       (void *items = realloc_stack(stk, capacity);)
                if (!items) {
                        log("Invalid stack expanding: %s\n", strerror(errno));
                        err = STK_BAD_ALLOC;
                        goto finally;
                }

                stk->capacity = capacity;
                stk->items    = (item_t *)items;
        }

        stk->items[stk->size++] = item;

#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */

finally:
        if (err) {
                set_error(error, err);
                log_dump(stk);
        }
}

static inline int shrinkable(const stack_t *const stk) 
{
        return stk->capacity / (CAP_FACTOR * CAP_FACTOR) + 1 >= stk->size && 
               stk->capacity > INIT_CAP;
}

item_t pop_stack(stack_t *const stk, int *const error)
{
        assert(stk);
        int err = 0;

#ifndef UNPROTECT
        size_t item = POISON;
#else
        size_t item = 0;
#endif /* UNPROTECT */

#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */
        if (err) {
                log("Can't pop item from invalid stack\n");
                goto finally;
        }

        if (stk->size == 0) {
                log("Can't pop from an empty stack\n");
                err = STK_EMPTY_POP;
                goto finally;
        }

        if (shrinkable(stk)) {
                size_t capacity = stk->capacity;
                capacity /= CAP_FACTOR;

$               (void *items = realloc_stack(stk, capacity);)
                if (!items) {
                        log("Invalid stack shrinking: %s\n", strerror(errno));
                        err = STK_BAD_ALLOC;
                        goto finally;
                }

                stk->items = (item_t *)items;
                stk->capacity = capacity;
        }

        item = stk->items[--stk->size];
        stk->items[stk->size] = POISON;

#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */

finally:
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
        stk->items    = nullptr;
        stk->capacity = -1;
        stk->size     = -1;
}

