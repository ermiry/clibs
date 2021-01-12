#include "clibs/version.h"

#include "clibs/utils/log.h"

// print full erver version information 
void clibs_version_print_full (void) {

    clibs_log_both (
        LOG_TYPE_NONE, LOG_TYPE_NONE,
        "\nClibs Version: %s",
        CLIBS_VERSION_NAME
    );

    clibs_log_both (
        LOG_TYPE_NONE, LOG_TYPE_NONE,
        "Release Date & time: %s - %s",
        CLIBS_VERSION_DATE, CLIBS_VERSION_TIME
    );

    clibs_log_both (
        LOG_TYPE_NONE, LOG_TYPE_NONE,
        "Author: %s\n",
        CLIBS_VERSION_AUTHOR
    );

}

// print the version id
void clibs_version_print_version_id (void) {

    clibs_log_both (
        LOG_TYPE_NONE, LOG_TYPE_NONE,
        "\nClibs Version ID: %s\n",
        CLIBS_VERSION
    );

}

// print the version name
void clibs_version_print_version_name (void) {

    clibs_log_both (
        LOG_TYPE_NONE, LOG_TYPE_NONE,
        "\nClibs Version: %s\n",
        CLIBS_VERSION_NAME
    );

}