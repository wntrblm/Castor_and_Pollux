#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct WntrDeque {
  // The underlying data
  uint8_t *data;
  // The size of one item.
  size_t item_size;
  // Total capacity. Should be in number of *items*, not number of *bytes*.
  size_t capacity;
  // The count of stored items.
  size_t _count;
  // Internal data.
  size_t _read_offset;
  size_t _write_offset;
};

struct WntrDequeIter {
  struct WntrDeque* q;
  size_t n;
  size_t _idx;
};

void WntrDeque_init(struct WntrDeque *q);
void WntrDeque_push(volatile struct WntrDeque *q, uint8_t *i);
void WntrDeque_pop(volatile struct WntrDeque *q, uint8_t *i);
uint8_t *WntrDeque_pop_zerocopy(volatile struct WntrDeque *q);
void WntrDeque_empty(volatile struct WntrDeque *q);

inline static bool WntrDeque_is_empty(volatile struct WntrDeque *q) {
  return q->_count == 0;
}

inline static bool WntrDeque_is_full(volatile struct WntrDeque *q) {
  return q->_count == q->capacity;
}

void WntrDequeIter_init(struct WntrDequeIter* iter);
bool WntrDequeIter_next(struct WntrDequeIter* iter, void* i);
