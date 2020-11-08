#include <stdio.h>

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
    RECEIVE
} EventType;

typedef struct log
{
    int id;
    int n;
    EventType type;
    int source;
} Log;

void ChildProcess(int qid, Link *mesh, int nodes);
void ParentProcess(int qid, Link *mesh, int nodes);
