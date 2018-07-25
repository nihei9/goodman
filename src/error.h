#ifndef good_ERROR_H
#define good_ERROR_H

typedef enum good_ErrorCode {
    good_ERR_UNKNOWN_ERROR,
    good_ERR_UNIMPLEMENTED_FEATURE,

    // tokenizer error
    good_ERR_UNKNOWN_TOKEN,
    good_ERR_UNCLOSED_STRING,
    good_ERR_EMPTY_STRING,

    // parser error
    good_ERR_UNTERMINATED_PRULE,
    good_ERR_MISSING_PRULE_LEADER,
    good_ERR_UNEXPECTED_TOKEN_IN_PRULE,

    good_ERR_CENTINEL,
} good_ErrorCode;

typedef struct good_Error {
    good_ErrorCode code;
} good_Error;

void good_print_error(const good_Error *error);

#endif
