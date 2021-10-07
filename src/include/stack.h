/**
 * @file 
 * @brief  Stack implementation
 * @author d3phys
 * @date   07.10.2021
 */

#ifndef STACK_H_
#define STACK_H_

#include <cstdint>
#include <stdlib.h>

#define CANARY_PROTECT
#define HASH_PROTECT
/* #define UNPROTECT */

typedef char item_t; 

#ifdef CANARY_PROTECT
typedef uint64_t canary_t;
const u_int64_t CANARY = 0xCCCCCCCCCCCCCCCC;
#endif /* CANARY_PROTECT */

#ifdef HASH_PROTECT
typedef uint32_t hash_t;
const int SEED = 0xDED32BAD;
#endif /* HASH_PROTECT */

/**
 * @brief Stack structure
 * @author d3phys
 */
struct stack_t {

#ifdef CANARY_PROTECT
        canary_t left_canary = 0;        /**< Canary protection from left */
#endif /* CANARY_PROTECT */

        item_t *items         = nullptr; /**< Stack data     */
        size_t capacity       = 0;       /**< Stack capacity */
        size_t size           = 0;       /**< Stack size     */

#ifdef HASH_PROTECT
        hash_t hash           = 0;       /**< Hash protection */
#endif /* HASH_PROTECT */

#ifdef CANARY_PROTECT
        canary_t right_canary = 0;       /**< Canary protection from right */
#endif /* CANARY_PROTECT */

};

/**
 * @brief Stack error codes 
 */
enum stack_error_t {
        STK_BAD_ALLOC = 0xBADA110C, 
        STK_OVERFLOW  = 0x0000F112, 
        STK_INVALID   = 0xABADBABE,
        STK_EMPTY_POP = 0x00000E11,
};

/**
 * @brief Verification error flags 
 */
enum invariant_err_t {
        INVALID_CAPACITY   = 1 << 1,
        INVALID_SIZE       = 1 << 2,
        INVALID_ITEMS      = 1 << 3,
        INVALID_HASH       = 1 << 4,
        INVALID_DATA_LCNRY = 1 << 5,
        INVALID_DATA_RCNRY = 1 << 6,
        INVALID_STK_LCNRY  = 1 << 7,
        INVALID_STK_RCNRY  = 1 << 8,
};

const int STK_EMPTY  = 0x0000000000001;
const int STK_FILLED = 0x0000000000000;

#define log_dump(_stk)               \
        do {                         \
                log("Stack dump\n"); \
                dump_stack(_stk);    \
        } while (0)

/**
 * @brief Dumps stack
 *
 * @param stk Stack to dump
 *
 * It prints stack dump to a log file. 
 * There are a lot of useful information inside.
 */
void dump_stack(stack_t *const stk);

/**
 * @brief Stack constructor
 *
 * @param[out] stk      Stack to create
 * @param[out] error    Error proceeded
 *
 * It is the manual way to create stack. Do not use other ways. 
 */
stack_t *const construct_stack(stack_t *const stk, int *const error = nullptr);

/**
 * @brief Stack destructor 
 *
 * @param      stk      Stack to destroy 
 * @param[out] error    Error proceeded
 *
 * It is the manual way to destroy stack. Do not use other ways. 
 */
stack_t *const destruct_stack(stack_t *const stk, int *const error = nullptr);

/**
 * @brief Stack push
 *
 * @param stk        Stack push to
 * @param item       Item to push
 * @param[out] error Error proceeded
 */
void push_stack(stack_t *const stk, const item_t item, int *const error = nullptr);

/**
 * @brief Stack pop 
 *
 * @param stk        Stack pop from 
 * @param[out] error Error proceeded
 *
 * @return 'Popped' item
 */
item_t pop_stack(stack_t *const stack, int *const error = nullptr);

/**
 * @brief Stack push
 *
 * @param stk Stack to verify 
 *
 * Main stack verifier. Use it to verify damaged stack.
 *
 * @return 
 * - 0 if stack is valid 
 * - non-zero otherwise (check invariant_err_t)
 */
int verify_stack(stack_t *const stk);

#endif /* STACK_H_ */


