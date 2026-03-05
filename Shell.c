#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "Jobs.h"
#include "Parser.h"
#include "Interpreter.h"
#include "error.h"

/* ── ANSI color support ────────────────────────────────────── */

static int colors_supported(void) {
  if (!isatty(STDOUT_FILENO))
    return 0;
  const char *term = getenv("TERM");
  if (!term || strcmp(term, "dumb") == 0)
    return 0;
  /* most modern terminals advertise *color* somewhere in TERM */
  if (strstr(term, "color") || strstr(term, "xterm") ||
      strstr(term, "screen") || strstr(term, "tmux") ||
      strstr(term, "rxvt") || strstr(term, "linux") ||
      strstr(term, "ansi") || strstr(term, "256color") ||
      getenv("COLORTERM"))
    return 1;
  return 1; /* optimistic default for unknown modern terms */
}

/* ── SIGINT handler (Ctrl-C gives a new prompt, not death) ── */

static void sigint_handler(int sig) {
  (void)sig;
  write(STDOUT_FILENO, "\n", 1);
  rl_on_new_line();
  rl_replace_line("", 0);
  rl_redisplay();
}

/* ── helpers ───────────────────────────────────────────────── */

static void trim_newline(char *s) {
  size_t len = strlen(s);
  while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r'))
    s[--len] = '\0';
}

/* ── CPU model name from /proc/cpuinfo ─────────────────────── */

static void get_cpu_model(char *buf, size_t sz) {
  buf[0] = '\0';
  FILE *f = fopen("/proc/cpuinfo", "r");
  if (!f) return;
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    if (strncmp(line, "model name", 10) == 0) {
      char *p = strchr(line, ':');
      if (p) {
        p++;                       /* skip ':' */
        while (*p == ' ') p++;     /* skip leading spaces */
        strncpy(buf, p, sz - 1);
        buf[sz - 1] = '\0';
        trim_newline(buf);
      }
      break;
    }
  }
  fclose(f);
}

/* ── print system info ─────────────────────────────────────── */

static void print_system_info(int use_color) {
  /* ANSI codes (only used when color is supported) */
  const char *BOLD  = use_color ? "\033[1m"    : "";
  const char *CYAN  = use_color ? "\033[36m"   : "";
  const char *GREEN = use_color ? "\033[32m"   : "";
  const char *RESET = use_color ? "\033[0m"    : "";
  const char *DIM   = use_color ? "\033[2m"    : "";

  struct utsname u;
  uname(&u);

  struct sysinfo si;
  sysinfo(&si);

  char *user = getenv("USER");
  if (!user) user = "unknown";

  char cpu[256];
  get_cpu_model(cpu, sizeof(cpu));
  if (!cpu[0]) strcpy(cpu, "unknown");

  long updays  = si.uptime / 86400;
  long uphours = (si.uptime % 86400) / 3600;
  long upmins  = (si.uptime % 3600) / 60;

  unsigned long total_mb = si.totalram / (1024 * 1024);
  unsigned long free_mb  = si.freeram  / (1024 * 1024);
  unsigned long used_mb  = total_mb - free_mb;

  /* Date & time */
  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);
  char timebuf[64];
  strftime(timebuf, sizeof(timebuf), "%a %b %d %Y, %I:%M %p", tm_now);

  printf("\n");
  printf("  %s%sDate%s      %s%s%s\n",    BOLD, CYAN, RESET, GREEN, timebuf, RESET);
  printf("  %s%sUser%s      %s%s%s\n",    BOLD, CYAN, RESET, GREEN, user, RESET);
  printf("  %s%sHost%s      %s%s%s\n",    BOLD, CYAN, RESET, GREEN, u.nodename, RESET);
  printf("  %s%sKernel%s    %s%s %s%s\n", BOLD, CYAN, RESET, GREEN, u.sysname, u.release, RESET);
  printf("  %s%sArch%s      %s%s%s\n",    BOLD, CYAN, RESET, GREEN, u.machine, RESET);
  printf("  %s%sUptime%s    %s%ldd %ldh %ldm%s\n", BOLD, CYAN, RESET, GREEN, updays, uphours, upmins, RESET);
  printf("  %s%sMemory%s    %s%lu MB / %lu MB%s\n", BOLD, CYAN, RESET, GREEN, used_mb, total_mb, RESET);
  printf("  %s%sCPU%s       %s%s%s\n",    BOLD, CYAN, RESET, GREEN, cpu, RESET);
  printf("  %s%sShell%s     %s%s%s\n",    BOLD, CYAN, RESET, GREEN, "melonsh", RESET);
  printf("  %s%sColors%s    %s%s%s\n",    BOLD, CYAN, RESET, GREEN, use_color ? "yes" : "no", RESET);
  printf("  %s%s────────────────────────────────%s\n", DIM, CYAN, RESET);
  printf("\n");
}

