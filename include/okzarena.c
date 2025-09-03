#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "okzarena.h"

OKZArena* arena = NULL;

int okz_init() {
  arena = malloc(sizeof(OKZArena));
  if (arena == NULL) return 1;
  arena->head = arena->tail = NULL;

  return 0;
}

int okz_push_copy(const char* key, const void* data, size_t size) {
  if (key[0] == '\0' || !key || (!data && size)) return 1;

  // arena not initialized
  if (arena == NULL) return 1;

  OKZNode* node = malloc(sizeof(OKZNode));
  if (node == NULL) return 1;

  node->key = strdup(key);
  if (node->key == NULL) {
    free(node);
    return 1;
  }

  node->prev = NULL;
  node->next = NULL;

  node->data = malloc(size);
  if (node->data == NULL) { 
    free(node->key);
    free(node);
    return 1;
  }
  memcpy(node->data, data, size);


  if (arena->head == NULL && arena->tail == NULL) {
    arena->head = node;
    arena->tail = node;
    return 0;
  }

  node->prev = arena->tail;
  arena->tail->next = node;
  arena->tail = node;

  return 0;
}

void* okz_get_ptr(const char* key) {
  if (arena == NULL) return NULL;

  OKZNode* current = arena->head;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) return current->data;
    current = current->next;
  }

  return NULL;
}

int okz_release(const char* key) {
  if (arena == NULL) return 1;

  OKZNode* current = arena->head;

  while (current != NULL) {
    if (strcmp(current->key, key) == 0) break;
    current = current->next;
  }

  if (current == NULL) return 1;

  if (current->prev) {
    current->prev->next = current->next;
  } else {
    arena->head = current->next;
  }

  if (current->next) {
    current->next->prev = current->prev;
  } else {
    arena->tail = current->prev;
  }


  free(current->key);
  free(current->data);
  free(current);

  return 0;
}

void okz_destroy() {
  if (!arena) return;

  OKZNode* current = arena->head;
  while (current != NULL) {
    OKZNode* next = current->next;
    free(current->key);
    free(current->data);
    free(current);
    current = next;
  }

  free(arena);
  arena = NULL;
}

void debug_arena(void) {
  OKZNode* current = arena->head;

  while (current != NULL) {
    printf("%s", current->key);

    if (current->next != NULL) {
      printf("<->");
    }

    current = current->next;
  }

  printf("\n");
}
