/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "debug.h"
#include "sfmm.h"

int MIN_BLOCK_SIZE = 32;
int ALIGN_SIZE = 16;
uint64_t BLK_SIZE_MASKING_BIT = 0x0000FFF0;

void initHeap();
void addFreeblkInFreelist(sf_block* freeblk);
int freelistIndexFinder(uint64_t size);
sf_block * searchFreelist(sf_size_t size);
sf_block * searchQuicklist(sf_size_t size);
void addFreeblkInQuicklist(sf_block* freeblk);
int isValidAddress(void * addr);
void growHeap();
sf_block* coalescing(sf_block *freeblock);
sf_block *allocate(sf_block * freeblk, sf_size_t requiredSize, sf_size_t payloadSize);

void *sf_malloc(sf_size_t size) {
    debug("given size is : %d",size);
    if (size == 0){
        return NULL;
    }

    if (sf_mem_start() == sf_mem_end()){
        initHeap();
    }

    sf_size_t requiredBlkSize = ((size + 8) % 16) ? ((((size + 8)/16) + 1) * 16 ) : (size + 8) ; 
    if (requiredBlkSize < MIN_BLOCK_SIZE) requiredBlkSize = MIN_BLOCK_SIZE;
    debug("size required is : %d",requiredBlkSize);
    sf_block *freeblk = NULL;
    if (requiredBlkSize < ((NUM_QUICK_LISTS * ALIGN_SIZE) + MIN_BLOCK_SIZE)){         
        freeblk = searchQuicklist(requiredBlkSize);
    }
    if (!freeblk){
        freeblk = searchFreelist(requiredBlkSize);
        if (!freeblk){
            while(!freeblk){
                growHeap();
                if (sf_errno == ENOMEM){
                    return NULL;
                }
                freeblk = searchFreelist(requiredBlkSize);
            }
        }
    }
    if (requiredBlkSize < ((NUM_QUICK_LISTS * ALIGN_SIZE) + MIN_BLOCK_SIZE)){         
        freeblk = allocate(freeblk, requiredBlkSize, size);
    }
    else{
        freeblk = allocate(freeblk, requiredBlkSize, size);
    }

    return (void *)freeblk + 16;
}

void sf_free(void *pp) {
    if (!isValidAddress(pp)){
        abort();
    }
    sf_header *freeingBlkHeader = (sf_header *)(pp - 8);
    sf_size_t freeingBlkSize = (*freeingBlkHeader ^ MAGIC) & BLK_SIZE_MASKING_BIT;
     *freeingBlkHeader = (*freeingBlkHeader << 32) >> 32;
    if (freeingBlkSize >= ((NUM_QUICK_LISTS * ALIGN_SIZE) + MIN_BLOCK_SIZE)){ 
        *freeingBlkHeader ^= MAGIC;
        *freeingBlkHeader = *freeingBlkHeader & ~THIS_BLOCK_ALLOCATED; 
        *freeingBlkHeader ^= MAGIC;
    }

    sf_footer* freeingBlkFooter = (sf_footer*)((void *)freeingBlkHeader + freeingBlkSize - 8);
    *freeingBlkFooter = *freeingBlkHeader;
    sf_header* nextBlkHeader = (sf_header*)((void *)freeingBlkFooter + 8);

    if (freeingBlkSize >= ((NUM_QUICK_LISTS * ALIGN_SIZE) + MIN_BLOCK_SIZE)){ 
        *nextBlkHeader ^= MAGIC;
        *nextBlkHeader = *nextBlkHeader & ~PREV_BLOCK_ALLOCATED;
        *nextBlkHeader ^= MAGIC;
    }
    sf_size_t nextBlkSize = (*nextBlkHeader ^ MAGIC) & BLK_SIZE_MASKING_BIT;
    sf_header *nextBlkFooter =  (sf_header*)((void *)nextBlkHeader + nextBlkSize - 8);
    *nextBlkFooter = *nextBlkHeader;

    sf_block * freeingBlk = (sf_block *)((void *)freeingBlkHeader - 8);
    if (freeingBlkSize < ((NUM_QUICK_LISTS * ALIGN_SIZE) + MIN_BLOCK_SIZE)){  
        addFreeblkInQuicklist(freeingBlk);
    } 
    else{
        freeingBlk = coalescing(freeingBlk);
        addFreeblkInFreelist(freeingBlk);
    }
    // TO BE IMPLEMENTED
    return;
}

