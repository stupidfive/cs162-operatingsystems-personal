#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>


int main() {
  struct rlimit lim;
  printf("stack size: %d\n", getrlimit(RLIMIT_STACK, &lim));
  printf("process limit: %d\n", getrlimit(RLIMIT_NPROC, &lim));
  printf("max file descriptors: %d\n", getrlimit(RLIMIT_NOFILE, &lim));
}
