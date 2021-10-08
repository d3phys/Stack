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
        for (size_t i = 0; i < 50; i++) {
                push_stack(&stk, i);
        }
        destruct_stack(&stk);
        
        return 0;
}
