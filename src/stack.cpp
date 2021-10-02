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
#include "include/hash.h"

static const int FILL_BYTE = 'u';

#ifdef UNPROTECT
#undef HASH_PROTECT
#undef CANARY_PROTECT
#endif /* UNPROTECT */

static inline const item_t get_poison(const int byte);
static item_t POISON = get_poison(FILL_BYTE);

#ifdef HASH_PROTECT
const int SEED = 0xDED32BAD;
#endif /* HASH_PROTECT */

#ifdef CANARY_PROTECT
const u_int64_t CANARY = 0xCCCCCCCCCCCCCCCC;
#endif /* CANARY_PROTECT */

static const size_t INIT_CAP       = 2;
static const size_t CAP_FACTOR     = 2;
static const size_t CAP_MAX        = ~(SIZE_MAX >> 1);

static inline int expandable(const stack_t *const stk);
static inline int shrinkable(const stack_t *const stk);

#ifdef CANARY_PROTECT
static inline canary_t *left_canary(const void *const items, const size_t capacity);
static inline canary_t *right_canary(const void *const items, const size_t capacity);
#endif /* CANARY_PROTECT */

static int verify_empty_stack(const stack_t *const stk);
static inline void set_error(int *const error, int value);

#ifdef HASH_PROTECT
static hash_t hash_stack(stack_t *const stk)
{
        hash_t hash = stk->hash;
        stk->hash = 0;

        hash_t stk_hash  = murmur_hash(stk, sizeof(stack_t), SEED);
        hash_t data_hash = murmur_hash(stk->items, stk->capacity * sizeof(item_t), SEED);

        stk->hash = hash;
        return stk_hash ^ data_hash;
}
#endif /* HASH_PROTECT */

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

$       (memset(items + stk->size, FILL_BYTE, cap - stk->size * sizeof(item_t));)

#ifdef CANARY_PROTECT
        *right_canary(items, capacity) = CANARY ^ (size_t)items;
        *left_canary(items, capacity)  = CANARY ^ (size_t)items;
#endif

        return items;
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
        stk->left_canary  = CANARY;
        stk->right_canary = CANARY;

#ifdef HASH_PROTECT
$       (stk->hash = hash_stack(stk);)
#endif /* HASH_PROTECT */

#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */

finally:
        if (err) {
                set_error(error, err);
                log_dump(stk);
        }
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

$               (void *items = realloc_stack(stk, capacity);)
                if (!items) {
                        log("Invalid stack expanding: %s\n", strerror(errno));
                        err = STK_BAD_ALLOC;
                        goto finally;
                }

                stk->capacity = capacity;
                stk->items    = (item_t *)items;
        }

        stk->items[stk->size++] = item;

#ifdef HASH_PROTECT
$       (stk->hash = hash_stack(stk);)
#endif /* HASH_PROTECT */

#ifndef UNPROTECT
$       (err = verify_stack(stk);)
#endif /* UNPROTECT */

finally:
        if (err) {
                set_error(error, err);
                log_dump(stk);
        }
}

