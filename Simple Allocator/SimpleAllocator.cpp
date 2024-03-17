/**
 * @file SimpleAllocator.cpp
 * @author Yugendren S/O Sooriya Moorthi
 * @brief SimpleAllocator class definition
 *        This class is used to allocate and free memory blocks
 *        for both pages and blocks according to use case
 * @date 14 Sept 2023
 */
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "SimpleAllocator.h"

void SimpleAllocator::corruptionCheck(Node* blockStart)
{
    //start of block
    unsigned char* corruptCheck = reinterpret_cast<unsigned char*>(blockStart);
    // Checking corruption before the block
    if (config_.padBytesSize > 0) {
        for (unsigned int i = 0; i < config_.padBytesSize; i++) 
        {
            // Calculate the address of the padding byte before the block
            unsigned char* paddingByteBefore = corruptCheck - config_.padBytesSize + i;
            
            // Check for corruption
            if (*paddingByteBefore != PAD_PATTERN) {
                throw SimpleAllocatorException(
                    SimpleAllocatorException::E_CORRUPTED_BLOCK,
                    "ERROR when checking pad bytes: memory corrupted before block."
                );
            }
        }

    // Checking corruption after the block
    for (unsigned int i = 0; i < config_.padBytesSize; i++) 
    {
        // Calculate the address of the padding byte after the block
        unsigned char* paddingByteAfter = corruptCheck + stats_.objectSize + i;

        // Check for corruption
        if (*paddingByteAfter != PAD_PATTERN) {
            throw SimpleAllocatorException(
                SimpleAllocatorException::E_CORRUPTED_BLOCK,
                "ERROR when checking pad bytes: memory corrupted after block."
            );
        }
    }
}
}


SimpleAllocator::SimpleAllocator(size_t objectSize, const SimpleAllocatorConfig& config) : config_(config) 
{
// Initialize statistics
stats_.objectSize = objectSize;
stats_.blockSize = objectSize + (config.padBytesSize *2) + config.headerBlockInfo.size;
//8 represents the size of the pointer to the next page
stats_.pageSize = 8 + config.leftAlignBytesSize + (stats_.blockSize * config.objectsPerPage) + (config.interAlignBytesSize*(config.objectsPerPage-1)) + (config_.padBytesSize*config_.maxPages);//+ config.headerBlockInfo.size+ config.padBytesSize;//might need to add header and pad again
//initalize all to 0
stats_.allocations = 0;
stats_.deallocations = 0;
stats_.objectsInUse = 0;
stats_.pagesInUse = 0;
stats_.freeObjects = 0;
stats_.mostObjects = 0;

// Initialize free and page lists
pFreeList_ = nullptr;
pPageList_ = nullptr;

// Allocate the first page
allocateNewPage();
}

SimpleAllocator::~SimpleAllocator() 
{
    // Release all allocated pages

    while (pPageList_ != nullptr) //for each page
    {   
        if (config_.headerBlockInfo.type == config_.EXTERNAL_HEADER)//if external header
        {
            //find start of page
            char* startPage = reinterpret_cast<char*>(pPageList_);
            size_t incr = config_.interAlignBytesSize+config_.headerBlockInfo.size+config_.padBytesSize + sizeof(char*);//find the increment from the start of the page
            char* currentBlock = startPage + incr;//find the position of the first block
            //loop every object on the page
            for(unsigned int i = 0; i < config_.objectsPerPage; i++) 
            {
                //find the header location
                char* header = reinterpret_cast<char*>(currentBlock) - config_.padBytesSize - config_.headerBlockInfo.size;
                //find the info location
                MemBlockInfo** temp = reinterpret_cast<MemBlockInfo**>(header);
                MemBlockInfo* info = *temp;
                //check if the MemBlockInfo still exists for that block
                if(info!=nullptr)//for the blocks that have not been unallocated, the label needs to be dreed
                {
                    if (config_.useCPPMemManager) //delete the label
                    {
                        delete info->pLabel;
                        delete info;                         
                    } 
                    else//free the label
                    {
                        std::free(info->pLabel);
                        std::free(info);
                    }
                
                }
                //Move to the next block in the free list
                currentBlock+=stats_.blockSize;
            }
        }
        // store the next page in the page list
        Node* currentPageNode = pPageList_;
        pPageList_ = pPageList_->pNext;

        //delete or free the current page
        if (config_.useCPPMemManager)
        {
            delete[] reinterpret_cast<char*>(currentPageNode);
        } else 
        {
            std::free(reinterpret_cast<char*>(currentPageNode));
        }
    }
}



