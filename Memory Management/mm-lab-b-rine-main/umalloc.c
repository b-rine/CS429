#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <assert.h>
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Brian Nguyen; bn6652" ANSI_RESET;

/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;

/*
 * block_metadata - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL);
    return block->block_metadata & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_metadata |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_metadata &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_metadata & ~(ALIGNMENT-1);
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_metadata = size | alloc;
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required. 
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    // following the first-fit implementation
    memory_block_t *temp = free_head;   
    while (temp->next != NULL) {

        // deferred coalescing for better performance
        if (temp != free_head) {
            coalesce(temp->next);
        }

        // return previous pointer to avoid extra while loops
        if (get_size(temp->next) >= size) {
            return temp;
        }
        temp = temp->next;
    }
    return temp;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    // adjust new size in case we need more than one PAGESIZE
    size_t new_size = PAGESIZE;
    while (new_size < size) {
        new_size += 1;
    }
    memory_block_t *new_block = csbrk(new_size);
    memory_block_t *temp = free_head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_block;

    // make the entire new block free because we will split anyways
    put_block(new_block, new_size, false);
    return new_block;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    // gets called by malloc & free block needs to be added to free list
    memory_block_t *temp = free_head;
    while (temp->next != NULL && temp->next != block) {
        temp = temp->next;
    }

    // freeing the back
    temp->next = (memory_block_t *) ((char *) block + size);
    put_block(temp->next, get_size(block) - size, false);
    
    // allocating the front
    temp->next->next = block->next;
    put_block(block, size, true);
    return block;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    // make sure the next block isn't null 
    memory_block_t *after = block->next;
    if (after != NULL && (char *) block + get_size(block) == (char *) after) {

        // check if there are more free blocks on the right that are contiguous
        size_t total_size = get_size(after);
        while ((char *) after + get_size(after) == (char *) after->next) {
            after = after->next;
            total_size += get_size(after);
        }
        memory_block_t *temp = after->next;

        // combine all contiguous free blocks into the starting block at the left
        put_block(block, get_size(block) + total_size, false);
        block->next = temp;
    }    
    return block;
}

/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // dummy header
    free_head = (memory_block_t *) csbrk(16);
    put_block(free_head, 16, false);

    // actual free list
    free_head->next = (memory_block_t *) csbrk(4 * PAGESIZE);
    put_block(free_head->next, 4 * PAGESIZE, false);
    if (free_head->next == NULL) {
        return -1;
    }
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    // adjust for alignment and header
    if (size == 0) {
        return NULL;
    }
    size_t total_size = ALIGN(size) + 16;

    // find where to place block
    memory_block_t *prev = find(total_size);
    memory_block_t *allocated = prev->next;
    if (allocated == NULL) {
        allocated = extend(total_size);
    } 

    // split here to allocate, only split when free block is 1.05x the necessary space
    // if (get_size(allocated) >= 21 * total_size / 20) {
    if (get_size(allocated) >= 21 * total_size / 20) {
        return get_payload(split(allocated, total_size));
    } else {
        // normal allocation
        prev->next = allocated->next;
        put_block(allocated, total_size, true);
        return get_payload(allocated);
    }
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    // grab block we want to free
    memory_block_t *block = get_block(ptr);
    if (is_allocated(block)) {
        deallocate(block);

        // curr is the previous block of the one we freed
        memory_block_t *curr = free_head;
        while (curr->next != NULL && curr->next < block) {
            curr = curr->next;
        }
        block->next = curr->next;
        curr->next = block;
    }
}