/* ── time-of-day greeting ──────────────────────────────────── */

static void print_greeting(int use_color) {
  const char *BOLD   = use_color ? "\033[1m"  : "";
  const char *YELLOW = use_color ? "\033[33m" : "";
  const char *RESET  = use_color ? "\033[0m"  : "";

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int hour = t->tm_hour;

  const char *greeting;
  if (hour < 12)      greeting = "Good morning";
  else if (hour < 17) greeting = "Good afternoon";
  else                 greeting = "Good evening";

  char *user = getenv("USER");
  if (!user) user = "user";

  printf("  %s%s%s, %s!%s\n", BOLD, YELLOW, greeting, user, RESET);
}

/* ── ASCII art banner (randomly selected from ascii/) ──────── */

#define NUM_BANNERS 11

static void print_banner(int use_color) {
  const char *BOLD  = use_color ? "\033[1m"  : "";
  const char *CYAN  = use_color ? "\033[36m" : "";
  const char *RESET = use_color ? "\033[0m"  : "";

  /* Resolve path relative to the executable */
  char exe_path[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if (len <= 0) return;
  exe_path[len] = '\0';
  char *dir = dirname(exe_path);

  srand(time(NULL));
  int pick = (rand() % NUM_BANNERS) + 1; /* 1..11 */

  char filepath[PATH_MAX];
  snprintf(filepath, sizeof(filepath), "%s/ascii/%d.txt", dir, pick);

  FILE *f = fopen(filepath, "r");
  if (!f) return;

  printf("%s%s\n", BOLD, CYAN);
  char line[1024];
  while (fgets(line, sizeof(line), f))
    printf("  %s", line);
  printf("%s\n", RESET);

  fclose(f);
}

/* ── colorized prompt (user@host:~/cwd$) ───────────────────── */

static char *build_prompt(int use_color) {
  static char buf[PATH_MAX + 512];
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd)))
    strcpy(cwd, "?");

  /* Replace $HOME prefix with ~ */
  char display_cwd[PATH_MAX];
  char *home = getenv("HOME");
  if (home && strncmp(cwd, home, strlen(home)) == 0)
    snprintf(display_cwd, sizeof(display_cwd), "~%s", cwd + strlen(home));
  else
    strncpy(display_cwd, cwd, sizeof(display_cwd));

  char host[256];
  gethostname(host, sizeof(host));
  char *user = getenv("USER");
  if (!user) user = "unknown";

  if (use_color) {
    /* \001/\002 = readline markers for non-printing chars */
    snprintf(buf, sizeof(buf),
      "\001\033[1;32m\002%s@%s\001\033[0m\002:"
      "\001\033[1;34m\002%s\001\033[0m\002$ ",
      user, host, display_cwd);
  } else {
    snprintf(buf, sizeof(buf), "%s@%s:%s$ ", user, host, display_cwd);
  }
  return buf;
}

/* ── goodbye message ───────────────────────────────────────── */
#define NUM_MSGS 5

static void print_goodbye(int use_color) {
  char *msgs[] = {"Later!", "Peace!", "See ya!", "Catch ya later sugarplum ;)", "Ight I see how it is. Bye then."};
  srand(time(NULL));
  int pick = (rand() % NUM_MSGS);

  const char *BOLD   = use_color ? "\033[1m"  : "";
  const char *YELLOW = use_color ? "\033[33m" : "";
  const char *RESET  = use_color ? "\033[0m"  : "";
  printf("\n  %s%s%s%s\n\n", BOLD, YELLOW, msgs[pick], RESET);
}

/* ── main ──────────────────────────────────────────────────── */

int main() {
  int eof=0;
  Jobs jobs=newJobs();
  int interactive = isatty(fileno(stdin));
  int use_color = 0;

  if (interactive) {
    using_history();
    read_history(".history");

    use_color = colors_supported();
    print_banner(use_color);
    print_greeting(use_color);
    print_system_info(use_color);

    signal(SIGINT, sigint_handler);
  } else {
    rl_bind_key('\t',rl_insert);
    rl_outstream=fopen("/dev/null","w");
  }

  while (!eof) {
    while (waitpid(-1,NULL,WNOHANG)>0);
    char *prompt = interactive ? build_prompt(use_color) : NULL;
    char *line=readline(prompt);
    if (!line)
      break;
    if (*line)
      add_history(line);
    Tree tree=parseTree(line);
    free(line);
    interpretTree(tree,&eof,jobs);
    freeTree(tree);
  }

  if (interactive) {
    print_goodbye(use_color);
    write_history(".history");
    rl_clear_history();
  } else {
    fclose(rl_outstream);
  }
  freeJobs(jobs);
  freestateCommand();
  return 0;
}
