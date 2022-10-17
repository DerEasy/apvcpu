//
// Created by easy on 01.09.22.
//

#include "apv_alu.h"
#include <stdlib.h>
#include <string.h>

word_t *apv_create_w(const char *init, const int base) {
    /* This function is messy and just improvised */

    bit_t is_negative = *init == '-';
    if (is_negative) ++init;

    mpz_t str_converter;
    mpz_init_set_str(str_converter, init, base);

    char *str = mpz_get_str(NULL, 2, str_converter); // Convert to base-2 string
    size_t len = strlen(str);
    str += len - 1; // Jump to last character

    intmax_t i;
    word_t *word = apv_undef_w();

    for (i = 0; i < len; ++i)
        word->bit[APV_LSB_INDEX() - i] = str[-i] - '0';
    memset(word->bit, 0, APV_WORD_SIZE() - i);

    if (is_negative) {
        word_t *zero = apv_allocq_w();
        apv_clear(zero);
        apv_sub(zero, word, 0, word);
        apv_freeq_w(zero);
    }

    return word;
}

word_t *apv_binary_w(const char *init, size_t len, const bit_t filler) {
    if (len > APV_WORD_SIZE()) {
        // If the given length exceeds the word size, we will only fetch the lower bits
        init += len - APV_WORD_SIZE();
        len = APV_WORD_SIZE();
    }

    const size_t offset = APV_WORD_SIZE() - len;
    word_t *word = apv_undef_w();
    memset(word->bit, filler, offset); // Fill the highest bits with the filler bit first

    // Then just copy the actual binary number to the remaining bits of the word
    for (intmax_t i = 0; i < len; ++i)
        word->bit[offset + i] = init[i] - '0';
    return word;
}

word_t *apv_undef_w() {
    // To be efficient, we do a single allocation in which we store
    // 1) the word_t struct itself, followed by
    // 2) the actual bit array memory
    word_t *word = malloc(APV_PHYSICAL_W_SIZE() + sizeof(word_t));
    // (word + 1) skips the first sizeof(word_t) bytes (which we use for the
    // struct), the rest is used as the bit array.
    word->bit = (bit_t *) (word + 1);
    return word;
}

word_t *apv_zero_w() {
    // To be efficient, we do a single allocation in which we store
    // 1) the word_t struct itself, followed by
    // 2) the actual bit array memory
    word_t *word = calloc(1, APV_PHYSICAL_W_SIZE() + sizeof(word_t));
    // (word + 1) skips the first sizeof(word_t) bytes (which we use for the
    // struct), the rest is used as the bit array.
    word->bit = (bit_t *) (word + 1);
    return word;
}

void apv_free_w(word_t *word) {
    if (word != NULL) free(word);
}






bit_t apv_half_adder(bit_t a, bit_t b, bit_t *dest) {
    *dest = a ^ b; // Sum
    return a & b; // Carry
}

bit_t apv_full_adder(bit_t a, bit_t b, bit_t carry, bit_t *dest) {
    bit_t cacheSum;
    bit_t cacheCarry = apv_half_adder(a, b, &cacheSum);
    return cacheCarry | apv_half_adder(cacheSum, carry, dest); // OR the carries
}

bit_t apv_half_subtractor(bit_t a, bit_t b, bit_t *dest) {
    *dest = a ^ b; // Difference
    return (!a) & b; // Carry
}

bit_t apv_full_subtractor(bit_t a, bit_t b, bit_t carry, bit_t *dest) {
    bit_t cacheDiff;
    bit_t cacheCarry = apv_half_subtractor(a, b, &cacheDiff);
    return cacheCarry | apv_half_subtractor(cacheDiff, carry, dest); // OR the carries
}

bit_t apv_add(word_t *a, word_t *b, bit_t carry, word_t *dest) {
    // This function emulates a carry-ripple-adder
    foreachbit_r(i)
        carry = apv_full_adder(a->bit[i], b->bit[i], carry, dest->bit + i);
    return carry;
}

bit_t apv_sub(word_t *a, word_t *b, bit_t carry, word_t *dest) {
    // This function emulates a carry-ripple-subtractor
    foreachbit_r(i)
        carry = apv_full_subtractor(a->bit[i], b->bit[i], carry, dest->bit + i);
    return carry;
}

bit_t apv_msb(word_t *word) {
    return word->bit[APV_MSB_INDEX()];
}

bit_t apv_lsb(word_t *word) {
    return word->bit[APV_LSB_INDEX()];
}

void apv_not(word_t *src, word_t *dest) {
    // Inverting all bits is equivalent to constructing the one's complement
    foreachbit(i) dest->bit[i] = !src->bit[i];
}

