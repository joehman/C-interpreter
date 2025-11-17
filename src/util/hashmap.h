#ifndef HashMap_h 
#define HashMap_h 

#include <stdlib.h> // for size_t

struct HashMapEntry {
    const char* key;
    void* value;

    struct HashMapEntry* next;
};

struct HashMap {
    struct HashMapEntry **buckets;

    size_t size;
    size_t count;    
};


struct HashMap* hashmap_create(size_t initialSize);
void hashmap_destroy(struct HashMap* map, void (*free_value)(void*));

void hashmap_put(struct HashMap* map, const char* key, void* value);
void hashmap_remove(struct HashMap* map, const char* key);

//@returns the value of the key-value pair
void* hashmap_get(struct HashMap* map, const char* key);

#endif // HASHMAP_H
