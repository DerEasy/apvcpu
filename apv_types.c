//
// Created by easy on 12.09.22.
//

#include "apv_types.h"

static intmax_t WORD_SIZE = 8; // Default value: 8
static intmax_t MSB_INDEX = 0; // This implementation always has MSB_INDEX = 0
static intmax_t LSB_INDEX = 7; // WORD_SIZE - 1
static intmax_t PHYS_SIZE = 8 * sizeof(bit_t); // WORD_SIZE * sizeof(bit_t)
static intmax_t MAX_SIZE = 1 << 10; // Tinker with this to fit the system's requirements
static bit_t TOO_LARGE_FOR_STACK; // Merely a boolean


int apv_set_word_prec(intmax_t prec) {
    if (prec < 1) return 1;

    WORD_SIZE = prec;
    LSB_INDEX = WORD_SIZE - 1;
    PHYS_SIZE = WORD_SIZE * (intmax_t) sizeof(bit_t);
    TOO_LARGE_FOR_STACK = PHYS_SIZE >= MAX_SIZE;
    return 0;
}

void apv_set_word_max_size(const intmax_t size) {
    MAX_SIZE = size;
    TOO_LARGE_FOR_STACK = WORD_SIZE * sizeof(bit_t) >= MAX_SIZE;
}

intmax_t APV_WORD_SIZE() {
    return WORD_SIZE;
}

intmax_t APV_LSB_INDEX() {
    return LSB_INDEX;
}

intmax_t APV_MSB_INDEX() {
    return MSB_INDEX;
}

intmax_t APV_PHYSICAL_W_SIZE() {
    return PHYS_SIZE;
}

bit_t APV_W_TOO_LARGE() {
    return TOO_LARGE_FOR_STACK;
}