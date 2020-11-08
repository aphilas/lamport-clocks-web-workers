#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "parent_child.h"

#define TRANSFERS 2
#define MAX_SLEEP 20
#define P_SEND 5

void Listen(int qid, Link* mesh, int nodes);
void Broadcast(int qid, Link *mesh, int nodes);


int counter;

void ChildProcess(int qid, Link *mesh, int nodes)
{
    CloseRemoteLinks(mesh, qid, nodes);

    counter = 0;

    Broadcast(qid, mesh, nodes);

    CloseLinksMode(mesh, WRITE_LINK, qid, nodes);

    Listen(qid, mesh, nodes);

    CloseLinksMode(mesh, READ_LINK, qid, nodes);

    CloseParentLinks(mesh, qid, nodes);
}

void Broadcast(int qid, Link *mesh, int nodes)
{
    for (int i = 0; i < TRANSFERS; i++)
    {
        usleep(rand() % MAX_SLEEP);

        int t = get_target(nodes, qid);

        counter++;

        Channel parent = {.mode = WRITE_LINK, .target = 0};

        Channel target = {.mode = WRITE_LINK, .target = t};

        int fd_parent = getPort(mesh, qid, parent, nodes);

        int fd_target = getPort(mesh, qid, target, nodes);

        if (toss(P_SEND))
        {
            Data data = {.id = qid, .n = counter};
            Log log = {.id = qid, .n = counter, .type = SEND, .source=t};

            write(fd_target, &data, sizeof(data));
            write(fd_parent, &log, sizeof(log));
        }
        else
        {
            Log log = {.id = qid, .n = counter, .type = LOCAL, .source=-1};
            write(fd_parent, &log, sizeof(log));
        }
    }
}

void Listen(int qid, Link* mesh, int nodes)
{
    for (int peer = 1; peer < nodes; peer++)
    {
        if (peer == qid) continue;

        Channel parent = {.mode = WRITE_LINK, .target = 0};

        Channel target = {.mode = READ_LINK, .target = peer};

        int fd_parent = getPort(mesh, qid, parent, nodes);

        int fd_target = getPort(mesh, qid, target, nodes);

        Data data;

        while (read(fd_target, &data, sizeof(data)) != 0) {
            counter = max(counter + 1, data.n);
            Log log = {.id = qid, .n = counter, .type = RECEIVE, .source = data.id };
            write(fd_parent, &log, sizeof(log));
        }

    }
}
