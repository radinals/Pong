#ifndef LETTERS_H
#define LETTERS_H

#include <cstddef>

namespace Letters {

    static constexpr size_t LETTER_H      = 5;
    static constexpr size_t LETTER_W      = 3;
    static constexpr size_t LETTER_AMOUNT = 4;

    const static bool letters[LETTER_AMOUNT][LETTER_H][LETTER_W] {
        {
            // ZERO
            { 1, 1, 1 },
            { 1, 0, 1 },
            { 1, 0, 1 },
            { 1, 0, 1 },
            { 1, 1, 1 },
        },

        {
            // ONE
            { 1, 1, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 1, 1, 1 },

        },

        {
            // TWO
            { 1, 1, 1 },
            { 0, 0, 1 },
            { 1, 1, 1 },
            { 1, 0, 0 },
            { 1, 1, 1 },

        },

        {
            // THREE
            { 1, 1, 1 },
            { 0, 0, 1 },
            { 1, 1, 1 },
            { 0, 0, 1 },
            { 1, 1, 1 },

        },

    };
};    // namespace Letters

#endif    // !LETTERS_H
