#include <stdio.h>
//#include "allocator.h"


union header {
        struct {
                union header *next; /* next block on the free list */
                size_t size; /* size of this block in bytes */
        } s;
        long aligner; /* force word alignment of blocks */
};
int main(void) 
{
        printf("%ld  %ld  %ld  %ld\n",sizeof(union header), sizeof(long),sizeof(ssize_t), sizeof(void*));
        return 0;
}