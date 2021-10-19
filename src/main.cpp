#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/stack.h"
#include "include/log.h"
#include "include/hash.h"

int main() {

        stack_t stk = {};

        construct_stack(&stk);
        pop_stack(&stk);
        for (int i = 0; i < 5000; i++) {
                push_stack(&stk, i);
        }
        log_dump(&stk);
        destruct_stack(&stk);
        
        return 0;
}
