#ifndef USEFUL_LIB_MEX_UTILS_H
#define USEFUL_LIB_MEX_UTILS_H

#include "mex.h"
#include <stdbool.h>
#include <stdlib.h>

// ------------------------------------------------------------------
// Functions used to Pause/Stop a MEX process
// ------------------------------------------------------------------

// Source: https://undocumentedmatlab.com/articles/mex-ctrl-c-interrupt
#if defined (_WIN32)
    #include <windows.h>
#elif defined (__linux__)
    #include <unistd.h>
#endif

#ifdef __cplusplus
    extern "C" bool utIsInterruptPending();
    extern "C" bool utSetInterruptPending(bool);
#else
    extern bool utIsInterruptPending();
    extern bool utSetInterruptPending(bool);
#endif

// ------------------------------------------------------------------
// Useful macros
// ------------------------------------------------------------------

#define MEX_ERROR_MESSAGE_NOT_SUPPORTED_INTERLEAVED_API "This MEX file has been compiled using interleaved complex API, but this feature is not supported currently by the script"

#ifndef MEX_ERROR_CATEGORY_MAIN_CATEGORY
    #define MEX_ERROR_CATEGORY_MAIN_CATEGORY "MyToolbox"
#endif

#ifndef MEX_ERROR_CATEGORY_DEFAULT
    #define MEX_ERROR_CATEGORY_DEFAULT "Error"
#endif

#ifndef MEX_ERROR_CATEGORY_NOT_ENOUGH_INPUT_ARGUMENTS
    #define MEX_ERROR_CATEGORY_NOT_ENOUGH_INPUT_ARGUMENTS "NotEnoughInputArgument"
#endif

#ifndef MEX_ERROR_CATEGORY_BAD_TYPE_ARGUMENT
    #define MEX_ERROR_CATEGORY_BAD_TYPE_ARGUMENT "BadTypeArgument"
#endif

#ifndef MEX_ERROR_CREATE_ERROR_MESSAGE
    #define MEX_ERROR_CREATE_CATEGORY(category_name) (MEX_ERROR_CATEGORY_MAIN_CATEGORY ":" category_name)
#endif

#ifndef MEX_ERROR_CREATE_ERROR_MESSAGE
    #define MEX_ERROR_CREATE_ERROR_MESSAGE(message, ...) __FILE__": <strong>MEX ERROR:</strong>"__LINE__" " message, __VA_ARGS__
#endif

#define MEX_ERROR_CREATE_ERROR_ID_TXT(category, message, ...) {\
    mexErrMsgIdAndTxt(\
        MEX_ERROR_CREATE_CATEGORY(category),\
        MEX_ERROR_CREATE_ERROR_MESSAGE(message, __VA_ARGS__)\
    );\
}

#define MEX_ERROR_CREATE_ERROR_TXT(message, ...) {\
    MEX_ERROR_CREATE_ERROR_ID_TXT(MEX_ERROR_CATEGORY_DEFAULT, message, __VA_ARGS__);\
}

#define MEX_CHECK_NB_ARGUMENTS(nb_args, nb_min) {\
    if (nb_args < nb_min) {\
        MEX_ERROR_CREATE_ERROR_ID_TXT(\
            MEX_ERROR_CATEGORY_NOT_ENOUGH_INPUT_ARGUMENTS, \
            "Not enough input arguments. This function require at least %d arguments [%d given]", nb_min, nb_args\
        );\
    }\
}

#define _MEX_CHECK_ARGUMENT_TYPE(condition, message, ...) {\
    if (condition) {\
        mexErrMsgIdAndTxt(\
            MEX_ERROR_CREATE_CATEGORY(MEX_ERROR_CATEGORY_BAD_TYPE_ARGUMENT),\
            MEX_ERROR_CREATE_ERROR_MESSAGE(message, __VA_ARGS__) \
        );\
    }\
}

// ------------------------------------------------------------------
// Checks and conditions
// ------------------------------------------------------------------

#define MEX_CONDITION_IS_COMPLEX(elt) (mxIsComplex(elt))
#define MEX_CHECK_IS_COMPLEX(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_COMPLEX(elt), "Argument \"%s\" must be a complex.", argument_name)
#define MEX_CHECK_IS_NOT_COMPLEX(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(MEX_CONDITION_IS_COMPLEX(elt), "Argument \"%s\" must be a real number. Complex given", argument_name)

