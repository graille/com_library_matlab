#include "../../quantizer.h"
#include "mex.h"

#include "../../../interfaces/matlab/mex_utils.h"

#define _INPUT_ELEMENTS prhs[0]
#define _INPUT_LEVELS prhs[1]
#define _INPUT_THRESHOLDS prhs[2]
#define _INPUT_SATURATION prhs[3]

#define _OUTPUT_VALUES plhs[0]

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /**
        Parameters:
            - prhs[0]: (vector array/matrix) Array of input elements
            - prhs[1]: (vector array) Array of levels
            - prhs[2]: (vector array) Array of threshold

            - prhs[3]: (Optionnal) (bool) Enable/Disable saturation
        Output:
            - plhs[0]: (vector array/matrix) Array containing the quantized values
    */
    
    // ------------------------------------------------------------------
    // Check input types
    // ------------------------------------------------------------------

    MEX_CHECK_NB_ARGUMENTS(nrhs, 3);

    MEX_CHECK_IS_MATRIX(_INPUT_ELEMENTS, "elements");

    MEX_CHECK_IS_DOUBLE(_INPUT_LEVELS, "levels");
    MEX_CHECK_IS_VECTOR(_INPUT_LEVELS, "levels");

    MEX_CHECK_IS_DOUBLE(_INPUT_THRESHOLDS, "thresholds");
    MEX_CHECK_IS_VECTOR(_INPUT_THRESHOLDS, "thresholds");

    if (nrhs >= 4) MEX_CHECK_IS_LOGICAL(_INPUT_SATURATION, "saturation");
    if (nrhs >= 4) MEX_CHECK_IS_SCALAR(_INPUT_SATURATION, "saturation");

    // ------------------------------------------------------------------
    // Initialize variables
    // ------------------------------------------------------------------

    int nb_elements = (int) mxGetNumberOfElements(_INPUT_ELEMENTS);
    int nb_levels = (int) mxGetNumberOfElements(_INPUT_LEVELS);
    int nb_thresholds = (int) mxGetNumberOfElements(_INPUT_THRESHOLDS);

    bool saturate = (nrhs >= 4) ? (bool) mxGetScalar(_INPUT_SATURATION) : true;

    // Get dimensions of the input matrix
    mwSize n_cols = mxGetN(_INPUT_ELEMENTS);
    mwSize n_rows = mxGetM(_INPUT_ELEMENTS);

    if(nb_levels == 0) MEX_ERROR_CREATE_ERROR_ID_TXT("badSizedArray", "You should specify at least one level");
    if(nb_thresholds != (nb_levels + 1)) MEX_ERROR_CREATE_ERROR_ID_TXT("badSizedArray", "Thresholds list must be exactly equal to levels list length + 1");

    #if MX_HAS_INTERLEAVED_COMPLEX
        double * levels = (double *) mxGetDoubles(_INPUT_LEVELS);
        double * thresholds = (double *) mxGetDoubles(_INPUT_THRESHOLDS);
    #else
        double * levels = (double *) mxGetPr(_INPUT_LEVELS);
        double * thresholds = (double *) mxGetPr(_INPUT_THRESHOLDS);
    #endif

    // Generate quantization schema
    QuantizationSchema schema = {
        .name = "",
        .levels = levels,
        .thresholds = thresholds,
        .nb_levels = nb_levels
    };
    
    // ------------------------------------------------------------------
    // Processing
    // ------------------------------------------------------------------

    int resultR = 0, resultI = 0;
    if (!MEX_CONDITION_IS_COMPLEX(_INPUT_ELEMENTS)) {
        _OUTPUT_VALUES = mxCreateDoubleMatrix(n_rows, n_cols, mxREAL);

        #if MX_HAS_INTERLEAVED_COMPLEX
            resultR = quantization_non_uniform_quantizer_quantize((double *) mxGetDoubles(_INPUT_ELEMENTS), (double *) mxGetDoubles(_OUTPUT_VALUES), nb_elements, &schema, saturate);
        #else
            resultR = quantization_non_uniform_quantizer_quantize((double *) mxGetPr(_INPUT_ELEMENTS), (double *) mxGetPr(_OUTPUT_VALUES), nb_elements, &schema, saturate);
        #endif
    } else {
        _OUTPUT_VALUES = mxCreateDoubleMatrix(n_rows, n_cols, mxCOMPLEX);

        #if MX_HAS_INTERLEAVED_COMPLEX
            resultR = quantization_non_uniform_quantizer_quantize((double *) mxGetComplexDoubles(_INPUT_ELEMENTS), (double *) mxGetComplexDoubles(_OUTPUT_VALUES), 2 * nb_elements, &schema, saturate);
        #else
            resultR = quantization_non_uniform_quantizer_quantize((double *) mxGetPr(_INPUT_ELEMENTS), (double *) mxGetPr(_OUTPUT_VALUES), nb_elements, &schema, saturate);
            resultI = quantization_non_uniform_quantizer_quantize((double *) mxGetPi(_INPUT_ELEMENTS), (double *) mxGetPi(_OUTPUT_VALUES), nb_elements, &schema, saturate);
        #endif
    }

    if (resultR == ERROR_NO_CORRESPONDING_LEVEL || resultI == ERROR_NO_CORRESPONDING_LEVEL)
        MEX_ERROR_CREATE_ERROR_ID_TXT("NoCorrespondingLevel", "Some elements have not been mapped to a specific level\n");
}