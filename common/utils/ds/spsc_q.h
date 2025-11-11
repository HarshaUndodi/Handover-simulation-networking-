/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef SPSC_Q_H_
#define SPSC_Q_H_

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct spsc_q {
  uint8_t *buf;
  size_t elsiz;
  size_t cnt;
  size_t write_idx;
  size_t read_idx;
  pthread_mutex_t mtx;
} spsc_q_t;

spsc_q_t spsc_q_alloc(size_t cnt, size_t elsiz);
void spsc_q_free(spsc_q_t *rb);

bool spsc_q_put(spsc_q_t *rb, const void *src, size_t elsiz);
bool spsc_q_get(spsc_q_t *rb, void *dest, size_t elsiz);

typedef bool (*pred)(const void *data, void *user);
bool spsc_q_get_if(spsc_q_t *rb, pred p, void *user, void *dest, size_t elsiz);
int spsc_q_drop_while(spsc_q_t *rb, pred p, void *user);
int spsc_q_get_while(spsc_q_t *rb, pred p, void *user, void *dest, size_t elsiz, size_t max_len);

#endif /* SPSC_Q_H_ */
