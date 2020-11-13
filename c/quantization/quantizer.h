#ifndef USEFUL_LIB_QUANTIZER
#define USEFUL_LIB_QUANTIZER

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../misc/pmath.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

// Return values
#define QUANTIZATION_SUCCESS 0x0

#define ERROR_NO_CORRESPONDING_LEVEL 0x7842
#define ERROR_UNKNOWN_QUANTIZATION_TYPE 0x8741

// Structs and types
typedef unsigned short NQ_DATA_TYPE;
typedef enum UniformQuantizationType {
    MID_TREAD,
    MID_TREAD_DEAD_ZONE,
    MID_RISER,

    UNKNOWN_TYPE
} UniformQuantizationType;

typedef struct QuantizationSchema {
    char * name;
    
    double * levels;
    double * thresholds;

    int nb_levels;
} QuantizationSchema;

// Functions
double quantization_saturate(double input_x, double min_threshold, double max_threshold);

/**
 * Return the uniform quantization type type from a string
*/
UniformQuantizationType get_type_from_string(char * type_str);

/**
 * Quantize (in an uniform way) values with on a specified number of bit
 * Input parameters:
 *      - input_x: array of input values
 *      - output_x: pre-allocated array where output values will be written
 *      - nb_elements: Number of elements in input_x and output_x
 *      - step: Step between two values
 *      - saturate: (bool) Data saturation
 *      - type: Type of quantization (MID_TREAD, MID_RISER, ...)
 * Output parameters:
 *      - code: Code of function, 0 = success, other values refer to an error
*/ 
int quantization_uniform_quantizer_quantize(double * input_x, double * output_x, int nb_elements, double step, bool saturate, UniformQuantizationType type);

/**
 * Quantize an array using a quantization schema
 * Input parameters:
 *      - input_x: array of input values
 *      - output_x: pre-allocated array where output values will be written
 *      - nb_elements: Number of elements in input_x and output_x
 *      - schema: Quantization schema
 *      - saturate: (bool) Data saturation
 * Output parameters:
 *      - code: Code of function, 0 = success, other values refer to an error
*/ 
int quantization_non_uniform_quantizer_quantize(double * input_x, double * output_x, int nb_elements, QuantizationSchema * schema, bool saturate);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif // USEFUL_LIB_QUANTIZER