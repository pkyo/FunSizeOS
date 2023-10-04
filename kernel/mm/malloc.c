//
// Created by root on 10/3/23.
//

#include "../include/linux/mm.h"
#include "../include/linux/kernel.h"
#include "../include/asm/system.h"

struct bucket_desc {    /* 16 bytes */
    void *page;
    struct bucket_desc *next;
    void *freeptr;
    unsigned short refcnt;
    unsigned short bucket_size;
};

struct _bucket_dir {    /* 8 bytes */
    int size;
    struct bucket_desc *chain;
};

struct _bucket_dir bucket_dir[] = {
        {16,   (struct bucket_desc *) 0},
        {32,   (struct bucket_desc *) 0},
        {64,   (struct bucket_desc *) 0},
        {128,  (struct bucket_desc *) 0},
        {256,  (struct bucket_desc *) 0},
        {512,  (struct bucket_desc *) 0},
        {1024, (struct bucket_desc *) 0},
        {2048, (struct bucket_desc *) 0},
        {4096, (struct bucket_desc *) 0},
        {0,    (struct bucket_desc *) 0}
};   /* End of list marker */

/*
 * This contains a linked list of free bucket descriptor blocks
 */
struct bucket_desc *free_bucket_desc = (struct bucket_desc *) 0;

/*
 * This routine initializes a bucket description page.
 */
static inline void init_bucket_desc() {
    struct bucket_desc *bdesc, *first;

    int i;

    first = bdesc = (struct bucket_desc *) get_free_page();
    if (!bdesc)
        return;

    for (i = PAGE_SIZE / sizeof(struct bucket_desc); i > 1; i--) {
        bdesc->next = bdesc + 1;
        bdesc++;
    }

    /*
     * This is done last, to avoid race conditions in case
     * get_free_page() sleeps and this routine gets called again....
     */
    bdesc->next = free_bucket_desc;

    free_bucket_desc = first;
}

void *kmalloc(size_t len) {
    struct _bucket_dir *bdir;
    struct bucket_desc *bdesc;
    void *retval;

    /*
     * First we search the bucket_dir to find the right bucket change
     * for this request.
     */
    for (bdir = bucket_dir; bdir->size; bdir++)
        if (bdir->size >= len)
            break;

    if (!bdir->size) {
        printk("malloc called with impossibly large argument (%d)\n", len);
        return NULL;
    }

    /*
     * Now we search for a bucket descriptor which has free space
     */
    CLI    /* Avoid race conditions */

    for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next)
        if (bdesc->freeptr)
            break;

    /*
     * If we didn't find a bucket with free space, then we'll
     * allocate a new one.
     */
    if (!bdesc) {
        char *cp;
        int i;

        if (!free_bucket_desc)
            init_bucket_desc();

        bdesc = free_bucket_desc;
        free_bucket_desc = bdesc->next;

        bdesc->refcnt = 0;
        bdesc->bucket_size = bdir->size;
        bdesc->page = bdesc->freeptr = (void *) (cp = (char *) get_free_page());

        if (!cp)
            return NULL;

        /* Set up the chain of free objects */
        for (i = PAGE_SIZE / bdir->size; i > 1; i--) {
            // This line of code modifies the content of memory that the `cp` pointer points to,
            // not the value of `cp` itself.
            // It means that we store a pointer to the next position, at the current `cp` position.
            *((char **) cp) = cp + bdir->size;
            // This operation merely changes the value of `cp` and does not alter the content of memory it points to.
            cp += bdir->size;
        }

        *((char **) cp) = 0;

        bdesc->next = bdir->chain; /* OK, link it in! */
        bdir->chain = bdesc;
    }

    retval = (void *) bdesc->freeptr;
    // `*((void **) retval)`
    // Here, we first treat `retval` as a pointer to a pointer.
    // Then, we dereference this pointer-to-pointer to obtain the actual pointer stored at the address pointed to by `retval`.
    // Why do this? Because every unallocated object (at the address pointed to by `freeptr`) essentially stores the address of the next available object,
    // which is the address of the next unallocated object.
    bdesc->freeptr = *((void **) retval);
    bdesc->refcnt++;

    STI    /* OK, we're safe again */

    return (retval);
}

void kfree_s(void *obj, int size) {
    void *page;
    struct _bucket_dir *bdir;
    struct bucket_desc *bdesc, *prev;

    bdesc = prev = 0;

    /* Calculate what page this object lives in */
    // This operation clears (sets to 0) the lower 12 bits of the `obj` address while leaving the other bits unchanged.
    // In a system where the size of a page is 4KB, the lower 12 bits of a page's address are all 0.
    // Thus, through this operation, we can obtain the starting address of the page where `obj` resides.
    page = (void *) ((unsigned long) obj & 0xfffff000);

    /* Now search the buckets looking for that page */
    for (bdir = bucket_dir; bdir->size; bdir++) {
        prev = 0;

        /* If size is zero then this conditional is always false */
        if (bdir->size < size)
            continue;

        for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next) {
            if (bdesc->page == page)
                goto found;

            prev = bdesc;
        }
    }

    return;

    found:
    CLI

    *((void **) obj) = bdesc->freeptr;

    bdesc->freeptr = obj;
    bdesc->refcnt--;

    if (bdesc->refcnt == 0) {
        /*
         * We need to make sure that prev is still accurate.  It
         * may not be, if someone rudely interrupted us....
         */
        if ((prev && (prev->next != bdesc)) || (!prev && (bdir->chain != bdesc)))
            for (prev = bdir->chain; prev; prev = prev->next)
                if (prev->next == bdesc)
                    break;

        if (prev)
            prev->next = bdesc->next;
        else {
            if (bdir->chain != bdesc)
                return;

            bdir->chain = bdesc->next;
        }

        free_page(bdesc->page);
        bdesc->next = free_bucket_desc;
        free_bucket_desc = bdesc;
    }

    STI

    return;
}