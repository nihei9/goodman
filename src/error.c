#include "error.h"
#include <stdio.h>

typedef struct good_ErrorMessageAndHint {
    const char *message;
    const char *hint;
} good_ErrorMessageAndHint;

static const good_ErrorMessageAndHint error_messages[good_ERR_CENTINEL] = {
    // UNKNOWN_ERROR
    {
        "Internal error occurred.",
        NULL,
    },
    // UNIMPLEMENTED_FEATURE
    {
        "This feature is unimplemented.",
        NULL,
    },

    // UNKNOWN_TOKEN
    {
        "Unknown token is detected.",
        NULL,
    },
    // UNCLOSED_STRING
    {
        "String is unclosed.",
        "String should be surrounded by ' sign.",
    },
    // EMPTY_STRING
    {
        "String is empty.",
        "String expresses a terminal symbol, so empty string is not allowed.",
    },

    // UNTERMINATED_PRULE
    {
        "Production rule is unterminated.",
        "Production rule should be terminated by ; sign.",
    },
    // MISSING_PRULE_LEADER
    {
        "Missing : sign.",
        NULL,
    },
    // UNEXPECTED_TOKEN_IN_PRULE
    {
        "Unexpected token was detected in a production rule.",
        NULL,
    },
    // UNCLOSED_GROUP
    {
        "Group is unclosed.",
        "Group should be surrounded by () sign.",
    },
};

void good_print_error(const good_Error *error)
{
    good_ErrorMessageAndHint msg = error_messages[error->code];

    printf("ERROR: %s\n", msg.message);
    if (msg.hint != NULL) {
        printf("HINT: %s\n", msg.hint);
    }
}
