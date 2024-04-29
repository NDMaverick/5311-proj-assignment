/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#include "verbosity.h"
#include "stdio.h"
#include <stdarg.h>

static VerbosityLevel verbosity=NORMAL;

void set_verbosity(VerbosityLevel level) {
    verbosity = level;
}

VerbosityLevel get_verbosity() {
    return verbosity;
}

void print_verbosity(VerbosityLevel level, const char* format, ...) {
    if (verbosity <= level) { // I want to print if the verbosity level is less or equal to the level of the message (A less verbosity means more verbose, since DEBUG<NORMAL<SILENT)
        va_list args;
        switch (level) {
            case DEBUG:
                printf("[DEBUG] - ");
                break;
            case NORMAL:
                printf("[NORMAL] - ");
                break;
            case SILENT:
                printf("[SILENT] - ");
                break;
        }
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }
}
