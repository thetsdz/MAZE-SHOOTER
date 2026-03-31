/**
 * \file log.c
 */

#include "../lib/headers/log.h"

#include <stdio.h>

static FILE *logFile = NULL;

int InitLog(const char *filename) {
    logFile = fopen(filename, "w");
    return (logFile != NULL) ? 1 : 0;
}

void CloseLog(void) {
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
}

void LogToFile(int logLevel, const char *text, va_list args) {
    if (!logFile)
        return;
    const char *levels[] = {"ALL",     "TRACE", "DEBUG", "INFO",
                            "WARNING", "ERROR", "FATAL", "NONE"};
    fprintf(logFile, "[%s] ", levels[logLevel]);
    vfprintf(logFile, text, args);
    fprintf(logFile, "\n");
    fflush(logFile);
}
