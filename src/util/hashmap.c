#include "hashmap.h"
#include <string.h>

//djb2 algorithm by Dan Bernstein
//http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

struct mtHashMapEntry* create_hashmap_entry(const char* key, void* value)
{
    struct mtHashMapEntry* entry = malloc(sizeof(struct mtHashMapEntry)); 

    entry->key = strdup(key);
    entry->value = value; 

    return entry;
}

// PUBLIC FUNCTIONS

struct mtHashMap* mtHashMapCreate(size_t initialSize)
{
    struct mtHashMap* map = malloc(sizeof(struct mtHashMap));
    
    map->buckets = calloc(initialSize, sizeof(struct mtHashMapEntry*));
    map->size = initialSize;
    map->count = 0;

    return map;
}

void mtHashMapDestroy(struct mtHashMap* map, void (*free_value)(void*))
{
    for (size_t i = 0; i < map->size; i++) {
        struct mtHashMapEntry *entry = map->buckets[i];
        while (entry) {
            struct mtHashMapEntry *tmp = entry;
            entry = entry->next;
            free((void*)tmp->key);
            if (free_value)
                free_value(tmp->value);
            free(tmp);
        }
    }
    free(map->buckets);
    free(map);
}

void mtHashMapPut(struct mtHashMap* map, const char* key, void* value)
{
    unsigned int index = hash((unsigned char*)key) % map->size;
    
    struct mtHashMapEntry* entry = map->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value; // overwrite pointer
            return;
        }
        entry = entry->next;
    }

    // Prepend new entry
    struct mtHashMapEntry *new_entry = create_hashmap_entry(key, value);
    new_entry->next = map->buckets[index];
    map->buckets[index] = new_entry;
    map->count++;
}

void mtHashMapRemove(struct mtHashMap* map, const char* key)
{
    unsigned int index = hash((unsigned char*)key) % map->size;

    struct mtHashMapEntry* entry;
    struct mtHashMapEntry* prev = NULL;

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev)
                prev->next = entry->next;
            else
                map->buckets[index] = entry->next;

            free((void*)entry->key);
            free(entry);
            map->count--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

void* mtHashMapGet(struct mtHashMap* map, const char* key)
{
    unsigned int index = hash((unsigned char*)key) % map->size;

    struct mtHashMapEntry*entry = map->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0)
            return entry->value;
        entry = entry->next;
    }
    return NULL;
}
