#include "../../concatenate.h"

#include "../../../interfaces/matlab/mex_utils.h"
#include <limits.h>

#include "mex.h"

#define EPSILON 1e-9

#define _INPUT_ELEMENTS prhs[0]
#define _INPUT_DEPTHS prhs[1]

#define _OUTPUT_VALUES plhs[0]

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    /**
        Parameters:
            - prhs[0]: (mxDouble array) Elements to concatenate
            - prhs[1]: (mxDouble array) Number of bits/depth of each elements. This vector must contains interger values. If not, the nearest value is used
        Output:
            - plhs[0]: (mxDouble) Array containing the concatenated values
    */
    
    // ------------------------------------------------------------------
    // Check input types
    // ------------------------------------------------------------------

    MEX_CHECK_NB_ARGUMENTS(nrhs, 2);

    MEX_CHECK_IS_DOUBLE(_INPUT_ELEMENTS, "elements");
    MEX_CHECK_IS_VECTOR(_INPUT_ELEMENTS, "elements");

    MEX_CHECK_IS_DOUBLE(_INPUT_DEPTHS, "depth");
    MEX_CHECK_IS_VECTOR(_INPUT_ELEMENTS, "depth");

    if(CHAR_BIT != 8)
        MEX_ERROR_CREATE_ERROR_ID_TXT(
            "badCharSize", 
            "A char is not on 8 bits on your system. This could cause issue."
        );

    // ------------------------------------------------------------------
    // Initialize variables
    // ------------------------------------------------------------------

    int i;
    unsigned int nb_elements = (unsigned int) mxGetNumberOfElements(_INPUT_ELEMENTS);
    unsigned int nb_depths = (unsigned int) mxGetNumberOfElements(_INPUT_DEPTHS);

    unsigned int * temp_input_elements = malloc(nb_elements * sizeof(unsigned int));
    unsigned char * temp_input_depths = malloc(nb_elements * sizeof(unsigned char));
    
    // Check dimensions of the input matrix
    if((nb_depths != 1) && (nb_elements != nb_depths))
        MEX_ERROR_CREATE_ERROR_ID_TXT(
            "badSize", 
            "You must specify a single depth value or an array with the same size as the first parameter"
        );
    
    // ------------------------------------------------------------------
    // Manage input data
    // ------------------------------------------------------------------
    
    OperationMode mode = (nb_depths == 1) ? SINGLE : ARRAY;

    #if MX_HAS_INTERLEAVED_COMPLEX
        double * elements = (double *) mxGetDoubles(_INPUT_ELEMENTS);
        double * depths = (double *) mxGetDoubles(_INPUT_DEPTHS);
    #else
        double * elements = (double *) mxGetPr(_INPUT_ELEMENTS);
        double * depths = (double *) mxGetPr(_INPUT_DEPTHS);
    #endif

    // Process input data
    for(i = 0; i < nb_elements; i++) {
        // We add EPSILON to be sure the double value is correctly converted into the corresponding integer
        temp_input_depths[i] = (unsigned char) ((mode == SINGLE) ? (round(*depths) + EPSILON) : (round(depths[i]) + EPSILON));
        temp_input_elements[i] = ((unsigned int) (round(elements[i]) + EPSILON)) & concatenate_tool_create_mask(0, (int) temp_input_depths[i]);
    }

    // Generate output row vector
    int output_nb_elements = (int) concatenate_tool_get_output_size(temp_input_depths, nb_elements, mode);

    if (mxGetN(_INPUT_ELEMENTS) == 1)
        _INPUT_DEPTHS = mxCreateNumericMatrix(output_nb_elements, 1, mxUINT8_CLASS, mxREAL);
    else
        _INPUT_DEPTHS = mxCreateNumericMatrix(1, output_nb_elements, mxUINT8_CLASS, mxREAL);
    
    #if MX_HAS_INTERLEAVED_COMPLEX
        unsigned char * output_elements = (unsigned char *) mxGetUint8s(_INPUT_DEPTHS);
    #else
        unsigned char * output_elements = (unsigned char *) mxGetData(_INPUT_DEPTHS);
    #endif

    // ------------------------------------------------------------------
    // Processing
    // ------------------------------------------------------------------

    concatenate_tool_concatenate(temp_input_elements, temp_input_depths, nb_elements, output_elements, output_nb_elements, ARRAY);

    // ------------------------------------------------------------------
    // Post-processing
    // ------------------------------------------------------------------

    free(temp_input_elements);
    free(temp_input_depths);
}