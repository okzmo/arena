#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "okzarena.h"

#define INITIAL_ARENA_SIZE (64 * 1024) // 64KB
#define INITIAL_TABLE_SIZE 1
#define ALIGNMENT 8

static OKZArena* arena = NULL;

static size_t hash_string(const char* str) {
  size_t hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

static size_t align_size(size_t size) {
  return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

int okz_init() {
  arena = malloc(sizeof(OKZArena));
  if (arena == NULL) return 1;

  arena->memory = malloc(INITIAL_ARENA_SIZE);
  if (arena->memory == NULL) {
    free(arena);
    return 1;
  }

  arena->size = INITIAL_ARENA_SIZE;
  arena->used = 0;

  arena->table_size = INITIAL_TABLE_SIZE;
  arena->table = calloc(INITIAL_TABLE_SIZE, sizeof(OKZEntry*));
  if (arena->table == NULL) {
    free(arena->memory);
    free(arena);
    return 1;
  }

  return 0;
}

int okz_push_copy(const char* key, const void* data, size_t size) {
  if (!key || key[0] == '\0') return 1;
  if (data == NULL || size <= 0) return 1;

  size_t aligned_size = align_size(size);
  
  if (arena->used + aligned_size > arena->size) {
    size_t new_size = arena->size * 2;
    while (new_size < arena->used + aligned_size) {
      new_size *=2;
    }

    char* new_memory = realloc(arena->memory, new_size);
    if (new_memory == NULL) {
      return 1;
    }

    arena->memory = new_memory;
    arena->size = new_size;
  }

  OKZEntry* entry = malloc(sizeof(OKZEntry));
  if (entry == NULL) return 1;

  entry->key = strdup(key);
  if (!entry->key) {
    free(entry);
    return 1;
  }

  entry->offset = arena->used;
  entry->size = size;
  memcpy(arena->memory + arena->used, data, size);
  entry->deleted = 0;

  arena->used += aligned_size;

  size_t bucket = hash_string(key) % arena->table_size;
  entry->next = arena->table[bucket];
  arena->table[bucket] = entry;

  return 0;
}

void* okz_get_ptr(const char* key) {
  if (!arena || !key || key[0] == '\0') return NULL;

  size_t bucket = hash_string(key) % arena->table_size;
  OKZEntry* entry = arena->table[bucket];

  while (entry) {
    if (!entry->deleted && strcmp(entry->key, key) == 0) {
      return arena->memory + entry->offset;
    }
    entry = entry->next;
  }

  return NULL;
}

int okz_release(const char* key) {
  if (!arena || !key || key[0] == '\0') return 1;

  size_t bucket = hash_string(key) % arena->table_size;
  OKZEntry* entry = arena->table[bucket];

  while (entry) {
    if (!entry->deleted && strcmp(entry->key, key) == 0) {
      entry->deleted = 1;
      free(entry->key);
      entry->key = NULL;
      return 0;
    }
    entry = entry->next;
  }

  return 1;
}

void okz_destroy() {
  if (!arena) return;

  for (size_t i = 0; i < arena->table_size; ++i) {
    OKZEntry* entry = arena->table[i];

    while (entry) {
      OKZEntry* next = entry->next;
      free(entry->key);
      free(entry);
      entry = next;
    }
  }


  free(arena->table);
  free(arena->memory);
  free(arena);
  arena = NULL;
}

void debug_arena(void) {
  printf("Arena memory: %p\n", arena->memory);
  printf("Arena size: %zu\n", arena->size);
  printf("Arena used: %zu\n", arena->used);
  printf("Arena table size: %zu\n", arena->table_size);
  printf("Arena table:\n");
  for (size_t i = 0; i < arena->table_size; ++i) {
    printf("  Bucket %zu:\n", i);
    OKZEntry* entry = arena->table[i];
    while (entry) {
      printf("    Key: %s, Offset: %zu, Size: %zu, Deleted: %d\n", entry->key, entry->offset, entry->size, entry->deleted);
      entry = entry->next;
    }
  }
  printf("\n");
}
