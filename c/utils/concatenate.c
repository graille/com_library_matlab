#include "concatenate.h"

#include <stdio.h>
#include <math.h>
#include <limits.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

size_t concatenate_tool_get_output_size(const char * Pi, int nb_elements, OperationMode mode) {
    unsigned int P = 0;

    if (mode == SINGLE)
        P = *Pi * nb_elements;
    else
        for (int i = 0; i < nb_elements; i++)
            P += Pi[i];

    if (P % CHAR_BIT == 0)
        return (size_t) ((double) P / (double) CHAR_BIT + 1e-9);
    else
        return (size_t) ((unsigned int) floor((double) P / (double) CHAR_BIT) + 1 + 1e-9);
}

unsigned int concatenate_tool_create_mask(int start, int size)
{
    unsigned int r = 0;
    for (int i = start; i < (start + size); i++)
        r |= 1 << i;

    return r;
}

void concatenate_tool_concatenate(const unsigned int * elements, const char * Pi, int input_size, unsigned char * output_elements, int output_size, OperationMode mode) {
    unsigned int ik;
    unsigned char dk;

    int l, P = 0;
    char alpha, current_P;

    // Storage for waiting value
    unsigned int rest;

    // IMPORTANT: Init output to zero to allow zero-skip when alpha < 0
    for (l = 0; l < output_size; l++)
        output_elements[l] = 0;
    
    // Concatenate
    for (int l = 0; l < input_size; l++) {
        current_P = (mode == SINGLE) ? *Pi : Pi[l];
        rest = elements[l] & concatenate_tool_create_mask(0, current_P);

        // Select next octet
        ik = P / CHAR_BIT; // Cell number
        dk = P % CHAR_BIT; // Number of occupied bits in cell ik
        
        P += current_P;
        while(rest != 0) {
            // Calculate movement
            alpha = (char) (CHAR_BIT - dk) - current_P; // Shift quantity = Number of available bits - Size of current element
            if (alpha < 0) {
                output_elements[ik] |= (char) (rest >> -alpha);
                rest = rest & concatenate_tool_create_mask(0, current_P + alpha);

                // Calculate new position
                ik++; // Move to next memory cell
                dk = 0; // We are moving into a new cell, no bits are occupied right now
                current_P += alpha; // Update rest size
            } else {
                output_elements[ik] |= (char) (rest << alpha);
                rest = 0;
            }
        }
    }
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif