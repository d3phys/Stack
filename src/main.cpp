#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/stack.h"
#include "include/log.h"
#include "include/hash.h"

int main() {

        stack_t stk = {};

        stack_t *stkptr = construct_stack(&stk);
        stkptr += 21;
        stkptr = nullptr;
        pop_stack(&stk);
        for (int i = 0; i < 50; i++) {
                push_stack(&stk, i);
        }
        log_dump(&stk);
        destruct_stack(&stk);
        
        return 0;
}