void* SimpleAllocator::allocate(const char* pLabel) 
{
    // Check if there are any free blocks available
    if (pFreeList_ == nullptr) 
    {
        // Allocate a new page if there are no free blocks
        allocateNewPage();
    }
   
  // Remove a block from the free list
    Node* allocatedBlock = pFreeList_;
    //check for corruption
    corruptionCheck(allocatedBlock);
    //link freelist to the next block
    pFreeList_ = pFreeList_->pNext;

    // Update allocation statistics
    stats_.allocations++;
    stats_.objectsInUse++;
    stats_.freeObjects--;
    char* header = reinterpret_cast<char*>(allocatedBlock) - config_.padBytesSize - config_.headerBlockInfo.size;
    // Assuming 'allocatedBlock' points to the start of the memory block
    unsigned char* blockPtr = reinterpret_cast<unsigned char*>(allocatedBlock);
    // find the first instance of the PATTERN
    unsigned char value = blockPtr[8];

    // set block to allocated pattern   
    memset(allocatedBlock, ALLOCATED_PATTERN, stats_.objectSize);
    // extended header, with mem layout:
    // | user-defined | use count        | alloc num      | flag   |
    // | char *       | unsigned short * | unsigned int * | bool * |
    if(config_.headerBlockInfo.type == config_.EXTENDED_HEADER)
    {  
        (*header)++;//increment header with each allocation
        unsigned short* useCount = reinterpret_cast<unsigned short*>(header + 2); 
        *useCount =stats_.allocations;//total no of allocations
    }
    // external header, with mem layout:
    // | MemBlockInfo*  | (it stores a pointer to a MemBlockInfo struct)
    // | MemBlockInfo** | (so the header ptr is a pointer to a pointer)
    else if(config_.headerBlockInfo.type == config_.EXTERNAL_HEADER)
    {
        // Use new or malloc to allocate a new page of memory
        //use char* for byte level memory control
        MemBlockInfo* info = nullptr;
        MemBlockInfo** temp = reinterpret_cast<MemBlockInfo**>(header);
        if (value == UNALLOCATED_PATTERN)//only new for the first allocation of the block
        {
            //only new for the first time
            if (config_.useCPPMemManager) //if true use new
            {
                info = new MemBlockInfo;//makes a new page using char* and the size of the page(how many bytes)
            } else //if false use malloc
            {
                info = static_cast<MemBlockInfo*>(malloc(sizeof(MemBlockInfo)));
            }
            
            *temp = info; //store the pointer to the new location
        } 
        else 
        {
            info = *temp;//if not the first time, get the info from the header
        }
         info->inUse = true;//store inUse value
         info->allocNum = stats_.allocations;//store number of allocations
         //info->pLabel = pLabel?strdup(pLabel):nullptr; //copy the label and store
         if (config_.useCPPMemManager) //if true use new
        {
            info->pLabel = new char[strlen(pLabel) + 1];;//makes a new label
        } else //if false use malloc
        {
            info->pLabel = static_cast<char*>(malloc(strlen(pLabel) + 1));//makes a new label
        }
         
         std::strcpy(info->pLabel, pLabel);//stores the label

        return reinterpret_cast<Node*>(allocatedBlock); //return immediately if external header
    }
    //for all other cases
    else
    {
        //store allocation value at the header
        *header = stats_.allocations;
    }
    //select flag location
    char* flag = reinterpret_cast<char*>(allocatedBlock) -config_.padBytesSize - 1;
    // Set the flag to 0x01
    *flag = 0x01;
    // Return a pointer to the allocated block (you might need to do some pointer arithmetic)
    return reinterpret_cast<Node*>(allocatedBlock);
}


