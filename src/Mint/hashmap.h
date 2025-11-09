#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h> // for size_t

struct HashMapEntry {
    const char* key;
    void* value;

    struct HashMapEntry* next;
};

typedef struct HashMap {
    struct HashMapEntry **buckets;

    size_t size;
    size_t count;    
} HashMap;


HashMap* hashmap_create(size_t initialSize);
void hashmap_destroy(HashMap* map, void (*free_value)(void*));

void hashmap_put(HashMap* map, const char* key, void* value);
void hashmap_remove(HashMap* map, const char* key);
void* hashmap_get(HashMap* map, const char* key);


#endif // HASHMAP_H
