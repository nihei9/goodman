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
};

void good_print_error(const good_Error *error)
{
    good_ErrorMessageAndHint msg = error_messages[error->code];

    printf("ERROR(%d): %s\n", error->code, msg.message);
    if (msg.hint != NULL) {
        printf("HINT: %s\n", msg.hint);
    }
}
