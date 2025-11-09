#ifndef mtDebug_h
#define mtDebug_h

#include <stdlib.h>

#define mtDebug // temporary

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
     (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))

//defines
#ifdef mtDebug
    #define malloc(size) mtMalloc(size, __FILENAME__, __func__, __LINE__)
    #define free mtFree
#endif


//Declarations
#ifdef mtDebug
    void* mtMalloc(size_t size, const char* file, const char* function, const int line);
    void mtFree(void* ptr);
    void mtCheckMemory();
#endif


#endif 
