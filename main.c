//
// Created by easy on 01.09.22.
//

#include "apv_alu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *apv_bitstr(word_t *word) {
    char *str = malloc(APV_WORD_SIZE() + 1);
    str[APV_WORD_SIZE()] = '\0';
    foreachbit(i) str[i] = (char) (word->bit[i] + '0');
    return str;
}

char *apv_numstr_u(word_t *word, const int base) {
    char str[APV_WORD_SIZE() + 1];
    str[APV_WORD_SIZE()] = '\0';
    foreachbit(i) str[i] = (char) (word->bit[i] + '0');

    mpz_t num;
    mpz_init_set_str(num, str, 2);

    return mpz_get_str(NULL, base, num);
}

char *apv_numstr_s(word_t *word, const int base) {
    if (word->bit[APV_MSB_INDEX()]) {
        char str[APV_WORD_SIZE() + 1];
        str[APV_WORD_SIZE()] = '\0';

        word_t *twos = apv_undef_w();
        apv_twos(word, twos);

        foreachbit(i) str[i] = (char) (twos->bit[i] + '0');
        apv_free_w(twos);

        mpz_t num;
        mpz_init_set_str(num, str, 2);

        char *output = malloc(mpz_sizeinbase(num, base) + 2);
        output[0] = '-';
        mpz_get_str(output + 1, base, num);
        return output;
    }

    return apv_numstr_u(word, base);
}

int main() {
    apv_set_word_prec(8);

    word_t *a = apv_create_w("56", 10);
    word_t *b = apv_create_w("178", 10);
    word_t *c = apv_create_w("56", 10);
    word_t *d = apv_create_w("178", 10);


    puts(apv_bitstr(a));
    puts(apv_bitstr(b));

    apv_not(a, a);
    puts(apv_bitstr(a));
    apv_add(a, b, 0, b);
    puts(apv_bitstr(b));



    puts(apv_bitstr(c));
    puts(apv_bitstr(d));

    apv_not(d, d);
    puts(apv_bitstr(d));
    apv_add(c, d, 0, d);
    puts(apv_bitstr(d));
}
