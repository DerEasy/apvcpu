//
// Created by easy on 01.09.22.
//

#ifndef APVCPU_APV_ALU_H
#define APVCPU_APV_ALU_H

#include "apv_types.h"
#include "gmp.h"

/// Convenient for loop to iterate the bit array (starting from MSB)
#define foreachbit(i) for (intmax_t i = APV_MSB_INDEX(); i < APV_WORD_SIZE(); ++i)

/// Convenient for loop to iterate the bit array (starting from MSB), with an extra condition
#define foreachbitc(i) for (intmax_t i = APV_MSB_INDEX(); i < APV_WORD_SIZE(); ++i)

/// Convenient for loop to _r_eversely iterate the bit array (starting from LSB)
#define foreachbit_r(i) for (intmax_t i = APV_WORD_SIZE(); i-- > APV_MSB_INDEX();)

/// Convenient for loop to _R_eversely iterate the bit array (starting from LSB), with an extra condition
#define foreachbitc_r(i, cond) for (intmax_t i = APV_WORD_SIZE(); i-- > APV_MSB_INDEX() && (cond);)

/// Convenient for loop to _s_can the bit array from both sides at once.
/// The left parameter begins at MSB, the right at LSB.
#define foreachbit_s(l, r) for (intmax_t l = APV_MSB_INDEX(), r = APV_LSB_INDEX(); l <= r; ++l, --r)

/// Allocates a word on the stack and checks if the bit array fits on the stack as well.
/// If it does, the array is allocated via alloca, else malloc is used.
/// \return Address of the word (to conform with other functions always using pointers.)
#define apv_allocq_w() (&(word_t) { \
    .bit = APV_W_TOO_LARGE() ?   \
    malloc(APV_PHYSICAL_W_SIZE()) : \
    alloca(APV_PHYSICAL_W_SIZE())   \
})

/// Frees whatever apv_allocq_w allocated if viable.\n\n
/// DO NOT modify the maximum word size before calling this function on any remaining
/// words that have been allocated via apv_allocq_w, as the function relies on the
/// output of APV_W_TOO_LARGE() to determine whether to free the memory or not.
#define apv_freeq_w(word) \
    if (APV_W_TOO_LARGE()) { \
        free((word)->bit); \
        free((word)); \
    } do;while(0)





/// Allocates a word on heap using a string of the desired number.
/// \param init A C-string containing the number, possibly with a preceding '-'
/// \param base The base of the number-string
/// \return Pointer to word with corresponding value
word_t *apv_create_w(const char *init, int base);

/// Allocates a word on heap using a string of the desired number in binary.
/// \param init A C-string containing the number in binary (two's complement)
/// \param len Length of the init string
/// \param filler Used to fill the rest of the bits if len is less than word size
/// \return Pointer to word with corresponding value
word_t *apv_binary_w(const char *init, size_t len, bit_t filler);

/// Allocates a word on heap with an undefined value.\n Using the word in any
/// calculations without further modification results in undefined behaviour.
/// This function is mainly useful if you just need a word in which you can
/// copy some other value you already have.
/// \return Pointer to word with undefined value
word_t *apv_undef_w();

/// Allocates a zero-initialised word on heap.
/// \return Pointer to word with value 0
word_t *apv_zero_w();

/// Frees a dynamically allocated word.
/// \param word The word to free
void apv_free_w(word_t *word);

/// \return Carry bit
bit_t apv_half_adder(bit_t a, bit_t b, bit_t *dest);

/// \return Carry bit
bit_t apv_full_adder(bit_t a, bit_t b, bit_t carry, bit_t *dest);

/// \return Carry bit
bit_t apv_half_subtractor(bit_t a, bit_t b, bit_t *dest);

/// \return Carry bit
bit_t apv_full_subtractor(bit_t a, bit_t b, bit_t carry, bit_t *dest);

/// \return Carry bit
bit_t apv_add(word_t *a, word_t *b, bit_t carry, word_t *dest);

/// \return Carry bit
bit_t apv_sub(word_t *a, word_t *b, bit_t carry, word_t *dest);

/// \return Most significant bit/sign bit
bit_t apv_msb(word_t *word);

/// \return Least significant bit
bit_t apv_lsb(word_t *word);

/// Bitwise-NOT
void apv_not(word_t *src, word_t *dest);

/// Two's complement
void apv_twos(word_t *src, word_t *dest);

/// Clears all bits (sets them to 0)
void apv_clear(word_t *word);

/// Sets all bits (sets them to 1)
void apv_set(word_t *word);

/// Moves all bits from src to dest
void apv_mov(word_t *src, word_t *dest);

/// Absolute value
void apv_abs(word_t *src, word_t *dest);

/// Increment
void apv_inc(word_t *src, word_t *dest);

/// Decrement
void apv_dec(word_t *src, word_t *dest);

/// Bitwise-AND
void apv_and(word_t *a, word_t *b, word_t *dest);

/// Bitwise-OR
void apv_or(word_t *a, word_t *b, word_t *dest);

/// Bitwise-XOR
void apv_xor(word_t *a, word_t *b, word_t *dest);

/// Logical/Arithmetic left shift
/// \return Carry bit
bit_t apv_shl(word_t *src, word_t *dest, intmax_t shift);

/// Logical right shift
/// \return Carry bit
bit_t apv_shr(word_t *src, word_t *dest, intmax_t shift);

/// Arithmetic right shift
/// \return Carry bit
bit_t apv_sar(word_t *src, word_t *dest, intmax_t shift);

/// Checks for equality (signed and unsigned)
bit_t apv_cmp_eq(word_t *a, word_t *b);

/// Checks for inequality (signed and unsigned)
bit_t apv_cmp_neq(word_t *a, word_t *b);

/// Signed comparison:\n a greather than b
bit_t apv_icmp_g(word_t *a, word_t *b);

/// Signed comparison:\n a greater or equal to b
bit_t apv_icmp_ge(word_t *a, word_t *b);

/// Signed comparison:\n a less than b
bit_t apv_icmp_l(word_t *a, word_t *b);

/// Signed comparison:\n a less or equal to b
bit_t apv_icmp_le(word_t *a, word_t *b);

/// Unsigned comparison:\n a greather than b
bit_t apv_cmp_g(word_t *a, word_t *b);

/// Unsigned comparison:\n a greater or equal to b
bit_t apv_cmp_ge(word_t *a, word_t *b);

/// Unsigned comparison:\n a less than b
bit_t apv_cmp_l(word_t *a, word_t *b);

/// Unsigned comparison:\n a less or equal to b
bit_t apv_cmp_le(word_t *a, word_t *b);


#endif //APVCPU_APV_ALU_H
