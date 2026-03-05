#ifndef COMMAND_H
#define COMMAND_H

typedef void *Command;

#include "Tree.h"
#include "Jobs.h"
#include "Sequence.h"

/**
 * Create a new Command from parsed words and optional I/O redirection files.
 *
 * @param words   Linked list of parsed words (argv); first word is the command name.
 * @param filein  Filename for input redirection, or NULL for stdin.
 * @param fileout Filename for output redirection, or NULL for stdout.
 * @return        Opaque Command handle.
 */
extern Command newCommand(T_words words, char *filein, char *fileout);

/**
 * Execute a command, forking a child process unless it is a foreground builtin
 * with no pipe redirection.  Handles pipe plumbing and builtin dispatch.
 *
 * @param command  The Command to execute.
 * @param pipeline The Pipeline this command belongs to (used for job tracking).
 * @param jobs     The global Jobs list.
 * @param jobbed   In/out flag: set to 1 once the pipeline is added to jobs.
 * @param eof      In/out flag: set to 1 if the "exit" builtin is invoked.
 * @param fg       Non-zero if the command runs in the foreground.
 * @param pipein   Read end of the incoming pipe, or -1 if none.
 * @param pipeout  Write end of the outgoing pipe, or -1 if none.
 * @return         PID of the forked child, or 0 if handled as a foreground builtin.
 */
extern int execCommand(Command command, Pipeline pipeline, Jobs jobs,
		       int *jobbed, int *eof, int fg, int pipein, int pipeout);

/**
 * Free all memory associated with a Command (argv strings, redirection
 * filenames, and the Command struct itself).
 *
 * @param command The Command to free.
 */
extern void freeCommand(Command command);

/**
 * Free module-level state (current and old working directory strings).
 * Call once at shell shutdown.
 */
extern void freestateCommand();

#endif
