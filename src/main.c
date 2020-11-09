#include "parent_child.h"
#include <time.h>

#define N0DES 5

int main(void)
{

  Link mesh[2 * (square(N0DES) - N0DES)];

  int links = CreateMesh(mesh, N0DES);

  printf("Mesh initialized..\n");
  printf("\n");
  
  int pid;

  for (int qid = 1; qid < N0DES; qid++)
  {
    pid = fork();

    if (pid < 0)
    {
      return 1;
    }

    if (pid == 0)
    {
      srand(getpid() * time(NULL));
      ChildProcess(qid, mesh, N0DES, links);
      break;
    }
  }

  if (pid > 0)
  {
    pid_t wpid;

    ParentProcess(0, mesh, N0DES, links);

    while ((wpid = wait(NULL)) > 0);

    printf("\n");
    printf("\n");

    printf("All children comleted\n");

  }

  exit(0);
}
