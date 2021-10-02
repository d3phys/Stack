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
//j        int i = 'u'<< sizeof(4);
//        log("I:   %c", i);
//        log("I:   %d", i);
      
        size_t key = 0x32321323ffaafda;
        size_t key2= 0x33321323ffaafda;
$       (log("Hash: %x\n", murmur_hash(&key, sizeof(key), 123));)
$       (log("Hash: %x\n", murmur_hash(&key, sizeof(key), 12));)
$       (log("Hash: %x\n", murmur_hash(&key2, sizeof(key2), 123));)


$       (construct_stack(&stk, 10);)
$       (log_dump(&stk);)

$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
        
$       (pop_stack(&stk);) 
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (*((char *)stk.items - 2) = 32;);
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (*((char *)stk.items - sizeof(canary_t) + 2) = 32;);
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (push_stack(&stk, 'f');) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 
$       (log_dump(&stk);)
$       (pop_stack(&stk);) 

$       (log_dump(&stk);)
        /*void *d = calloc(2, sizeof(float));
        log("d: 0x%lx\n", (size_t)d);
        void *a = calloc(2, sizeof(int));
        log("A: 0x%lx\n", (size_t)a);
        void *b = calloc(3, sizeof(char));
        log("B: 0x%lx\n", (size_t)b);
        void *c = calloc(1, sizeof(char));
        log("C: 0x%lx\n", (size_t)c);
        free(a);
        free(b);
        free(c);
        free(d);
        */
        // heap debug functions

        /*
        push_stack(&stk, 21);
        push_stack(&stk, 21);
        log_dump(&stk);
        pop_stack(&stk);
        pop_stack(&stk);
        pop_stack(&stk);
        pop_stack(&stk);

        log("Sum: %d\n", pop_stack(&stk) +  pop_stack(&stk));
        //push_stack(&stk, 32);
*/
        destruct_stack(&stk);
        
        return 0;
}
