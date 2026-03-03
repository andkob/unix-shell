#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Pipeline.h"
#include "deq.h"
#include "error.h"

typedef struct {
  Deq processes;
  int fg;			// not "&"
} *PipelineRep;

extern Pipeline newPipeline(int fg) {
  PipelineRep r=(PipelineRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->processes=deq_new();
  r->fg=fg;
  return r;
}

extern void addPipeline(Pipeline pipeline, Command command) {
  PipelineRep r=(PipelineRep)pipeline;
  deq_tail_put(r->processes,command);
}

extern int sizePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  return deq_len(r->processes);
}

static void execute(Pipeline pipeline, Jobs jobs, int *jobbed, int *eof) {
  PipelineRep r=(PipelineRep)pipeline;
  int pids[512];
  int npids=0;
  for (int i=0; i<sizePipeline(r) && !*eof; i++) {
    int pid=execCommand(deq_head_ith(r->processes,i),pipeline,jobs,jobbed,eof,r->fg);
    if (pid>0)
      pids[npids++]=pid;
  }
  if (r->fg)
    for (int i=0; i<npids; i++)
      waitpid(pids[i],NULL,0);
}

extern void execPipeline(Pipeline pipeline, Jobs jobs, int *eof) {
  int jobbed=0;
  execute(pipeline,jobs,&jobbed,eof);
  if (!jobbed)
    freePipeline(pipeline);	// for fg builtins, and such
}

extern void freePipeline(Pipeline pipeline) {
  PipelineRep r=(PipelineRep)pipeline;
  deq_del(r->processes,freeCommand);
  free(r);
}
