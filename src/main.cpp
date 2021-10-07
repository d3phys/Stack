#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/stack.h"
#include "include/log.h"
#include "include/hash.h"


int main() 
{
        stack_t stk = {};
$       (log_dump(&stk);)

$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (construct_stack(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (int h = stk.hash);
$       (stk.items[0] = 9);
$       (stk.hash = h);
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
        destruct_stack(&stk);
        
        return 0;
}
