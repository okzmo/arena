#ifndef OKZARENA_H
#define OKZARENA_H

typedef struct OKZArena {
  char* memory;
  size_t size;
  size_t used;

  struct OKZEntry** table;
  size_t table_size;
} OKZArena;

typedef struct OKZEntry {
  char* key;
  size_t offset;
  size_t size;
  struct OKZEntry* next;
  int deleted;
} OKZEntry;


int okz_init(void);
int okz_push_copy(const char* key, const void* data, size_t size);
void* okz_get_ptr(const char* key);
int okz_release(const char* key);
void okz_destroy(void);
void debug_arena(void); // print the arena;

#define okz_push(key, ...) okz_push_copy((key), &(__VA_ARGS__), sizeof(__VA_ARGS__))
#define okz_get(type, key) ((type *)okz_get_ptr((key)))

#endif
