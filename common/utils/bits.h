/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef BITS_H_
#define BITS_H_

#include <stddef.h>
#include <stdint.h>
#include "assertions.h"

uint64_t reverse_bits(uint64_t in, int n_bits);
void reverse_bits_u8(uint8_t const* in, size_t sz, uint8_t* out);

static inline int count_bits(uint8_t *arr, int sz)
{
  AssertFatal(sz % sizeof(int) == 0, "to implement if needed\n");
  int ret = 0;
  for (uint *ptr = (uint *)arr; (uint8_t *)ptr < arr + sz; ptr++)
    ret += __builtin_popcount(*ptr);
  return ret;
}

static __attribute__((always_inline)) inline int count_bits64(uint64_t v)
{
  return __builtin_popcountll(v);
}

static __attribute__((always_inline)) inline int count_bits64_with_mask(uint64_t v, int start, int num)
{
  uint64_t mask = ((1LL << num) - 1) << start;
  return count_bits64(v & mask);
}
#endif /* BITS_H_ */
