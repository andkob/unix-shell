#ifndef SEQUENCE_H
#define SEQUENCE_H

typedef void *Sequence;

#include "Jobs.h"
#include "Pipeline.h"

/**
 * Create a new, empty Sequence backed by a deque.
 *
 * @return Opaque Sequence handle.
 */
extern Sequence newSequence();

/**
 * Append a Pipeline to the sequence.
 *
 * @param sequence The Sequence to append to.
 * @param pipeline The Pipeline to add.
 */
extern void addSequence(Sequence sequence, Pipeline pipeline);

/**
 * Free the Sequence and all of its constituent Pipelines.
 *
 * @param sequence The Sequence to free.
 */
extern void freeSequence(Sequence sequence);

/**
 * Execute every Pipeline in the sequence in order, stopping early
 * if eof is set (e.g., by the "exit" builtin).  Frees the Sequence
 * when done.
 *
 * @param sequence The Sequence to execute.
 * @param jobs     The global Jobs list.
 * @param eof      In/out flag: set to 1 on "exit".
 */
extern void execSequence(Sequence sequence, Jobs jobs, int *eof);

#endif
