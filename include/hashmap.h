#ifndef HASHMAP_H
#define HASHMAP_H

/* Hashmap Handle */
typedef struct HashMap *hashmap_t;

/* Hashmap Key Iterator Handle */
typedef struct HashMapKeysIter *hashmap_keys_iter_t;


/* 
  Creates hashmap that maps char* to void*.
    Keys are copied, and will be freed when the hashmap is freed.
    Elements are not copied, and must be freed by the caller.
    Caller is responsible for freeing the hashmap using hashmap_free.
*/
hashmap_t hashmap_create(void);

/*
  Frees the hashmap and all keys.
    Elements are not freed, and must be freed by the caller.
*/
void hashmap_free(hashmap_t map);

/* 
  Looks up element with given key.
    Returns pointer to element, or NULL if not found.
*/
void **hashmap_get(hashmap_t map, const char *key);

/* 
  Inserts element with given key.
    If key already exists, the old element is replaced (caller is responsible for freeing beforehand).
*/
void hashmap_insert(hashmap_t map, const char *key, void *element);

/* 
  Deletes element with given key.
    Caller is responsible for freeing the element.
    Returns 0 on success, -1 when not found.
*/
int hashmap_delete(hashmap_t map, const char *key);


/* 
  Creates iterator for keys.
    Caller is responsible for freeing the iterator using hashmap_keys_iter_free.
    Iterator is invalid when the hashmap is modified and must be freed.
*/
hashmap_keys_iter_t hashmap_keys_iter_create(hashmap_t map);

/* 
  Returns next key in the iterator.
    Returns NULL when there are no more keys.
    Keys are not copied, and must not be freed.
*/
const char *hashmap_keys_iter_next(hashmap_keys_iter_t iter);

/* Frees the iterator. */
void hashmap_keys_iter_free(hashmap_keys_iter_t iter);


#endif