item_t pop_stack(stack_t *const stk, int *const error)
{
        assert(stk);
        int err = 0;

        size_t item = POISON;

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

#ifdef HASH_PROTECT
$       (stk->hash = hash_stack(stk);)
#endif /* HASH_PROTECT */

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

static int verify_empty_stack(const stack_t *const stk)
{
        assert(stk);
        int err = 0;

        if (stk->capacity)
                err |= INVALID_CAPACITY;

        if (stk->items) 
                err |= INVALID_ITEMS;

        if (stk->size) 
                err |= INVALID_SIZE;

#ifdef HASH_PROTECT
        if (stk->hash != 0)
                err |= INVALID_HASH;
#endif /* HASH_PROTECT */

#ifdef CANARY_PROTECT
        if (stk->left_canary  != 0 ||
            stk->right_canary != 0 )
                err |= INVALID_STK_CNRY;
#endif /* CANARY_PROTECT */

        if (err)
                return err;

        return 0;
}

int verify_stack(stack_t *const stk)
{
        assert(stk);
        int err = 0;
        if (stk->capacity > CAP_MAX || stk->capacity < INIT_CAP)
                err |= INVALID_CAPACITY;

        if (stk->items == nullptr) 
                err |= INVALID_ITEMS;

        if (stk->size > stk->capacity) 
                err |= INVALID_SIZE;

#ifdef HASH_PROTECT
        if (stk->hash != hash_stack(stk))
                err |= INVALID_HASH;
#endif /* HASH_PROTECT */
        
#ifdef CANARY_PROTECT
        canary_t cnry = CANARY ^ (canary_t)stk->items;
        if (*left_canary (stk->items, stk->capacity) != cnry ||
            *right_canary(stk->items, stk->capacity) != cnry )
                err |= INVALID_DATA_CNRY;

        if (stk->left_canary  != CANARY ||
            stk->right_canary != CANARY )
                err |= INVALID_STK_CNRY;

#endif /* CANARY_PROTECT */
        if (err)
                return err;

        return 0;
}

static inline const item_t get_poison(const int byte) 
{
        item_t poison = 0;
        memset((void *)&poison, byte, sizeof(item_t));
        return poison;
}

static inline int expandable(const stack_t *const stk) 
{
        return stk->capacity == stk->size;
}

static inline int shrinkable(const stack_t *const stk) 
{
        return stk->capacity / (CAP_FACTOR * CAP_FACTOR) + 1 >= stk->size && 
               stk->capacity > INIT_CAP;
}

static inline void set_error(int *const error, int value) 
{
        if (error)
                *error = value;
}

#ifdef CANARY_PROTECT
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
#endif /* CANARY_PROTECT */

/*
 * @brief Weird and scary stack dump
 * @param stk Stack to dump
 */
void dump_stack(stack_t *const stk)
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

#ifdef HASH_PROTECT 
        log_buf(" Hash       (hex): %x %s\n", hash_stack(stk), 
                                                hash_stack(stk) == stk->hash ?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");
        log_buf(" Saved hash (hex): %x\n", stk->hash);
        log_buf("----------------------------------------------\n");
#endif  /* HASH_PROTECT */

#ifdef CANARY_PROTECT
        log_buf(" Left  stack canary(hex) = %lx %s\n", CANARY,
                               stk->left_canary == CANARY ?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");
        log_buf(" Right stack canary(hex) = %lx %s\n", CANARY,
                                    stk->right_canary == CANARY ?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");
        log_buf("----------------------------------------------\n");

        log_buf(" Left data canary(hex) = %lx\n Address: 0x%lx %s\n", 
                *left_canary(stk->items, stk->capacity), 
                (size_t)left_canary(stk->items, stk->capacity), 
                *left_canary (stk->items, 
                              stk->capacity) == (CANARY ^ (size_t)stk->items)?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");

        log_buf("\n");

        log_buf(" Right data canary(hex) = %lx\n Address: 0x%lx %s\n", 
                *right_canary(stk->items, stk->capacity), 
                (size_t)right_canary(stk->items, stk->capacity), 
                *right_canary(stk->items, 
                              stk->capacity) == (CANARY ^ (size_t)stk->items)?
                "<font color=\"green\"><b>ok</b></font>" :
                "<font color=\"red\"><b>error</b></font>");
        log_buf("----------------------------------------------\n");
#endif  /* CANARY_PROTECT */

        if (stk->items) {
                for (size_t i = 0; i < stk->capacity; i++) {
                        if (stk->items[i] == POISON) {
                                log_buf("| 0x%.4lX stack[%7ld] = %18s |\n", 
                                        sizeof(*stk->items) * i, i, "poison");
                        } else {
                                log_buf("| 0x%.4lX stack[%7ld] = %18d |\n", 
                                        sizeof(*stk->items) * i, i, stk->items[i]);
                        }
                }
        }

        log_buf("----------------------------------------------\n");


        log_buf("\n\n\n");
        log_flush();
}

