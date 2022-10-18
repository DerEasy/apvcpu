//
// Created by easy on 01.09.22.
//

#ifndef APVCPU_APV_TYPES_H
#define APVCPU_APV_TYPES_H

#include <stdint.h>

/// The bit_t type may be any integral type on which bitwise operations may be
/// performed on. It is treated as a boolean value.\n\n Its value must either be
/// 0 or 1. Any action performed on or with a bit with some other value is
/// undefined behaviour.
typedef unsigned char bit_t;

/// A word contains APV_WORD_SIZE() many bits. This implementation
/// uses a pointer to bit_t.
typedef struct word_t { bit_t *bit; } word_t;

/// Set word size/precision. Does nothing on failure.
/// \return 0 if setting precision was successful, 1 otherwise
int apv_set_word_prec(intmax_t prec);

/// If the current precision exceeds the maximum word size, internal
/// memory allocations will allocate on the heap instead of on the stack.
/// \param size Maximum word size/precision
void apv_set_word_max_size(intmax_t size);

/// \return Current word size
intmax_t APV_WORD_SIZE();

/// \return Current LSB index, which is defined as APV_WORD_SIZE() - 1.
intmax_t APV_LSB_INDEX();

/// \return Current MSB index, which is defined as 0.
intmax_t APV_MSB_INDEX();

/// \return Current actual (physical) size of a word
intmax_t APV_PHYSICAL_W_SIZE();

/// Determines if current word size exceeds maximum word size
/// \return 1 if too current word size too large, else 0.
bit_t APV_W_TOO_LARGE();


#endif //APVCPU_APV_TYPES_H