void *sf_realloc(void *pp, sf_size_t rsize) {
    if (!isValidAddress(pp)){
        abort();
    }
    if (rsize ==0){
        sf_free(pp); 
        return NULL;
    }
    sf_header * blkHeader = (sf_header *)(pp - 8);
    uint64_t size = (*blkHeader ^ MAGIC) & BLK_SIZE_MASKING_BIT;
    sf_size_t requiredBlkSize = ((rsize + 8) % 16) ? ((((rsize + 8)/16) + 1) * 16 ) : (rsize + 8) ; 
    if (requiredBlkSize < MIN_BLOCK_SIZE) requiredBlkSize = MIN_BLOCK_SIZE;
    debug("size required in realloc is : %d",requiredBlkSize);
    sf_block* newBlk = NULL;
    if (size >= requiredBlkSize){
        newBlk = allocate((sf_block *)(pp - 16), requiredBlkSize, rsize);
        return ((void *)newBlk + 16);
    }
    else{
        newBlk = sf_malloc(rsize);
        if (newBlk){
            memcpy(newBlk, pp, size - 8);
            sf_free(pp);
        }
        return newBlk;
    }
}

double sf_internal_fragmentation() {
    if (sf_mem_start() == sf_mem_end()){
        return 0.0;
    }
    sf_block * blk = (sf_block *)(sf_mem_start() + 32);
    void* blkEnd = sf_mem_end() - 16;
    
    double totalPayload = 0;
    double totalAllocatedSize = 0;

    while( ((void *)blk) != blkEnd){
        long blkSize = ((blk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
        if (((blk -> header) ^ MAGIC) & THIS_BLOCK_ALLOCATED){
            long pldSize = ((blk -> header) ^ MAGIC) >> 32;
            totalAllocatedSize += blkSize;
            totalPayload += pldSize;
        }
        blk = (void *)blk + blkSize;
    }
    if (totalPayload == 0 || totalAllocatedSize == 0){
        return 0.0;
    }

    return (totalPayload/totalAllocatedSize);
}

double sf_peak_utilization() {
    // TO BE IMPLEMENTED

    abort();
}

void initHeap(){
    if (sf_mem_grow() == NULL){
        sf_errno = ENOMEM;
        return ;
    }
    for (int i = 0 ; i < NUM_FREE_LISTS; i++){
        sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
        sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
    }

    void* heapStart = sf_mem_start();
    void* heapEnd = sf_mem_end();

    sf_header *prologue = (sf_header*)(heapStart + 8);
    *prologue = 0;
    *prologue = *prologue | PREV_BLOCK_ALLOCATED | THIS_BLOCK_ALLOCATED | 32;

    *prologue ^=  MAGIC;    ////

    sf_block *freeblk = (sf_block*) (heapStart + 32);
    freeblk -> prev_footer = *prologue;
    freeblk -> header = 0;
    freeblk -> header = (freeblk -> header) | (PAGE_SZ - (8 * 6)) | PREV_BLOCK_ALLOCATED;

    freeblk -> header ^=  MAGIC;    ////

    addFreeblkInFreelist(freeblk);

    sf_footer *epilogue = (sf_footer*)(heapEnd - 8);
    *epilogue  = THIS_BLOCK_ALLOCATED;                                                  /////

    *epilogue ^=  MAGIC;    ////


    sf_footer *epiloguePrefooter = (sf_footer*)(heapEnd - 16);
    *epiloguePrefooter = freeblk -> header;

}

void addFreeblkInFreelist(sf_block* freeblk){
    uint64_t size = ((freeblk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
    int index = freelistIndexFinder(size);
    freeblk -> body.links.next = sf_free_list_heads[index].body.links.next;
    freeblk -> body.links.prev = &sf_free_list_heads[index];
    sf_free_list_heads[index].body.links.next -> body.links.prev = freeblk;
    sf_free_list_heads[index].body.links.next = freeblk;
}

sf_block * searchFreelist(sf_size_t size){
    int index = freelistIndexFinder(size);
    for (int i = index; i < NUM_FREE_LISTS; i++){
        sf_block* head = &sf_free_list_heads[i];
        sf_block* finder = sf_free_list_heads[i].body.links.next;
        while(finder != head){
            if ((((finder -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT) >= size){
                finder -> body.links.prev -> body.links.next = finder -> body.links.next;
                finder -> body.links.next -> body.links.prev = finder -> body.links.prev;
                return finder;
            }
            finder = finder -> body.links.next;
        }
    }
    return NULL;
}

int freelistIndexFinder(uint64_t size){
    int index = size / MIN_BLOCK_SIZE;
    if (index > 256) return NUM_FREE_LISTS - 1;
    else if (index > 128) return NUM_FREE_LISTS - 2;
    else if (index > 64) return NUM_FREE_LISTS -3;
    else if (index > 32) return NUM_FREE_LISTS - 4;
    else if (index > 16) return NUM_FREE_LISTS - 5;
    else if (index > 8) return NUM_FREE_LISTS - 6;
    else if (index > 4) return NUM_FREE_LISTS - 7;
    else if (index > 2) return NUM_FREE_LISTS - 8;
    else if (index > 1) return NUM_FREE_LISTS - 9;
    else if (index > 0) return NUM_FREE_LISTS - 10;
    else return -1;
}

void growHeap(){
    void* heapEnd = sf_mem_end();
    if (sf_mem_grow() == NULL){
        sf_errno = ENOMEM;
        return ;
    }
    sf_footer *previousEpilogue = (sf_footer*)(heapEnd - 8);

    *previousEpilogue ^=  MAGIC;

    *previousEpilogue = *previousEpilogue | PAGE_SZ;
    *previousEpilogue = *previousEpilogue & ~THIS_BLOCK_ALLOCATED;

    *previousEpilogue ^=  MAGIC; 

    heapEnd = sf_mem_end();

    sf_footer *newEpilogue = (sf_footer*)(heapEnd - 8);
    *newEpilogue  = THIS_BLOCK_ALLOCATED;

    *newEpilogue ^=  MAGIC; 
    
    sf_footer *epiloguePrefooter = (sf_footer*)(heapEnd - 16);
    *epiloguePrefooter = *previousEpilogue;

    sf_block * freeblk = coalescing((sf_block *)((void*)previousEpilogue - 8));
    addFreeblkInFreelist(freeblk);
    return ;
}

sf_block* coalescing(sf_block *freeblock){
    sf_block *returnBlk = NULL;
    sf_block *midBlk = freeblock;

    uint64_t midBlkSize = ((midBlk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
    // int isMidAllocated = ((midBlk -> header) & THIS_BLOCK_ALLOCATED) >> 2;

    uint64_t prevBlkSize = ((midBlk -> prev_footer) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
    sf_block *prevBlk = (sf_block *)((void *)midBlk - prevBlkSize);
    int isPrevAllocated = (((prevBlk -> header) ^ MAGIC) & THIS_BLOCK_ALLOCATED) >> 2; 

    sf_block *nextBlk = (sf_block *)((void *)midBlk + midBlkSize);
    uint64_t nextBlkSize = ((nextBlk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT; 
    int isNextAllocated = (((nextBlk -> header) ^ MAGIC) & THIS_BLOCK_ALLOCATED) >> 2;

    debug("isPrevAllocated %d isNextAllocated %d",isPrevAllocated,isNextAllocated);
    if (!isPrevAllocated && !isNextAllocated){
        debug("coalescing 1");
        prevBlk -> header ^= MAGIC;
        prevBlk -> header = prevBlk -> header + midBlkSize + nextBlkSize;
        prevBlk -> header ^= MAGIC;

        sf_footer * nextBlkFooter = ((void*)prevBlk + prevBlkSize + midBlkSize +  nextBlkSize);
        *nextBlkFooter = prevBlk -> header;
        nextBlk -> body.links.prev -> body.links.next = nextBlk -> body.links.next;
        nextBlk -> body.links.next -> body.links.prev = nextBlk -> body.links.prev;
        prevBlk -> body.links.prev -> body.links.next = prevBlk -> body.links.next;
        prevBlk -> body.links.next -> body.links.prev = prevBlk -> body.links.prev;
        returnBlk = prevBlk;
    }
    else if (!isPrevAllocated && isNextAllocated){
        debug("coalescing 2");
        prevBlk -> header ^= MAGIC;
        prevBlk -> header = prevBlk -> header + midBlkSize;
        prevBlk -> header ^= MAGIC; 
        nextBlk -> prev_footer = prevBlk -> header;
        prevBlk -> body.links.prev -> body.links.next = prevBlk -> body.links.next;
        prevBlk -> body.links.next -> body.links.prev = prevBlk -> body.links.prev;
        returnBlk = prevBlk;
    }
    else if (isPrevAllocated && !isNextAllocated){
        debug("coalescing 3");
        midBlk -> header ^= MAGIC;
        midBlk -> header += nextBlkSize;
        midBlk -> header ^= MAGIC;

        sf_footer * nextBlkFooter = (sf_footer *)((void*)nextBlk + nextBlkSize);
        *nextBlkFooter = midBlk -> header;
        nextBlk -> body.links.prev -> body.links.next = nextBlk -> body.links.next;
        nextBlk -> body.links.next -> body.links.prev = nextBlk -> body.links.prev;
        returnBlk = midBlk;
        debug("hekko");
    }
    else{
        debug("coalescing 4");
        returnBlk = midBlk;
    }
    return returnBlk;
}

sf_block *allocate(sf_block * freeblk, sf_size_t requiredSize, sf_size_t payloadSize){
    uint64_t payloadSize64 = ((uint64_t)payloadSize) << 32;
    uint64_t freeBlkSize = ((freeblk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
            freeblk -> header ^= MAGIC;
    freeblk -> header = ((freeblk -> header) << 32) >> 32;
                freeblk -> header ^= MAGIC;

    if (freeBlkSize > requiredSize){
        if ((freeBlkSize - requiredSize) < MIN_BLOCK_SIZE){
            freeblk -> header ^= MAGIC;
            freeblk -> header = freeblk->header | payloadSize64| THIS_BLOCK_ALLOCATED;
            freeblk -> header ^= MAGIC;

            sf_block *nextBlk = (sf_block*) ((void*) freeblk + freeBlkSize);
            nextBlk -> header ^= MAGIC;
            nextBlk -> header |= PREV_BLOCK_ALLOCATED;
            nextBlk -> header ^= MAGIC;

            
            sf_footer* nextBlkFooter = (sf_footer *)((void *)nextBlk + (((nextBlk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT));
            *nextBlkFooter = nextBlk -> header;
        }
        else{
            sf_size_t remainingSize = freeBlkSize - requiredSize;
            freeblk -> header ^= MAGIC;
            freeblk -> header = freeblk->header | payloadSize64 | THIS_BLOCK_ALLOCATED;
            freeblk -> header -= remainingSize;
            freeblk -> header ^= MAGIC;
            
            sf_block *remainingBlk = (sf_block*)((void*)freeblk + requiredSize);
            remainingBlk -> prev_footer = freeblk -> header;
            remainingBlk -> header = 0;
            remainingBlk -> header = remainingBlk -> header | PREV_BLOCK_ALLOCATED | remainingSize;
            remainingBlk -> header ^= MAGIC;

            sf_block *nextBlk = (sf_block*) ((void*) remainingBlk + remainingSize);
            nextBlk -> prev_footer = remainingBlk -> header;

            remainingBlk = coalescing(remainingBlk);
            addFreeblkInFreelist(remainingBlk);
        }
    }
    else{
        freeblk -> header ^= MAGIC;
        freeblk -> header = freeblk->header | payloadSize64 | THIS_BLOCK_ALLOCATED;
        freeblk -> header ^= MAGIC;

        sf_block *nextBlk = (sf_block*) ((void*) freeblk + freeBlkSize);
        nextBlk -> header ^= MAGIC;
        nextBlk -> header |= PREV_BLOCK_ALLOCATED;
        nextBlk -> header ^= MAGIC;

        sf_size_t nextBlkSize = ((nextBlk -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
        sf_footer *nextBlkFooter =  (sf_footer*)((void *)nextBlk + nextBlkSize);
        *nextBlkFooter = nextBlk -> header;

    }
    return freeblk;

}

sf_block * searchQuicklist(sf_size_t size){
    int index = ((size - MIN_BLOCK_SIZE)/ALIGN_SIZE);
    sf_block *freeblk = NULL;
    if (sf_quick_lists[index].length > 0){
        sf_quick_lists[index].length -= 1;
        freeblk = sf_quick_lists[index].first;
        sf_quick_lists[index].first = sf_quick_lists[index].first -> body.links.next;
        freeblk -> header ^= MAGIC;
        freeblk -> header &= ~IN_QUICK_LIST;
        freeblk -> header ^= MAGIC;

        return freeblk;
    }
    else{
        return NULL;
    }
}

void addFreeblkInQuicklist(sf_block* freeblk){
    freeblk -> header ^= MAGIC;
    freeblk -> header |= IN_QUICK_LIST;
    uint64_t size = freeblk -> header & BLK_SIZE_MASKING_BIT;
    freeblk -> header ^= MAGIC;

    int index = ((size - MIN_BLOCK_SIZE)/ALIGN_SIZE);
    if (sf_quick_lists[index].length == 5){
        sf_block *current = sf_quick_lists[index].first;
        while(current){
            current = sf_quick_lists[index].first;
            sf_block *next  = sf_quick_lists[index].first -> body.links.next;
            sf_quick_lists[index].first = sf_quick_lists[index].first -> body.links.next;
            
            current -> header ^= MAGIC;
            current -> header &= ~THIS_BLOCK_ALLOCATED;
            current -> header &= ~IN_QUICK_LIST; 
            current -> header ^= MAGIC;

            uint64_t tempSize = ((current -> header) ^ MAGIC) & BLK_SIZE_MASKING_BIT;
            sf_block *nextBlk = (sf_block *)((void *)current + tempSize);
            nextBlk -> prev_footer = current -> header; 

            nextBlk -> header ^= MAGIC;
            nextBlk -> header &= ~PREV_BLOCK_ALLOCATED; 
            sf_size_t nextBlkSize = nextBlk -> header & BLK_SIZE_MASKING_BIT;
            nextBlk -> header ^= MAGIC;

            sf_footer *nextBlkFooter =  (sf_footer*)((void *)nextBlk + nextBlkSize);
            *nextBlkFooter = nextBlk -> header;

            sf_block* deletingBlk = coalescing(current);
            addFreeblkInFreelist(deletingBlk);
            current = next;
        }
        sf_quick_lists[index].length = 0;
    }
    sf_quick_lists[index].length += 1;
    freeblk -> body.links.next = sf_quick_lists[index].first;
    sf_quick_lists[index].first = freeblk;
}


int isValidAddress(void * addr){
    if ((addr == NULL) || (((uintptr_t)addr % 16)!= 0)){   
        debug("WRONG ADDRESS 1");
        return 0;
    }
    sf_header * header = (sf_header *) (addr - 8);
    sf_footer * footer = (sf_footer *) (addr - 16);

    sf_size_t currentBlkSize = (*header ^ MAGIC) & BLK_SIZE_MASKING_BIT;
    if (currentBlkSize < MIN_BLOCK_SIZE || currentBlkSize % 16){
        debug("WRONG ADDRESS 2");
        return 0;
    }

    if ((void *)header < (sf_mem_start() + MIN_BLOCK_SIZE + 8)){
        debug("WRONG ADDRESS 3");
        return 0;
    }

    if ((void *)footer >= (sf_mem_end() - 16)){ // maybe - 8? check needed
        debug("WRONG ADDRESS 4");
        return 0;
    }

    if (!((*header ^ MAGIC) & THIS_BLOCK_ALLOCATED)){
        debug("WRONG ADDRESS 5");
        return 0;
    }

    if (!((*header ^ MAGIC) &  PREV_BLOCK_ALLOCATED)){
        if ((*footer ^ MAGIC) &  THIS_BLOCK_ALLOCATED){
            debug("WRONG ADDRESS 6");
            return 0;
        }
        uint64_t prevBlkSize = (*footer ^ MAGIC) & BLK_SIZE_MASKING_BIT;
        sf_header *prevBlkHeader = (sf_header *)((void *)header - prevBlkSize);
        if ((*prevBlkHeader ^ MAGIC) & THIS_BLOCK_ALLOCATED){
            debug("WRONG ADDRESS 7");
            return 0;
        }
    }    
    return 1;
}