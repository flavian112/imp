#ifndef HASHMAP_H
#define HASHMAP_H


typedef struct HashMap *hashmap_t;
typedef struct HashMapKeys *hashmap_keys_t;


hashmap_t hashmap_create(void);
void hashmap_free(hashmap_t map);

void **hashmap_get(hashmap_t map, const char *key);
void hashmap_insert(hashmap_t map, const char *key, void *value);
int hashmap_delete(hashmap_t map, const char *key);

void hashmap_iterate(hashmap_t map, void (*callback)(const char *key, void *value));
hashmap_keys_t hashmap_keys_create(hashmap_t map);
const char *hashmap_keys_next(hashmap_keys_t iter);
void hashmap_keys_free(hashmap_keys_t iter);



#endif