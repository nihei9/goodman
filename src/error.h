#ifndef good_ERROR_H
#define good_ERROR_H

typedef enum good_ErrorCode {
    good_ERR_UNKNOWN_ERROR,

    // tokenizer error
    good_ERR_UNCLOSED_STRING,
    good_ERR_EMPTY_STRING,

    good_ERR_CENTINEL,
} good_ErrorCode;

typedef struct good_Error {
    good_ErrorCode code;
} good_Error;

void good_print_error(const good_Error *error);

#endif
