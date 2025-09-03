#ifndef OKZARENA_H
#define OKZARENA_H

typedef struct OKZNode {
  char* key;
  void* data;
  struct OKZNode* next;
  struct OKZNode* prev;
} OKZNode;

typedef struct OKZArena {
  OKZNode* head;
  OKZNode* tail;
} OKZArena;

int okz_init(void);
int okz_push_copy(const char* key, const void* data, size_t size);
void* okz_get_ptr(const char* key);
int okz_release(const char* key);
void okz_destroy(void);
void debug_arena(void); // print the arena;

#define okz_push(key, ...) okz_push_copy((key), &(__VA_ARGS__), sizeof(__VA_ARGS__))
#define okz_get(type, key) ((type *)okz_get_ptr((key)))

#endif
