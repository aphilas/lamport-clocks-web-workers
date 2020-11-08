#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "parent_child.h"

#define SEED 549873
#define N0DES 3

int main(void)
{
  Link mesh[square(N0DES) - N0DES];

  CreateMesh(mesh, N0DES);

  bool parent = false;

  for (int qid = 1; qid < N0DES; qid++)
  {
    int pid = fork();

    if (pid < 0)
    {
      //fork failed
      return 1;
    }

    if (pid == 0)
    {
      ChildProcess(qid, mesh, N0DES);
      break;
    }
    else
    {
      parent = true;
      // push pid onto an array
    }
  }

  if (parent)
  {
    pid_t wpid;
    int status = 0;
    ParentProcess(0, mesh, N0DES);
    wait(NULL);
    wait(NULL);
    // while ((wpid = wait(&status)) > 0) {
      // printf("Child with PID #%d completed", wpid);
    // }
    //wait for children before exiting
  }

  // free(mesh);

  exit(0);
}
