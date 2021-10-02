#ifndef STACK_H_
#define STACK_H_

#include <cstdint>
#include <stdlib.h>

#define CANARY_PROTECT
//#define HASH_PROTECT
//#define UNPROTECT

typedef char item_t; 

#ifdef CANARY_PROTECT
typedef uint64_t canary_t;
#endif /* CANARY_PROTECT */

#ifdef HASH_PROTECT
typedef uint64_t hash_t;
#endif /* HASH_PROTECT */


struct stack_t {
        uint64_t canary_begin = 0;
        item_t *items         = nullptr;
        size_t capacity       = 0;
        size_t size           = 0;
        uint64_t canary_end   = 0;
};


enum stack_error_t {
        STK_BAD_ALLOC = 0xBADA110C, 
        STK_OVERFLOW  = 0x0000F11, 
        STK_INVALID   = 0xABADBABE,
        STK_EMPTY_POP = 0x00000E11,
};

#define log_dump(_stk)               \
        do {                         \
                log("Stack dump\n"); \
                dump_stack(_stk);    \
        } while (0)

void dump_stack(const stack_t *const stk);

void construct_stack(stack_t *const stk, size_t capacity, int *const error = nullptr);
void destruct_stack(stack_t *const stk, int *const error = nullptr);
void push_stack(stack_t *const stk, const item_t item, int *const error = nullptr);
item_t pop_stack(stack_t *const stack, int *const error = nullptr);

int verify_stack(const stack_t *const stk);

#endif /* STACK_H_ */
