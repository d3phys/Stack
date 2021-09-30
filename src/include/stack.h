#ifndef STACK_H_
#define STACK_H_

#include <stdlib.h>

typedef int item_t; 

struct stack_t {
        item_t *items   = nullptr;
        size_t capacity = 0;
        size_t size     = 0;
};


enum stack_error_t {
        STK_BAD_ALLOC = 0x000A110C, 
        STK_OVERFLOW  = 0x00000F11, 
        STK_INVALID   = 0x00000BAD,
        STK_EMPTY_POP = 0x00000E11,
};

#define log_dump(stk)                 \
        do {                          \
                log("Stack dump\n"); \
                dump_stack(stk);      \
        } while (0)

void construct_stack(stack_t *const stk, size_t capacity, int *const error = nullptr);
void destruct_stack(stack_t *const stk, int *const error = nullptr);
void push_stack(stack_t *const stk, const item_t item, int *const error = nullptr);
item_t pop_stack(stack_t *const stack, int *const error = nullptr);

int verify_stack(const stack_t *const stk);
const char *const _dump_stack(const stack_t *const stk);

#endif /* STACK_H_ */
