#ifndef HashMap_h 
#define HashMap_h 

#include <stdlib.h> // for size_t

struct mtHashMapEntry {
    const char* key;
    void* value;

    struct mtHashMapEntry* next;
};

struct mtHashMap {
    struct mtHashMapEntry **buckets;

    size_t size;
    size_t count;    
};


struct mtHashMap* mtHashMapCreate(size_t initialSize);
void mtHashMapDestroy(struct mtHashMap* map, void (*free_value)(void*));

void mtHashMapPut(struct mtHashMap* map, const char* key, void* value);
void mtHashMapRemove(struct mtHashMap* map, const char* key);

//@returns the value of the key-value pair
void* mtHashMapGet(struct mtHashMap* map, const char* key);

#endif // HASHMAP_H
