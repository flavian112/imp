#include "hash_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define INITIAL_SIZE (16)
#define LOAD_FACTOR_THRESHOLD (0.75)

typedef struct Pair {
  char *key;
  int value;
  struct Pair *next;
} Pair;

typedef struct HashMap {
  Pair **table;
  size_t size;
  size_t count;
} HashMap;

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

void hashmap_insert(HashMap *map, const char *key, int value) {
  int *old_value = hashmap_get(map, key);
  if (old_value) {
    *old_value = value;
    return;
  }
  if ((float)map->count / map->size > LOAD_FACTOR_THRESHOLD) resize(map);
  unsigned int index = hash(key, map->size);
  Pair *new_pair = (Pair*)malloc(sizeof(Pair));
  assert(new_pair);
  new_pair->key = strdup(key);
  assert(new_pair->key);
  new_pair->value = value;
  new_pair->next = map->table[index];
  map->table[index] = new_pair;
  map->count++;
}

int *hashmap_get(HashMap *map, const char *key) {
  unsigned int index = hash(key, map->size);
  Pair *pair = map->table[index];
  while (pair) {
    if (strcmp(pair->key, key) == 0) return &pair->value;
    pair = pair->next;
  }
  return NULL;
}

void hashmap_delete(HashMap *map, const char *key) {
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
      return;
    }
    prev = pair;
    pair = pair->next;
  }
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

void hashmap_iterate(HashMap *map, void (*callback)(const char *key, int value)) {
  for (size_t i = 0; i < map->size; ++i) {
    Pair *pair = map->table[i];
    while (pair) {
      callback(pair->key, pair->value);
      pair = pair->next;
    }
  }
}