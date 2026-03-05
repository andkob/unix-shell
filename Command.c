#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <readline/history.h>

#include "Command.h"
#include "error.h"

typedef struct {
  char *file;
  char **argv;
  char *filein;
  char *fileout;
} *CommandRep;

#define BIARGS CommandRep r, int *eof, Jobs jobs
#define BINAME(name) bi_##name
#define BIDEFN(name) static void BINAME(name) (BIARGS)
#define BIENTRY(name) {#name,BINAME(name)}

static char *owd=0;
static char *cwd=0;

static int builtin_args(CommandRep r, int n) {
  char **argv=r->argv;
  for (n++; *argv++; n--);
  if (n) {
    WARN("wrong number of arguments to builtin command");
    return 0;
  }
  return 1;
}

BIDEFN(exit) {
  if (!builtin_args(r,0)) return;
  *eof=1;
}

BIDEFN(pwd) {
  if (!builtin_args(r,0)) return;
  if (!cwd)
    cwd=getcwd(0,0);
  printf("%s\n",cwd);
}

BIDEFN(cd) {
  if (!builtin_args(r,1)) return;
  char *dir=r->argv[1];
  unsigned char fail_oldpwd = 0;
  if (strcmp(dir,"-")==0) {
    if (!owd) {
      WARN("cd: OLDPWD not set");
      fail_oldpwd = 1;
    }
    dir=owd;
  }
  if (chdir(dir) && !fail_oldpwd)
    ERROR("chdir() failed");
  if (owd) free(owd);
  owd=cwd;
  cwd=getcwd(0,0);
}

BIDEFN(history) {
  if (!builtin_args(r,0)) return;
  HIST_ENTRY **list=history_list();
  if (list)
    for (int i=0; list[i]; i++)
      printf("%d: %s\n",i+history_base,list[i]->line);
}

static int builtin(BIARGS) {
  typedef struct {
    char *s;
    void (*f)(BIARGS);
  } Builtin;
  static const Builtin builtins[]={
    BIENTRY(exit),
    BIENTRY(pwd),
    BIENTRY(cd),
    BIENTRY(history),
    {0,0}
  };
  int i;
  for (i=0; builtins[i].s; i++)
    if (!strcmp(r->file,builtins[i].s)) {
      builtins[i].f(r,eof,jobs);
      return 1;
    }
  return 0;
}

static char **getargs(T_words words) {
  int n=0;
  T_words p=words;
  while (p) {
    p=p->words;
    n++;
  }
  char **argv=(char **)malloc(sizeof(char *)*(n+1));
  if (!argv)
    ERROR("malloc() failed");
  p=words;
  int i=0;
  while (p) {
    argv[i++]=strdup(p->word->s);
    p=p->words;
  }
  argv[i]=0;
  return argv;
}

extern Command newCommand(T_words words, char *filein, char *fileout) {
  CommandRep r=(CommandRep)malloc(sizeof(*r));
  if (!r)
    ERROR("malloc() failed");
  r->argv=getargs(words);
  r->file=r->argv[0];
  r->filein=filein ? strdup(filein) : 0;
  r->fileout=fileout ? strdup(fileout) : 0;
  return r;
}

static void redirect(CommandRep r) {
  if (r->filein) {
    int fd=open(r->filein,O_RDONLY);
    if (fd<0)
      ERROR("failed to open %s for input",r->filein);
    dup2(fd,STDIN_FILENO);
    close(fd);
  }
  if (r->fileout) {
    int fd=open(r->fileout,O_WRONLY|O_CREAT|O_TRUNC,0644);
    if (fd<0)
      ERROR("failed to open %s for output",r->fileout);
    dup2(fd,STDOUT_FILENO);
    close(fd);
  }
}

static void child(CommandRep r, int fg, int pipein, int pipeout) {
  if (pipein>=0) {
    dup2(pipein,STDIN_FILENO);
    close(pipein);
  }
  if (pipeout>=0) {
    dup2(pipeout,STDOUT_FILENO);
    close(pipeout);
  }
  redirect(r);
  int eof=0;
  Jobs jobs=newJobs();
  if (builtin(r,&eof,jobs))
    exit(0);
  execvp(r->argv[0],r->argv);
  ERROR("execvp() failed");
  exit(0);
}

extern int execCommand(Command command, Pipeline pipeline, Jobs jobs,
		       int *jobbed, int *eof, int fg, int pipein, int pipeout) {
  CommandRep r=command;
  if (fg && pipein<0 && pipeout<0 && builtin(r,eof,jobs))
    return 0;
  if (!*jobbed) {
    *jobbed=1;
    addJobs(jobs,pipeline);
  }
  int pid=fork();
  if (pid==-1)
    ERROR("fork() failed");
  if (pid==0)
    child(r,fg,pipein,pipeout);
  return pid;
}

extern void freeCommand(Command command) {
  CommandRep r=command;
  char **argv=r->argv;
  while (*argv)
    free(*argv++);
  free(r->argv);
  if (r->filein) free(r->filein);
  if (r->fileout) free(r->fileout);
  free(r);
}

extern void freestateCommand() {
  if (cwd) free(cwd);
  if (owd) free(owd);
}
