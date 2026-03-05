#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Parser.h"
#include "Tree.h"
#include "Jobs.h"

/**
 * Walk a parse tree, converting it into executable Sequence/Pipeline/Command
 * objects, and execute them.
 *
 * @param t    Root of the parse tree (T_sequence) returned by parseTree().
 * @param eof  In/out flag: set to 1 if an "exit" builtin is encountered.
 * @param jobs The global Jobs list for background-job tracking.
 */
extern void interpretTree(Tree t, int *eof, Jobs jobs);

#endif
