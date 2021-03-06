/* Copyright 2013 Google Inc. All Rights Reserved.

   Distributed under MIT license.
   See file LICENSE for detail or copy at https://opensource.org/licenses/MIT
*/

/* Bit reading helpers */

#include "./bit_reader.h"

#include "./platform.h"
#include "./types.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

void BrotliInitBitReader(BrotliBitReader* const br) {
  br->val_ = 0;
  br->bit_pos_ = sizeof(br->val_) << 3;
}

BROTLI_BOOL BrotliWarmupBitReader(BrotliBitReader* const br) {
  size_t aligned_read_mask = (sizeof(br->val_) >> 1) - 1;
  /* Fixing alignment after unaligned BrotliFillWindow would result accumulator
     overflow. If unalignment is caused by BrotliSafeReadBits, then there is
     enough space in accumulator to fix alignment. */
  if (!BROTLI_ALIGNED_READ) {
    aligned_read_mask = 0;
  }
  if (BrotliGetAvailableBits(br) == 0) {
    if (!BrotliPullByte(br)) {
      return BROTLI_FALSE;
    }
  }

  while ((((size_t)br->next_in) & aligned_read_mask) != 0) {
    if (!BrotliPullByte(br)) {
      /* If we consumed all the input, we don't care about the alignment. */
      return BROTLI_TRUE;
    }
  }
  return BROTLI_TRUE;
}

BROTLI_BOOL BrotliSafeReadBits32Slow(BrotliBitReader* const br,
    uint32_t n_bits, uint32_t* val) {
  uint32_t low_val;
  uint32_t high_val;
  BrotliBitReaderState memento;
  BROTLI_DCHECK(n_bits <= 32);
  BROTLI_DCHECK(n_bits > 24);
  BrotliBitReaderSaveState(br, &memento);
  if (!BrotliSafeReadBits(br, 16, &low_val) ||
      !BrotliSafeReadBits(br, n_bits - 16, &high_val)) {
    BrotliBitReaderRestoreState(br, &memento);
    return BROTLI_FALSE;
  }
  *val = low_val | (high_val << 16);
  return BROTLI_TRUE;
}

#if defined(__cplusplus) || defined(c_plusplus)
}  /* extern "C" */
#endif
