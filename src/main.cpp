#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/stack.h"
#include "include/log.h"


int main() 
{
        stack_t stk = {};

        construct_stack(&stk, 1);
        for (int i = 0; i < 10; i++) {
                pop_stack(&stk);
 //               if (i%5 == 0)
//                        dump_stack(&stk);
        }
        destruct_stack(&stk);
        
        return 0;
}