void apv_twos(word_t *src, word_t *dest) {
    intmax_t i = APV_LSB_INDEX();

    // Copy all the bits from right to left
    do dest->bit[i] = src->bit[i];
    while (!src->bit[i] && --i >= APV_MSB_INDEX());
    // When the first 1 bit is encountered, copy that bit as well, then stop

    // Next, copy all the remaining bits, but invert them first
    for (--i; i >= APV_MSB_INDEX(); --i)
        dest->bit[i] = !src->bit[i];
}

void apv_clear(word_t *word) {
    memset(word->bit, 0, APV_PHYSICAL_W_SIZE());
}

void apv_set(word_t *word) {
    memset(word->bit, 1, APV_PHYSICAL_W_SIZE());
}

void apv_mov(word_t *src, word_t *dest) {
    if (src == dest) return;
    memcpy(dest->bit, src->bit, APV_PHYSICAL_W_SIZE());
}

void apv_abs(word_t *src, word_t *dest) {
    // If the MSB is not set we just need to copy, else we construct the two's complement
    apv_msb(src) ? apv_twos(src, dest) : apv_mov(src, dest);
}

void apv_inc(word_t *src, word_t *dest) {
    bit_t carry = 1;
    // An increment is an addition by a constant summand. We can optimise this process
    // by stopping when there is no carry left, because we know beforehand that there
    // will never be one after that.
    foreachbitc_r(i, carry)
        carry = apv_full_adder(src->bit[i], 0, carry, dest->bit + i);
}

void apv_dec(word_t *src, word_t *dest) {
    bit_t carry = 1;
    // An increment is a subtraction by a constant subtrahend. We can optimise this process
    // by stopping when there is no carry left, because we know beforehand that there
    // will never be one after that.
    foreachbitc_r(i, carry)
        carry = apv_full_subtractor(src->bit[i], 0, carry, dest->bit + i);
}

void apv_and(word_t *a, word_t *b, word_t *dest) {
    foreachbit(i) dest->bit[i] = a->bit[i] & b->bit[i];
}

void apv_or(word_t *a, word_t *b, word_t *dest) {
    foreachbit(i) dest->bit[i] = a->bit[i] | b->bit[i];
}

void apv_xor(word_t *a, word_t *b, word_t *dest) {
    foreachbit(i) dest->bit[i] = a->bit[i] ^ b->bit[i];
}

/**
 * Checks if the requested shift is out of bounds.
 * @param src source word
 * @param dest destination word
 * @param shift amount to be shifted by
 * @param carryIndex index of where the carry should be fetched from
 *                   in the case that shift equals the word size
 * @param carry storage where the carry is to be stored
 * @return 1 if shift was out of bounds, 0 otherwise
 */
bit_t apv_shift_bound_check(word_t *src, word_t *dest, const intmax_t shift, intmax_t carryIndex, bit_t *carry) {
    if (shift <= 0) { // Shifting by zero or a negative number will just move the word to its destination
        *carry = 0; // There can be no carry when we do no shifting
        apv_mov(src, dest);
        return 1;
    }
    if (shift >= APV_WORD_SIZE()) { // This case actually encompasses two distinct scenarios
        // If the shift is equal to the word size, we need to fetch the bit from one end of the word.
        // In the case of a left shift, the carry is the LSB. For right shifts, it's the MSB.
        // If the shift exceeds the word size, there can be no carry, so we just set the carry to 0.
        *carry = shift == APV_WORD_SIZE() ? src->bit[carryIndex] : 0;
        apv_clear(dest); // In either scenario, the word ought to be cleared.
        return 1;
    }
    return 0; // Shift is inside bounds
}

/**
 * As to not repeat ourselves, the right shift can be generified, as the only difference
 * between an arithmetic and logical right shift is the "filler bit". The logical
 * right shift's filler bit is 0 (constant), the arithmetic's is its MSB (variable).
 * @param src source word
 * @param dest destination word
 * @param shift amount to shift by
 * @param filler the "filler bit" which is constant for a logical right shift but may
 *               vary for an arithmetic one
 * @return carry bit (the last bit that has been shifted out of bounds)
 */
bit_t apv_shift_right_generic(word_t *src, word_t *dest, const intmax_t shift, bit_t filler) {
    bit_t carry;
    if (apv_shift_bound_check(src, dest, shift, APV_MSB_INDEX(), &carry))
        return carry; // The bound check will have done all the work, so we can just stop here

    intmax_t destIndex = APV_LSB_INDEX();
    // Fetch the carry bit before it may be overwritten
    carry = src->bit[APV_LSB_INDEX() + 1 - shift];

    for (intmax_t srcIndex = APV_LSB_INDEX() - shift; srcIndex >= APV_MSB_INDEX();)
        dest->bit[destIndex--] = src->bit[srcIndex--]; // Copy all bits that are in-bounds

    // At this point the rest of the bits will be a constant, so fill them with memset
    memset(dest->bit, filler, destIndex - APV_MSB_INDEX() + 1);
    return carry;
}

