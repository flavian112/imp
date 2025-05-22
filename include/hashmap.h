#ifndef HASHMAP_H
#define HASHMAP_H


typedef struct HashMap *hashmap_t;

typedef struct HashMapKeysIter *hashmap_keys_iter_t;


hashmap_t hashmap_create(void);
void hashmap_free(hashmap_t map);

void **hashmap_get(hashmap_t map, const char *key);
void hashmap_insert(hashmap_t map, const char *key, void *value);
int hashmap_delete(hashmap_t map, const char *key);

hashmap_keys_iter_t hashmap_keys_iter_create(hashmap_t map);
const char *hashmap_keys_iter_next(hashmap_keys_iter_t iter);
void hashmap_keys_iter_free(hashmap_keys_iter_t iter);


#endif