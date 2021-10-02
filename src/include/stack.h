#ifndef STACK_H_
#define STACK_H_

#include <cstdint>
#include <stdlib.h>

#define CANARY_PROTECT
#define HASH_PROTECT
//#define UNPROTECT

typedef char item_t; 

#ifdef CANARY_PROTECT
typedef uint64_t canary_t;
#endif /* CANARY_PROTECT */

#ifdef HASH_PROTECT
typedef uint32_t hash_t;
#endif /* HASH_PROTECT */


struct stack_t {

#ifdef CANARY_PROTECT
        canary_t left_canary = 0;
#endif /* CANARY_PROTECT */

        item_t *items         = nullptr;
        size_t capacity       = 0;
        size_t size           = 0;

#ifdef HASH_PROTECT
        hash_t hash           = 0;
#endif /* HASH_PROTECT */

#ifdef CANARY_PROTECT
        canary_t right_canary = 0;
#endif /* CANARY_PROTECT */

};

enum stack_error_t {
        STK_BAD_ALLOC = 0xBADA110C, 
        STK_OVERFLOW  = 0x0000F11, 
        STK_INVALID   = 0xABADBABE,
        STK_EMPTY_POP = 0x00000E11,
};

enum invariant_err_t {
        INVALID_CAPACITY    = 1,
        INVALID_SIZE        = 1 << 1,
        INVALID_ITEMS       = 1 << 2,
        INVALID_HASH        = 1 << 3,
        INVALID_DATA_CNRY   = 1 << 4,
        INVALID_STK_CNRY    = 1 << 5,
};

#define log_dump(_stk)               \
        do {                         \
                log("Stack dump\n"); \
                dump_stack(_stk);    \
        } while (0)

void dump_stack(stack_t *const stk);
void construct_stack(stack_t *const stk, size_t capacity, int *const error = nullptr);
void destruct_stack(stack_t *const stk, int *const error = nullptr);
void push_stack(stack_t *const stk, const item_t item, int *const error = nullptr);
item_t pop_stack(stack_t *const stack, int *const error = nullptr);
int verify_stack(stack_t *const stk);

#endif /* STACK_H_ */


