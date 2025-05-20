#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash_map.h"

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
  while (*key)hash_value = (hash_value * 31) + *key++;
  return hash_value % size;
}

hashmap_t hashmap_create(void) {
  HashMap *map = (HashMap *)malloc(sizeof(HashMap));
  assert(map);
  map->size = INITIAL_SIZE;
  map->count = 0;
  map->table = (Pair **)calloc(map->size, sizeof(Pair *));
  assert(map->table);
  return map;
}

static void resize(hashmap_t map) {
  size_t new_size = ((HashMap*)map)->size * 2;
  Pair **new_table = (Pair **)calloc(new_size, sizeof(Pair *));
  assert(new_table);
  for (size_t i = 0; i < ((HashMap*)map)->size; ++i) {
    Pair *pair = ((HashMap*)map)->table[i];
    while (pair) {
      unsigned int index = hash(pair->key, new_size);
      Pair *next = pair->next;
      pair->next = new_table[index];
      new_table[index] = pair;
      pair = next;
    }
  }
  free(((HashMap*)map)->table);
  ((HashMap*)map)->table = new_table;
  ((HashMap*)map)->size = new_size;
}

void hashmap_insert(hashmap_t map, const char *key, int value) {
  if ((float)((HashMap*)map)->count / ((HashMap*)map)->size > LOAD_FACTOR_THRESHOLD) resize(((HashMap*)map));
  unsigned int index = hash(key, ((HashMap*)map)->size);
  Pair *new_pair = (Pair *)malloc(sizeof(Pair));
  assert(new_pair);
  new_pair->key = strdup(key);
  assert(new_pair->key);
  new_pair->value = value;
  new_pair->next = ((HashMap*)map)->table[index];
  ((HashMap*)map)->table[index] = new_pair;
  ((HashMap*)map)->count++;
}

int *hashmap_get(hashmap_t map, const char *key) {
  unsigned int index = hash(key, ((HashMap*)map)->size);
  Pair *pair = ((HashMap*)map)->table[index];
  while (pair) {
    if (strcmp(pair->key, key) == 0) return &pair->value;
    pair = pair->next;
  }
  return NULL;
}

void hashmap_delete(hashmap_t map, const char *key) {
  unsigned int index = hash(key, ((HashMap*)map)->size);
  Pair *pair = ((HashMap*)map)->table[index];
  Pair *prev = NULL;
  while (pair) {
    if (strcmp(pair->key, key) == 0) {
      if (prev) prev->next = pair->next;
      else ((HashMap*)map)->table[index] = pair->next;
      free(pair->key);
      free(pair);
      ((HashMap*)map)->count--;
      return;
    }
    prev = pair;
    pair = pair->next;
  }
}

void hashmap_free(hashmap_t map) {
  for (size_t i = 0; i < ((HashMap*)map)->size; ++i) {
    Pair *pair = ((HashMap*)map)->table[i];
    while (pair){
      Pair *temp = pair;
      pair = pair->next;
      free(temp->key);
      free(temp);
    }
  }
  free(((HashMap*)map)->table);
  free(((HashMap*)map));
}