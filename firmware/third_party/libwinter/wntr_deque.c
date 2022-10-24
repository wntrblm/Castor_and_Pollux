#include "wntr_deque.h"
#include <string.h>

void WntrDeque_init(struct WntrDeque* q) {
    q->_read_offset = 0;
    q->_write_offset = 0;
    q->_count = 0;
}

void WntrDeque_push(volatile struct WntrDeque* q, uint8_t* i) {
    bool overflow = q->_count == q->capacity;

    if (overflow) {
        // Advance the read head one to drop the oldest item.
        q->_read_offset = (q->_read_offset + 1) % q->capacity;
        // This will be incremented again below, keeping the count the same.
        q->_count--;
    }

    uint8_t* write_ptr = q->data + (q->item_size * q->_write_offset);

    memcpy(write_ptr, i, q->item_size);

    q->_write_offset = (q->_write_offset + 1) % q->capacity;
    q->_count++;
}

void WntrDeque_pop(volatile struct WntrDeque* q, uint8_t* i) {
    uint8_t* read_ptr = q->data + (q->item_size * q->_read_offset);

    memcpy(i, read_ptr, q->item_size);

    q->_read_offset = (q->_read_offset + 1) % q->capacity;
    q->_count--;
}

uint8_t* WntrDeque_pop_zerocopy(volatile struct WntrDeque* q) {
    uint8_t* read_ptr = q->data + (q->item_size * q->_read_offset);
    q->_read_offset = (q->_read_offset + 1) % q->capacity;
    q->_count--;
    return read_ptr;
}

void WntrDeque_empty(volatile struct WntrDeque* q) {
    q->_write_offset = 0;
    q->_read_offset = 0;
    q->_count = 0;
}

void WntrDequeIter_init(struct WntrDequeIter* iter) { iter->_idx = iter->q->_read_offset; }

bool WntrDequeIter_next(struct WntrDequeIter* iter, void* i) {
    if (WntrDeque_is_empty(iter->q)) {
        return false;
    }
    if (iter->_idx == iter->q->_write_offset && iter->n > 0) {
        return false;
    }

    uint8_t* read_ptr = iter->q->data + (iter->q->item_size * iter->_idx);

    memcpy(i, read_ptr, iter->q->item_size);

    iter->_idx = (iter->_idx + 1) % iter->q->capacity;
    iter->n++;

    return true;
}
