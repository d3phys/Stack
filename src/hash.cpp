#include "include/hash.h"

/*
 * @brief Murmur2 algorithm hash function.
 *
 * @param key     Hash key pointer
 * @param length  Key length
 * @param seed    Random seed
 *
 * This hash function was originally created by Austin Appleby.
 * It is fast and simple hash algorithm.
 *
 * There are a few assumptions about how your machine behaves:
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) = 4
 *
 * Also there are a few limitations:
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */
unsigned int murmur_hash(const void *key, int len, unsigned int seed)
{
        const unsigned int mp = 0x5bd1e995;
        const int sft = 24;

        unsigned int hash = seed ^ len;

        const unsigned char *data = (const unsigned char *)key;

        while(len >= 4) {
                unsigned int k = *(unsigned int *)data;

                k *= mp; 
                k ^= k >> sft; 
                k *= mp; 
                
                hash *= mp; 
                hash ^= k;

                data += 4;
                len -= 4;
        }

        switch(len) {
        case 3: 
                hash ^= data[2] << 16;
        case 2: 
                hash ^= data[1] << 8;
        case 1: 
                hash ^= data[0];

                hash *= mp;
        };

        hash ^= hash >> 13;
        hash *= mp;
        hash ^= hash >> 15;

        return hash;
} 