bit_t apv_shl(word_t *src, word_t *dest, const intmax_t shift) {
    bit_t carry;
    if (apv_shift_bound_check(src, dest, shift, APV_LSB_INDEX(), &carry))
        return carry; // The bound check will have done all the work, so we can just stop here

    intmax_t destIndex = APV_MSB_INDEX();
    // Fetch the carry bit before it may be overwritten
    carry = src->bit[APV_MSB_INDEX() - 1 + shift];

    for (intmax_t srcIndex = APV_MSB_INDEX() + shift; srcIndex <= APV_LSB_INDEX();)
        dest->bit[destIndex++] = src->bit[srcIndex++]; // Copy all bits that are in-bounds

    // At this point the rest of the bits will be 0, so fill them with memset
    memset(dest->bit + destIndex, 0, APV_LSB_INDEX() - destIndex + 1);
    return carry;
}

bit_t apv_shr(word_t *src, word_t *dest, const intmax_t shift) {
    // A logical right shift always fills the new bits with 0
    return apv_shift_right_generic(src, dest, shift, 0);
}

bit_t apv_sar(word_t *src, word_t *dest, const intmax_t shift) {
    // An arithmetic right shift fills the new bits with the MSB
    return apv_shift_right_generic(src, dest, shift, apv_msb(src));
}

bit_t apv_cmp_eq(word_t *a, word_t *b) {
    if (a == b) return 1;

    foreachbit_s(l, r) {
        if (a->bit[l] ^ b->bit[l] || a->bit[r] ^ b->bit[r])
            return 0;
    } return 1;
}

bit_t apv_cmp_neq(word_t *a, word_t *b) {
    if (a == b) return 0;

    foreachbit_s(l, r) {
        if (a->bit[l] ^ b->bit[l] || a->bit[r] ^ b->bit[r])
            return 1;
    } return 0;
}

bit_t apv_icmp_g(word_t *a, word_t *b) {
    if (a == b) return 0;
    if (apv_msb(a) ^ apv_msb(b)) return apv_msb(b);

    word_t *diff = apv_allocq_w();
    apv_sub(a, b, 1, diff);
    bit_t positive = !apv_msb(diff);
    apv_freeq_w(diff);

    return positive;
}

bit_t apv_icmp_ge(word_t *a, word_t *b) {
    if (a == b) return 1;
    if (apv_msb(a) ^ apv_msb(b)) return apv_msb(b);

    word_t *diff = apv_allocq_w();
    apv_sub(a, b, 0, diff);
    bit_t positive = !apv_msb(diff);
    apv_freeq_w(diff);

    return positive;
}

bit_t apv_icmp_l(word_t *a, word_t *b) {
    if (a == b) return 0;
    if (apv_msb(a) ^ apv_msb(b)) return apv_msb(a);

    word_t *diff = apv_allocq_w();
    apv_sub(a, b, 0, diff);
    bit_t negative = apv_msb(diff);
    apv_freeq_w(diff);

    return negative;
}

bit_t apv_icmp_le(word_t *a, word_t *b) {
    if (a == b) return 1;
    if (apv_msb(a) ^ apv_msb(b)) return apv_msb(a);

    word_t *diff = apv_allocq_w();
    apv_sub(a, b, 1, diff);
    bit_t negative = apv_msb(diff);
    apv_freeq_w(diff);

    return negative;
}

bit_t apv_cmp_g(word_t *a, word_t *b) {
    if (a == b) return 0;

    foreachbit(i) {
        if (a->bit[i] ^ b->bit[i])
            return a->bit[i];
    } return 0;
}

bit_t apv_cmp_ge(word_t *a, word_t *b) {
    if (a == b) return 1;

    foreachbit(i) {
        if (a->bit[i] ^ b->bit[i])
            return a->bit[i];
    } return 1;
}

bit_t apv_cmp_l(word_t *a, word_t *b) {
    if (a == b) return 0;

    foreachbit(i) {
        if (a->bit[i] ^ b->bit[i])
            return b->bit[i];
    } return 0;
}

bit_t apv_cmp_le(word_t *a, word_t *b) {
    if (a == b) return 1;

    foreachbit(i) {
        if (a->bit[i] ^ b->bit[i])
            return b->bit[i];
    } return 1;
}