void SimpleAllocator::free(void* pObj) 
{
    //exception handling
    if (pObj == nullptr) {
        // Do nothing if pObj is nullptr
        throw SimpleAllocatorException(
            SimpleAllocatorException::E_BAD_BOUNDARY,
            "Error during free: not on a block boundary in page."
        );
    }
    //exception handling
    if(stats_.objectsInUse==0)
    {
        throw SimpleAllocatorException(
            SimpleAllocatorException::E_BAD_BOUNDARY,
            "Error during free: not on a block boundary in page."
        );
    }
    
    //check for corruption
    corruptionCheck(reinterpret_cast<Node*>(pObj));
    //find the start of the block
    char* blockStart = reinterpret_cast<char*>(pObj);
    //set the pattern for the block
    memset(blockStart, FREED_PATTERN, stats_.objectSize);
    //link the block to the free list
    Node* freeBlock = reinterpret_cast<Node*>(blockStart);
    freeBlock->pNext = pFreeList_;
    pFreeList_ = freeBlock;
    //set flag back to nullptr
    char* flag = nullptr;
    //depending on pad size, set the flag location
    if(config_.padBytesSize>0)//if padding exists
    {
        flag = reinterpret_cast<char*>(pObj) - 1 - config_.padBytesSize;
    }
    else//no padding
    {
        flag = reinterpret_cast<char*>(pObj) - 1;
    }
    //store stats
    stats_.deallocations++;
    stats_.objectsInUse--;
    stats_.freeObjects++;
    //find header location
    char* header = reinterpret_cast<char*>(freeBlock) - config_.padBytesSize - config_.headerBlockInfo.size; 
    // extended header, with mem layout:
    // | user-defined | use count        | alloc num      | flag   |
    // | char *       | unsigned short * | unsigned int * | bool * |
    if(config_.headerBlockInfo.type == config_.EXTENDED_HEADER)
    {  
        //set all to 0
        unsigned short* useCount = reinterpret_cast<unsigned short*>(header + 2); 
        *useCount = 0;
        *flag = 00;

    }
    // external header, with mem layout:
    // | MemBlockInfo*  | (it stores a pointer to a MemBlockInfo struct)
    // | MemBlockInfo** | (so the header ptr is a pointer to a pointer)
    else if(config_.headerBlockInfo.type == config_.EXTERNAL_HEADER)
    {
        MemBlockInfo** temp = reinterpret_cast<MemBlockInfo**>(header); //find location of double pointer
        MemBlockInfo* info = *temp;//get the info from the double pouinter
        info->inUse = false;//store to false
        //based of specified memory management, delete or free the label
        if (config_.useCPPMemManager) //if true use new
        {
            delete info->pLabel;//delete the label for every tune free
        } else //if false use malloc
        {
           std::free(info->pLabel);//delete the label for every tune free
        }
        info->pLabel = nullptr;//store to null
        //delete info->pLabel;//delete the label
        
    }
    //for all else
    else
    {
        //set to 0s
        *header  = 00;
        *flag = 00;
    }   
}

void SimpleAllocator::allocateNewPage() 
{
    // Check if the maximum number of pages has been reached
    //exception handling
    if (stats_.pagesInUse >= config_.maxPages) 
    {
        throw SimpleAllocatorException(
            SimpleAllocatorException::E_NO_PAGE,
            "ERROR when allocating new page: maximum number of pages has been allocated."
        );
    }

    // Use new or malloc to allocate a new page of memory
    //use char* for byte level memory control
    char* newPage = nullptr;
    if (config_.useCPPMemManager) //if true use new
    {
        newPage = new char[stats_.pageSize];//makes a new page using char* and the size of the page(how many bytes)
    } else //if false use malloc
    {
        newPage = static_cast<char*>(malloc(stats_.pageSize));
    }

    // Check if memory allocation failed
    //exception handling
    if (newPage == nullptr) 
    {
        throw SimpleAllocatorException(
            SimpleAllocatorException::E_NO_MEMORY,
            "Memory allocation for a new page failed."
        );
    }
    char** storePage = reinterpret_cast<char**>(newPage);
    *storePage = newPage;
    //link new page in pagelist
    Node* nextPage = reinterpret_cast<Node*>(newPage);
    nextPage->pNext = pPageList_;
    pPageList_ = nextPage;

    // Set the first block as the new free list
    size_t incr = config_.interAlignBytesSize+config_.headerBlockInfo.size+config_.padBytesSize + sizeof(char*);//find the increment from the start of the page
    char* currentBlock = newPage + incr;//find the position of the first block
    Node* previous = nullptr; //set to null
    Node* current = nullptr; //set to null
    //memset memory pattern to unallocated, and set 
    for(size_t i = 0; i < config_.objectsPerPage; i++, currentBlock+=stats_.blockSize)
    {
        //if padding exists, set the padding pattern
        if(config_.padBytesSize > 0)
        {
            //for padding before block
            memset(currentBlock-config_.padBytesSize, PAD_PATTERN, config_.padBytesSize);
            //for padding after block
            memset(currentBlock+stats_.objectSize, PAD_PATTERN, config_.padBytesSize);
        }
        //set the block to unallocated
        memset(currentBlock, UNALLOCATED_PATTERN, stats_.objectSize);


        //simple singly linked list looping
        //set the inital current block
        current = reinterpret_cast<Node*>(currentBlock);
        //for the first instance on the block
        if(i == 0)  
        {
            current->pNext = pFreeList_;//will be null if first block of the first page
        }
        //for all other instances of the block
        else
        {
            current->pNext = previous;
        }
        //previous to store current
        previous = current;
    }
    //link the last block to the free list
    pFreeList_ = current;
    // Update allocation statistics
    stats_.pagesInUse++;
    stats_.freeObjects += config_.objectsPerPage; 
}

// Setters and getters
void SimpleAllocator::setDebug(bool _isDebug) 
{
    config_.isDebug = _isDebug;
}

const void* SimpleAllocator::getFreeList() const 
{
    return pFreeList_;
}

const void* SimpleAllocator::getPageList() const 
{
    return pPageList_;
}

SimpleAllocatorConfig SimpleAllocator::getConfig() const 
{
    return config_;
}

SimpleAllocatorStats SimpleAllocator::getStats() const 
{
    return stats_;
}