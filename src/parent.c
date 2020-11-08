#include "parent_child.h"

void Monitor(Link *mesh, int qid, int nodes);

void ParentProcess(int qid, Link *mesh, int nodes)
{
    CloseRemoteLinks(mesh, qid, nodes);

    CloseLinksMode(mesh, WRITE_LINK, qid, nodes);

    Monitor(mesh, qid, nodes);

    // log out their messages
    CloseLinksMode(mesh, READ_LINK, qid, nodes);
}

void Monitor(Link *mesh, int qid, int nodes)
{
    for (int child = 1; child < nodes; child++)
    {
        printf("hello from parent\n");
        Channel target = {.mode = READ_LINK, .target = child};

        int fd_target = getPort(mesh, qid, target, nodes);

        Log log;

        int msg = 5;
        while (msg > 0)
        {
            int res = read(fd_target, &log, sizeof(log));

            if (res == 0)
            {
                printf("buffer closed\n");
                break;
            }
            else if (res == -1)
            {
                printf("error reading from buffer: %d\n", fd_target);
            }
            else
            {

                switch (log.type)
                {
                case LOCAL:
                    printf("State: Child #%d has value: %d\n", log.id, log.n);
                    break;

                case RECEIVE:
                    printf("Transfer: Child #%d has received value: %d from Child #%d\n", log.id, log.n, log.source);

                case SEND:
                    printf("Transfer: Child #%d has sent value: %d to Child #%d\n", log.id, log.n, log.source);

                default:
                    printf("Invalid log format! %d\n", log.id);
                }
            }
            msg--;
        }
    }
}