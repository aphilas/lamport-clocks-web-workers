#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "mesh.h"
#include "utils.h"

typedef struct data
{
    int n;
    int id;
} Data;

typedef enum event_type
{
    LOCAL,
    SEND,
    RECEIVE,
    UPDATE
} EventType;

typedef struct log
{
    int actor;
    int patner;
    int state;
    EventType type;
} Log;

void ChildProcess(int qid, Link *mesh, int nodes, int links);
void ParentProcess(int qid, Link *mesh, int nodes, int links);
