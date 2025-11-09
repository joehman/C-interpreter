
#include "mtDebug.h"
#include <stdio.h>


struct AllocatedMemoryTracker {
    size_t size;

    const char* file;
    const char* function;
    int line;

    const void* ptr;
};


#define MAX_TRACKED_ALLOCATIONS 1024

int allocatedPointersIndex = 0;
int freedPointersIndex = 0;

static struct AllocatedMemoryTracker allocatedPointers[MAX_TRACKED_ALLOCATIONS];
static void* freedPointers[MAX_TRACKED_ALLOCATIONS];


#undef malloc
void* mtMalloc(size_t size, const char* file, const char* function, const int line) 
{
    void* ptr = malloc(size);

    allocatedPointers[allocatedPointersIndex] = (struct AllocatedMemoryTracker){
        .file = file,
        .function = function,
        .line = line,
        .ptr = ptr,
        .size = size
    };
    allocatedPointersIndex++;

    return ptr;
}

#undef free
void mtFree(void* ptr)
{
    freedPointers[freedPointersIndex] = ptr;
    freedPointersIndex++; 

    free(ptr);
}

void mtCheckMemory()
{
    printf("Checking for memory leaks!\n");

    int numberFound = 0;
    size_t sumSize = 0;


    for (size_t i = 0; i < MAX_TRACKED_ALLOCATIONS; i++)
    {
        int counter = 0;
        for (size_t j = 0; j < MAX_TRACKED_ALLOCATIONS; j++)
        {
            if (allocatedPointers[i].ptr == freedPointers[j])
            {
                counter++;
            }
        }

        if (counter <= 0)
        {
            printf("%zu unfreed bytes allocated in function: %s on line: %d in file: %s\n", 
                    allocatedPointers[i].size,
                    allocatedPointers[i].function,
                    allocatedPointers[i].line,
                    allocatedPointers[i].file);
            sumSize += allocatedPointers[i].size;
            numberFound++;
        }
    }
    
    if (numberFound > 0)
    {
        printf("Total leaked memory: %zu bytes\n", sumSize);
        printf("Total number of unfreed pointers: %d\n", numberFound);
    } else {
        printf("No memory leaked!\n");
    }   
}

__attribute__((constructor)) // gcc/clang only!
void mtInitializeMemoryTracker()
{
    atexit(mtCheckMemory);
}
