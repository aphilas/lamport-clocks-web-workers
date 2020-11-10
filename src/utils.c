#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


int fact(int n)
{
  int res = 1;
  while (n > 0)
  {
    res = res * n;
    n--;
  }
  return res;
}

int n_choose_k(int n, int k)
{
  return fact(n) / (fact(k) * fact(n - k));
}

bool toss(int k)
{

  double random = (double)rand() / (double)RAND_MAX;

  double mark = (double)1 / (double)k;

  return random < mark;
}

int square(int n)
{
  return n * n;
}

int get_target(int nodes, int qid)
{
  if (nodes <= 2)
  {
    return 0;
  }

  if (nodes == 3)
  {
    if (qid == 1)
    {
      return 2;
    }
    else
    {
      return 1;
    }
  }

  int n = rand() % (nodes - 2);
  if (n < qid - 1)
  {
    return n+1;
  }
  else
  {
    return n + 2;
  }
}

int max(int a, int b)
{
  if (a < b)
  {
    return b;
  }
  else
  {
    return a;
  }
}