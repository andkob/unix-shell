#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

/**
 * WARNLOC - Print a warning message to stderr with file and line information,
 * then flush stderr.  Does not terminate the program.
 *
 * @param file Source filename (typically __FILE__).
 * @param line Source line number (typically __LINE__).
 * @param kind Label string (e.g. "warning").
 * @param args printf-style format string and arguments.
 */
#define WARNLOC(file,line,kind,args...) do {  \
  fprintf(stderr,"%s:%d: ",file,line);        \
  fprintf(stderr,"%s: ",kind);                \
  fprintf(stderr,args);                       \
  fprintf(stderr,"\n");                       \
  fflush(stderr);                             \
} while (0)

/**
 * ERRORLOC - Print an error message to stderr with file and line information,
 * then terminate the program with exit(1).
 *
 * @param file Source filename (typically __FILE__).
 * @param line Source line number (typically __LINE__).
 * @param kind Label string (e.g. "error").
 * @param args printf-style format string and arguments.
 */
#define ERRORLOC(file,line,kind,args...) do { \
  WARNLOC(file,line,kind,args);               \
  exit(1);                                    \
} while (0)

/**
 * WARN - Convenience macro: prints a warning with automatic file/line.
 *
 * @param args printf-style format string and arguments.
 */
#define WARN(args...) WARNLOC(__FILE__,__LINE__,"warning",args)

/**
 * ERROR - Convenience macro: prints an error with automatic file/line
 * and terminates the program.
 *
 * @param args printf-style format string and arguments.
 */
#define ERROR(args...) ERRORLOC(__FILE__,__LINE__,"error",args)

#endif
