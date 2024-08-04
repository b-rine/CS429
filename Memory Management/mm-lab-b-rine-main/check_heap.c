#include "stdio.h"
#include "umalloc.h"

//Place any variables needed here from umalloc.c as an extern.
extern memory_block_t *free_head;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 *
 * STUDENT TODO:
 * Required to be completed for checkpoint 1:
 *
 *      - Ensure that the free block list is in the order you expect it to be in.
 *        As an example, if you maintain the free list in memory-address order,
 *        lowest address first, ensure that memory addresses strictly ascend as you
 *        traverse the free list.
 *
 *      - Check if any free memory_blocks overlap with each other. 
 *
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // Check that all blocks in the free list are marked free.
    // If a block is marked allocated, return -1.
    memory_block_t *curr = free_head;
    while (curr != NULL && curr->next != NULL) {
        if (is_allocated(curr)) {
            return -1;
        } else if ((char *) curr->next <= (char *) curr) {
            return -1;
        } else if ((char *) curr->next < (char *) curr + get_size(curr)) {
            return -1;
        } else if ((unsigned long) curr % ALIGNMENT != 0) {
            return -1;
        }
        curr = curr->next;
    }
    return 0;
}