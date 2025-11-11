/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "spsc_q.h"

#define rb_lock(LOCK) if (pthread_mutex_lock(LOCK) != 0) abort()
#define rb_unlock(LOCK) if (pthread_mutex_unlock(LOCK) != 0) abort()

spsc_q_t spsc_q_alloc(size_t cnt, size_t elsiz)
{
  /* internally, use one element more: the ringbuffer is full if
   * (write_idx + 1) % cnt == read_idx, so it's full if one element is still
   * free (because otherwise write_idx == read_idx, but this is the empty
   * condition) */
  cnt += 1;
  spsc_q_t rb = {.cnt = cnt, .elsiz = elsiz};
  rb.buf = calloc(cnt, elsiz);
  pthread_mutex_init(&rb.mtx, NULL);
  return rb;
}

void spsc_q_free(spsc_q_t *rb)
{
  free(rb->buf);
  pthread_mutex_destroy(&rb->mtx);
  memset(rb, 0, sizeof(*rb));
}

bool spsc_q_put(spsc_q_t *rb, const void *src, size_t elsiz)
{
  assert(elsiz == rb->elsiz);
  rb_lock(&rb->mtx);
  if ((rb->write_idx + 1) % rb->cnt == rb->read_idx) {
    rb_unlock(&rb->mtx);
    return false;
  }

  uint8_t *bufpos = &rb->buf[rb->write_idx * rb->elsiz];
  memcpy(bufpos, src, elsiz);
  rb->write_idx = (rb->write_idx + 1) % rb->cnt;
  rb_unlock(&rb->mtx);
  return true;
}

bool spsc_q_get_if(spsc_q_t *rb, pred p, void *user, void *dest, size_t elsiz)
{
  assert(elsiz == rb->elsiz);
  rb_lock(&rb->mtx);
  if (rb->write_idx == rb->read_idx) {
    rb_unlock(&rb->mtx);
    return false;
  }

  uint8_t *bufpos = &rb->buf[rb->read_idx * rb->elsiz];
  if (!p(bufpos, user)) {
    rb_unlock(&rb->mtx);
    return false;
  }

  memcpy(dest, bufpos, elsiz);
  rb->read_idx = (rb->read_idx + 1) % rb->cnt;
  rb_unlock(&rb->mtx);
  return true;
}

static bool always(const void *data, void *user)
{
  (void) data;
  (void) user;
  return true;
}
bool spsc_q_get(spsc_q_t *rb, void *dest, size_t elsiz)
{
  return spsc_q_get_if(rb, always, NULL, dest, elsiz);
}

int spsc_q_get_while(spsc_q_t *rb, pred p, void *user, void *dest, size_t elsiz, size_t max_len)
{
  assert(elsiz == rb->elsiz);
  rb_lock(&rb->mtx);
  size_t count = 0;
  while (rb->write_idx != rb->read_idx && count < max_len) {
    uint8_t *bufpos = &rb->buf[rb->read_idx * rb->elsiz];
    if (!p(bufpos, user))
      break;
    memcpy(dest + count * elsiz, bufpos, elsiz);
    count++;
    rb->read_idx = (rb->read_idx + 1) % rb->cnt;
  }
  rb_unlock(&rb->mtx);
  return count;
}

int spsc_q_drop_while(spsc_q_t *rb, pred p, void *user)
{
  rb_lock(&rb->mtx);
  size_t dropped = 0;
  while (rb->write_idx != rb->read_idx) {
    uint8_t *bufpos = &rb->buf[rb->read_idx * rb->elsiz];
    if (!p(bufpos, user))
      break;
    dropped++;
    rb->read_idx = (rb->read_idx + 1) % rb->cnt;
  }
  rb_unlock(&rb->mtx);
  return dropped;
}
