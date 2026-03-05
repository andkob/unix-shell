#ifndef PIPELINE_H
#define PIPELINE_H

typedef void *Pipeline;

#include "Command.h"
#include "Jobs.h"

/**
 * Create a new, empty Pipeline.
 *
 * @param fg Non-zero for a foreground pipeline, 0 for background ("&").
 * @return   Opaque Pipeline handle.
 */
extern Pipeline newPipeline(int fg);

/**
 * Append a Command to the end of a pipeline.
 *
 * @param pipeline The Pipeline to append to.
 * @param command  The Command to add.
 */
extern void addPipeline(Pipeline pipeline, Command command);

/**
 * Return the number of commands in a pipeline.
 *
 * @param pipeline The Pipeline to query.
 * @return         Number of commands.
 */
extern int sizePipeline(Pipeline pipeline);

/**
 * Execute all commands in the pipeline, connecting them with pipes,
 * and wait for completion if the pipeline is foreground.
 *
 * @param pipeline The Pipeline to execute.
 * @param jobs     The global Jobs list for job tracking.
 * @param eof      In/out flag: set to 1 if "exit" is encountered.
 */
extern void execPipeline(Pipeline pipeline, Jobs jobs, int *eof);

/**
 * Free the Pipeline and all of its constituent Commands.
 *
 * @param pipeline The Pipeline to free.
 */
extern void freePipeline(Pipeline pipeline);

#endif
