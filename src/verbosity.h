/*
 * Created by Nikos Ntokos on 24/4/24.
 * Copyright (c) 2024, Nikos Ntokos
 * All rights reserved.
 */

#ifndef VERBOSITY_H
#define VERBOSITY_H

/**
 * @brief The verbosity level of the program\n
 * {DEBUG, NORMAL, SILENT}\n
 * Default: NORMAL\n
 * To change the verbosity level, use set_verbosity()
 */
typedef enum {
    DEBUG,
    NORMAL,
    SILENT
} VerbosityLevel;

/**
 * Set the verbosity level of the program
 * @param level The verbosity level to set {DEBUG, NORMAL, SILENT}
 */
void set_verbosity(VerbosityLevel level);

/**
 * Get the verbosity level of the program
 * @param level The verbosity level to set {DEBUG, NORMAL, SILENT}
 * @return The current verbosity level
 */
VerbosityLevel get_verbosity();

/**
 * Print a message if the verbosity level is equal or higher than the given level
 * @param level The verbosity level of the message
 * @param message The message to print
 */
void print_verbosity(VerbosityLevel level, const char* format, ...);

#endif //VERBOSITY_H
