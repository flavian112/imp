#ifndef HASH_MAP_H
#define HASH_MAP_H

typedef struct HashMap *hashmap_t;

hashmap_t hashmap_create(void);
void hashmap_insert(hashmap_t map, const char *key, int value);
int *hashmap_get(hashmap_t map, const char *key);
void hashmap_delete(hashmap_t map, const char *key);
void hashmap_free(hashmap_t map);
void hashmap_iterate(hashmap_t map, void (*callback)(const char *key, int value));

#endif