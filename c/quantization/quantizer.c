#include "quantizer.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

double quantization_saturate(double input_x, double min_threshold, double max_threshold) {
    input_x = min(max_threshold, input_x);
    input_x = max(min_threshold, input_x);

    return input_x;
}

UniformQuantizationType get_type_from_string(char * type_str) {
    UniformQuantizationType type;

    if (_strcmpi(type_str, "MID-RISER") == 0)
        type = MID_RISER;
    else if (_strcmpi(type_str, "MID-TREAD") == 0)
        type = MID_TREAD;
    else if (_strcmpi(type_str, "MID-TREAD-DEAD-ZONE") == 0)
        type = MID_TREAD_DEAD_ZONE;
    else
        return UNKNOWN_TYPE;
}

int quantization_uniform_quantizer_quantize(double * input_x, double * output_x, int nb_elements, double step, bool saturate, UniformQuantizationType type) {
    for (int i = 0; i < nb_elements; i++) {
        // Saturation
        output_x[i] = (saturate) ? quantization_saturate(input_x[i], -1.0, 1.0) : input_x[i];
        
        // Quantization
        if (step >= 0) {
            switch (type)
            {
            case MID_TREAD:
                output_x[i] = step * floor(output_x[i] / step + 0.5);
                break;
            case MID_RISER:
                output_x[i] = step * (floor(output_x[i] / step) + 0.5);
                break;
            case MID_TREAD_DEAD_ZONE:
                // Todo
                break;
            default:
                return ERROR_UNKNOWN_QUANTIZATION_TYPE;
                break;
            }
        }
    }

    return QUANTIZATION_SUCCESS;
}

int quantization_non_uniform_quantizer_quantize(double * input_x, double * output_x, int nb_elements, QuantizationSchema * schema, bool saturate) {
    int i, j;
    bool value_found;
    
    for(i = 0; i < nb_elements; i++) {
        output_x[i] = (saturate) ? quantization_saturate(input_x[i], -1.0, 1.0) : input_x[i];
        
        value_found = false;
        if (output_x[i] <= schema->thresholds[1]) {
            output_x[i] = schema->levels[0];
            value_found = true;
        } else if (output_x[i] > schema->thresholds[schema->nb_levels - 1]) {
            output_x[i] = schema->levels[schema->nb_levels - 1];
            value_found = true;
        } else {
            for(j = 1; j < schema->nb_levels - 1; j++) {
                if ((output_x[i] > schema->thresholds[j]) && (output_x[i] <= schema->thresholds[j + 1])) {
                    output_x[i] = schema->levels[j];
                    value_found = true;
                    break;
                }
            }
        }

        if (!value_found) return ERROR_NO_CORRESPONDING_LEVEL;
    }

    return QUANTIZATION_SUCCESS;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif