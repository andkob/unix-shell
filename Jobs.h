#ifndef JOBS_H
#define JOBS_H

typedef void *Jobs;

#include "Pipeline.h"

/**
 * Create an empty Jobs collection backed by a deque.
 *
 * @return Opaque Jobs handle.
 */
extern Jobs newJobs();

/**
 * Append a pipeline to the jobs list.
 *
 * @param jobs     The Jobs collection.
 * @param pipeline The Pipeline to add.
 */
extern void addJobs(Jobs jobs, Pipeline pipeline);

/**
 * Return the number of pipelines currently tracked.
 *
 * @param jobs The Jobs collection.
 * @return     Number of tracked pipelines.
 */
extern int sizeJobs(Jobs jobs);

/**
 * Free the Jobs collection and every Pipeline it contains.
 *
 * @param jobs The Jobs collection to free.
 */
extern void freeJobs(Jobs jobs);

#endif
