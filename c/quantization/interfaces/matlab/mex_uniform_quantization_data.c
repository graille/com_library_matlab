#include "../../quantizer.h"
#include "../../../misc/pstring.h"

#include "../../../interfaces/matlab/mex_utils.h"

#include "mex.h"

// Internal definitions
#define _INPUT_STEP (prhs[0])
#define _INPUT_TYPE (prhs[1])

#define _OUTPUT_RESULT_STRUCT (plhs[0])

#define _IS_DOUBLE_EQUAL(a, b) (fabs(((double) a) - ((double) b)) < (double) 1e-9)

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /**
        Parameters:
            - prhs[0]: (string) Step
            - prhs[1]: (string) Type
        Output:
            - plhs[0]: (structure) s
                s.levels: Levels of each quantized values
                s.thresholds: Threshold between each levels
    */
    
    // ------------------------------------------------------------------
    // Check input types
    // ------------------------------------------------------------------

    MEX_CHECK_NB_ARGUMENTS(nrhs, 1);

    MEX_CHECK_IS_DOUBLE(_INPUT_STEP, "step");
    MEX_CHECK_IS_SCALAR(_INPUT_STEP, "step");

    if (nrhs >= 2) MEX_CHECK_IS_STRING(_INPUT_TYPE, "type");

    // ------------------------------------------------------------------
    // Initialize variables
    // ------------------------------------------------------------------

    int i, k, nb_levels = 0;
    double step = (double) mxGetScalar(_INPUT_STEP);
    int goal_precision = (step > 0) ? (int) (4.0 / step + 1.0) : 0;

    // Manage type
    UniformQuantizationType type = MID_RISER;
    if (nrhs >= 2)
        type = get_type_from_string(mxArrayToString(_INPUT_TYPE));

    if (type == UNKNOWN_TYPE)
        mexErrMsgIdAndTxt(MEX_ERROR_CATEGORY_MAIN_CATEGORY ":unknownType", "Unknown type \"%s\"", mxArrayToString(_INPUT_TYPE));
    
    // Temp variables
    double * in_array = malloc(goal_precision * sizeof(double));
    double * out_array = malloc(goal_precision * sizeof(double));
    double * levels_array = malloc(goal_precision * sizeof(double));
    double * thresholds_array = malloc(goal_precision * sizeof(double));

    for(i = 0; i < goal_precision; i++)
        in_array[i] = i*(2.0/((double) goal_precision - 1.0)) - 1.0;
    
    // ------------------------------------------------------------------
    // Processing
    // ------------------------------------------------------------------

    quantization_uniform_quantizer_quantize(in_array, out_array, goal_precision, step, false, type);

    // ------------------------------------------------------------------
    // Post-processing
    // ------------------------------------------------------------------

    // Calculate levels and thresholds
    bool already_included;
    for(i = 0; i < goal_precision; i++) {
        already_included = false;
        for(k = 0; k < nb_levels; k++) {
            if (_IS_DOUBLE_EQUAL(out_array[i], levels_array[k])) {
                already_included = true;
                break;
            }
        }

        // Add level if already included
        if (!already_included) {
            levels_array[nb_levels] = out_array[i];

            if(nb_levels >= 1)
                thresholds_array[nb_levels - 1] = (levels_array[nb_levels] + levels_array[nb_levels - 1]) / 2.0;

            nb_levels++;
        }
    }

    // Manage outputs
    mxArray* output_levels_field = mxCreateDoubleMatrix(1, nb_levels, mxREAL);
    mxArray* output_thresholds_field = mxCreateDoubleMatrix(1, nb_levels - 1, mxREAL);
    
    #if MX_HAS_INTERLEAVED_COMPLEX
        double * output_levels_array = (double *) mxGetDoubles(output_levels_field);
        double * output_thresholds_array = (double *) mxGetDoubles(output_thresholds_field);
    #else
        double * output_levels_array = (double *) mxGetPr(output_levels_field);
        double * output_thresholds_array = (double *) mxGetPr(output_thresholds_field);
    #endif

    const mwSize struct_dim[] = {1};
    const char * struct_fields_name[] = {"levels", "thresholds"};
    _OUTPUT_RESULT_STRUCT = mxCreateStructArray((mwSize) 1, struct_dim, 2, struct_fields_name);

    // Fill outputs
    for(i = 0; i < nb_levels; i++)     output_levels_array[i] = levels_array[i];
    for(i = 0; i < nb_levels - 1; i++) output_thresholds_array[i] = thresholds_array[i];

    mxSetField(_OUTPUT_RESULT_STRUCT, 0, struct_fields_name[0], output_levels_field); 
    mxSetField(_OUTPUT_RESULT_STRUCT, 0, struct_fields_name[1], output_thresholds_field); 

    // Free temp structures
    free(in_array);
    free(levels_array);
    free(thresholds_array);
    free(out_array);
}