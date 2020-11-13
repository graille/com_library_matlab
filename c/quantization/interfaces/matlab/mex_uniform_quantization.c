#include "../../quantizer.h"
#include "../../../misc/pstring.h"

#include "../../../interfaces/matlab/mex_utils.h"

#include "mex.h"

#define _INPUT_ELEMENTS prhs[0]
#define _INPUT_STEP prhs[1]
#define _INPUT_TYPE prhs[2]
#define _INPUT_SATURATION prhs[3]

#define _OUTPUT_VALUES plhs[0]

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /**
        Parameters:
            - prhs[0]: (vector array/matrix) Array of input elements
            - prhs[1]: (double) step between levels
            - prhs[2]: (string) Type
            - prhs[3]: (bool) Enable saturation (default: true)
        Output:
            - plhs[0]: (vector array/matrix) Array containing the quantized values
    */
    
    // ------------------------------------------------------------------
    // Check input types
    // ------------------------------------------------------------------

    MEX_CHECK_NB_ARGUMENTS(nrhs, 2);
    
    MEX_CHECK_IS_MATRIX(_INPUT_ELEMENTS, "elements");

    MEX_CHECK_IS_DOUBLE(_INPUT_STEP, "step");
    MEX_CHECK_IS_SCALAR(_INPUT_STEP, "step");
    
    if (nrhs >= 3) MEX_CHECK_IS_STRING(_INPUT_TYPE, "type");

    if (nrhs >= 4) MEX_CHECK_IS_LOGICAL(_INPUT_SATURATION, "saturation");
    if (nrhs >= 4) MEX_CHECK_IS_SCALAR(_INPUT_SATURATION, "saturation");

    // ------------------------------------------------------------------
    // Initialize variables
    // ------------------------------------------------------------------

    int nb_elements = (int) mxGetNumberOfElements(_INPUT_ELEMENTS);
    double step = (double) mxGetScalar(_INPUT_STEP);

    // Manage saturation
    bool saturate = (nrhs >= 4) ? (bool) mxGetScalar(_INPUT_SATURATION) : true;

    // Manage type
    UniformQuantizationType type = (nrhs >= 3) ? get_type_from_string(mxArrayToString(_INPUT_TYPE)) : MID_RISER;

    if (type == UNKNOWN_TYPE) MEX_ERROR_CREATE_ERROR_ID_TXT("unknownType", "Unknown quantization type \"%s\"", mxArrayToString(_INPUT_TYPE));
    
    // ------------------------------------------------------------------
    // Processing
    // ------------------------------------------------------------------

    if (!mxIsComplex(_INPUT_ELEMENTS)) {
        _OUTPUT_VALUES = mxCreateDoubleMatrix(mxGetN(_INPUT_ELEMENTS), mxGetM(_INPUT_ELEMENTS), mxREAL);

        #if MX_HAS_INTERLEAVED_COMPLEX
            quantization_uniform_quantizer_quantize((double *) mxGetDoubles(_INPUT_ELEMENTS), (double *) mxGetDoubles(_OUTPUT_VALUES), nb_elements, step, saturate, type);
        #else
            quantization_uniform_quantizer_quantize((double *) mxGetPr(_INPUT_ELEMENTS), (double *) mxGetPr(_OUTPUT_VALUES), nb_elements, step, saturate, type);
        #endif
    } else {
        _OUTPUT_VALUES = mxCreateDoubleMatrix(mxGetN(_INPUT_ELEMENTS), mxGetM(_INPUT_ELEMENTS), mxCOMPLEX);

        #if MX_HAS_INTERLEAVED_COMPLEX
            quantization_uniform_quantizer_quantize((double *) mxGetComplexDoubles(_INPUT_ELEMENTS), (double *) mxGetComplexDoubles(_OUTPUT_VALUES), 2 * nb_elements, step, saturate, type);
        #else
            quantization_uniform_quantizer_quantize((double *) mxGetPr(_INPUT_ELEMENTS), (double *) mxGetPr(_OUTPUT_VALUES), nb_elements, step, saturate, type);
            quantization_uniform_quantizer_quantize((double *) mxGetPi(_INPUT_ELEMENTS), (double *) mxGetPi(_OUTPUT_VALUES), nb_elements, step, saturate, type);
        #endif
    }
}