#define MEX_CONDITION_IS_DOUBLE(elt) ((mxIsDouble(elt)) && (!mxIsComplex(elt)))
#define MEX_CHECK_IS_DOUBLE(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_DOUBLE(elt), "Argument \"%s\" must be a double.", argument_name)

#define MEX_CONDITION_IS_CLASS_STR_ID(elt, category_str) (mxIsClass(elt, category_str)) // (mxGetClassID(elt) != (mxClassID) category)
#define MEX_CHECK_IS_CLASS_STR_ID(elt, category_str, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_CLASS_STR_ID(elt, category_str), "Argument \"%s\" should be of class \"%s\" but it is of class \"%s\".", argument_name, category_str, mxGetClassName(elt))

#define MEX_CONDITION_IS_LOGICAL(elt) (mxIsLogical(elt))
#define MEX_CHECK_IS_LOGICAL(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_LOGICAL(elt), "Argument \"%s\" must be a logical/boolean value.", argument_name)

#define MEX_CONDITION_IS_SCALAR(elt) (mxIsScalar(elt))
#define MEX_CHECK_IS_SCALAR(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_SCALAR(elt), "Argument \"%s\" must be a scalar. The given argument has %d rows and %d cols", argument_name, (int) mxGetM(elt), (int) mxGetN(elt))

#define MEX_CONDITION_IS_EMPTY(elt) (!mxIsEmpty(elt))
#define MEX_CHECK_IS_NOT_EMPTY(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(MEX_CONDITION_IS_EMPTY(elt), "Argument \"%s\" is empty", argument_name)

#define MEX_CONDITION_IS_VECTOR(elt) (((mxGetN(elt) >= 1) && (mxGetM(elt) == 1)) || ((mxGetN(elt) == 1) && (mxGetM(elt) >= 1)) || MEX_CONDITION_IS_EMPTY(elt)) // As in Matlab, empty matrix/vector are valid
#define MEX_CHECK_IS_VECTOR(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_VECTOR(elt), "Argument \"%s\" must be a single row vector. The given argument has %d rows and %d cols", argument_name, (int) mxGetM(elt), (int) mxGetN(elt))

#define MEX_CONDITION_IS_MATRIX(elt) ((mxGetN(elt) >= 1) && (mxGetM(elt) >= 1) || MEX_CONDITION_IS_EMPTY(elt)) // As in Matlab, empty matrix/vector are valid
#define MEX_CHECK_IS_MATRIX(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_MATRIX(elt), "Argument \"%s\" must be a matrix. The given argument has %d rows and %d cols", argument_name, (int) mxGetM(elt), (int) mxGetN(elt))

#define MEX_CONDITION_IS_SQUARE_MATRIX(elt) (mxGetN(elt) == mxGetM(elt))
#define MEX_CHECK_IS_SQUARE_MATRIX(elt, argument_name) {\
    MEX_CHECK_IS_MATRIX(elt, argument_name);\
    _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_SQUARE_MATRIX(elt), "Argument \"%s\" must be a quare matrix. The given matrix has %d rows and %d cols", argument_name, (int) mxGetM(elt), (int) mxGetN(elt));\
}

#define MEX_CONDITION_IS_SIZED_MATRIX(elt, nb_rows, nb_cols) (mxGetN(elt) == nb_rows && mxGetM(elt) == nb_cols)
#define MEX_CHECK_IS_SIZED_MATRIX(elt, nb_rows, nb_cols, argument_name) {\
    MEX_CHECK_IS_MATRIX(elt, argument_name);\
    _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_SIZED_MATRIX(elt, nb_rows, nb_cols), "Argument \"%s\" must be a matrix with %d rows and %d cols. The given matrix has %d rows and %d cols", argument_name, (int) nb_rows, (int) nb_cols, (int) mxGetM(elt), (int) mxGetN(elt));\
}

#define MEX_CONDITION_IS_STRING(elt) (MEX_CONDITION_IS_VECTOR(elt) && mxIsChar(elt))
#define MEX_CHECK_IS_STRING(elt, argument_name) _MEX_CHECK_ARGUMENT_TYPE(!MEX_CONDITION_IS_STRING(elt), "Argument \"%s\" must be a string", argument_name)

#endif // USEFUL_LIB_MEX_UTILS_H