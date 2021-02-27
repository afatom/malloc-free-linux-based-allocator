#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>     /* size_t */
#include <unistd.h>     /* sbrk */

union header {
        struct {
                union header *next; /* next block on the free list */
                size_t size; /* size of this block in bytes */
        } s;
        Word aligner; /* force word alignment of blocks */
};

#define nullptr               NULL
#define NU_MIN_ALLOC         (1024)  /* min nunits to request from sbrk */
#define PGS_ALLIGN           (NU_MIN_ALLOC)  /* in the future allign nunits to page size. for now i will leave it as it is*/
#define HEADER_SIZE()        (sizeof(union header))


static union header base;                 /* empty list to get started */
static union header *freep = nullptr;     /* start of free blocks list */





/* simple logging function */
static void log(const char *s)
{
        puts(s);
}


typedef unsigned long Word;







static union header *morecore(size_t nunits)
{
        char *ps;
        union header *hp;

        // make sure that nunits is mults of page size
        if (nunits < NU_MIN_ALLOC)
                nunits = NU_MIN_ALLOC;
        
        /*sbrk() increments the program's data space by increment bytes.*/
        ps = sbrk(nunits * HEADER_SIZE());
        if (ps == (char*)-1) {
                //failed to allocate more memory. return nullptr
                log("morecore: system failed to increase process break, aka allocate more memory");
                return nullptr;
        }

        hp = (union header*)ps;
        hp->s.size = nunits;
        /*now link the payload (the free part of the new allocated buffer) to the free blocks list*/ 
        /*linking the payload is done by calling to free function */
        mm_free((void*)(hp+1));
        return freep;
}


void *mm_allocate(size_t nbytes)
{
        union header *p, *prevp;
        //union header *moreroce(unsigned);
        size_t nunits;
        
        nunits = (nbytes+HEADER_SIZE()-1)/HEADER_SIZE() + 1;
        if ((prevp = freep) == nullptr) { /* first call to mm_malloc, no free list yet */
                base.s.next = freep = prevp = &base;
                base.s.size = 0;
        }

        for (p = prevp->s.next; ; prevp = p, p = p->s.next) {
                if (p->s.size >= nunits) { /* big enough */
                        if (p->s.size == nunits) {/* exactly */
                                prevp->s.next = p->s.next;
                        } else {  /* allocate tail end */
                                p->s.size -= nunits;
                                p += p->s.size;
                                p->s.size = nunits;
                        }
                        freep = prevp;
                        return (void *)(p+1);
                }
                if (p == freep) /* wrapped around free list */
                        if ((p = morecore(nunits)) == nullptr)
                                return nullptr; /* none left */
        }
}

void  mm_free(void *buf)
{

}
