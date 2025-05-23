#include "hashmap.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define INITIAL_SIZE (16)
#define LOAD_FACTOR_THRESHOLD (0.75)


typedef struct Pair {
  char *key;
  void *element;
  struct Pair *next;
} Pair;

typedef struct HashMap {
  Pair **table;
  size_t size;
  size_t count;
} HashMap;

typedef struct HashMapKeysIter {
  HashMap *map;
  size_t bucket_index;
  Pair *current;
} HashMapKeysIter;


static unsigned int hash(const char *key, size_t size) {
  unsigned int hash_value = 0;
  while (*key) hash_value = (hash_value * 31) + *key++;
  return hash_value % size;
}

static void resize(HashMap *map) {
  size_t new_size = map->size * 2;
  Pair **new_table = (Pair**)calloc(new_size, sizeof(Pair*));
  assert(new_table);
  for (size_t i = 0; i < map->size; ++i) {
    Pair *pair = map->table[i];
    while (pair) {
      unsigned int index = hash(pair->key, new_size);
      Pair *next = pair->next;
      pair->next = new_table[index];
      new_table[index] = pair;
      pair = next;
    }
  }
  
  free(map->table);
  map->table = new_table;
  map->size = new_size;
}

HashMap *hashmap_create(void) {
  HashMap *map = (HashMap*)malloc(sizeof(HashMap));
  assert(map);
  map->size = INITIAL_SIZE;
  map->count = 0;
  map->table = (Pair**)calloc(map->size, sizeof(Pair*));
  assert(map->table);
  return map;
}

void hashmap_insert(HashMap *map, const char *key, void *element) {
  void **old_value = hashmap_get(map, key);
  if (old_value) {
    *old_value = element;
    return;
  }
  if ((float)map->count / map->size > LOAD_FACTOR_THRESHOLD) resize(map);
  unsigned int index = hash(key, map->size);
  Pair *new_pair = (Pair*)malloc(sizeof(Pair));
  assert(new_pair);
  new_pair->key = strdup(key);
  assert(new_pair->key);
  new_pair->element = element;
  new_pair->next = map->table[index];
  map->table[index] = new_pair;
  map->count++;
}

void **hashmap_get(HashMap *map, const char *key) {
  unsigned int index = hash(key, map->size);
  Pair *pair = map->table[index];
  while (pair) {
    if (strcmp(pair->key, key) == 0) return &pair->element;
    pair = pair->next;
  }
  return NULL;
}

int hashmap_delete(HashMap *map, const char *key) {
  unsigned int index = hash(key, map->size);
  Pair *pair = map->table[index];
  Pair *prev = NULL;
  while (pair) {
    if (strcmp(pair->key, key) == 0) {
      if (prev) prev->next = pair->next;
      else map->table[index] = pair->next;
      free(pair->key);
      free(pair);
      map->count--;
      return 0;
    }
    prev = pair;
    pair = pair->next;
  }
  return -1;
}

void hashmap_free(HashMap *map) {
  for (size_t i = 0; i < map->size; ++i) {
    Pair *pair = map->table[i];
    while (pair) {
      Pair *temp = pair;
      pair = pair->next;
      free(temp->key);
      free(temp);
    }
  }
  free(map->table);
  free(map);
}

HashMapKeysIter *hashmap_keys_iter_create(HashMap *map) {
  HashMapKeysIter *iter = (HashMapKeysIter*)malloc(sizeof(HashMapKeysIter));
  assert(iter);
  iter->map = map;
  iter->bucket_index = 0;
  iter->current = NULL;
  while (iter->bucket_index < map->size && map->table[iter->bucket_index] == NULL) {
    iter->bucket_index++;
  }
  if (iter->bucket_index < map->size) {
    iter->current = map->table[iter->bucket_index];
  }
  return iter;
}

const char *hashmap_keys_iter_next(HashMapKeysIter *iter) {
  if (!iter->current) return NULL;
  const char *key = iter->current->key;
  if (iter->current->next) {
    iter->current = iter->current->next;
  } else {
    iter->bucket_index++;
    while (iter->bucket_index < iter->map->size && iter->map->table[iter->bucket_index] == NULL) {
      iter->bucket_index++;
    }
    iter->current = (iter->bucket_index < iter->map->size) ? iter->map->table[iter->bucket_index] : NULL;
  }
  return key;
}

void hashmap_keys_iter_free(HashMapKeysIter *iter) {
  free(iter);
}