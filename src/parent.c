#include "parent_child.h"

typedef struct
{
    Link *mesh;
    int qid;
    int child;
    int links;
} MonitorArgs;

void MonitorChild(Link *mesh, int qid, int child, int links);
void MonitorAll(Link *mesh, int qid, int nodes, int links);
void *MonitorChild_P(void *args);

void ParentProcess(int qid, Link *mesh, int nodes, int links)
{
    CloseRemoteLinks(mesh, qid, links);

    CloseLinksMode(mesh, WRITE_LINK, qid, links);

    MonitorAll(mesh, qid, nodes, links);

    CloseLinksMode(mesh, READ_LINK, qid, links);
}

void MonitorAll(Link *mesh, int qid, int nodes, int links)
{
    pthread_t threads[nodes - 1];
    MonitorArgs* m_args;

    for (int child = 1; child < nodes; child++)
    {
        m_args = malloc(sizeof(MonitorArgs));
        m_args->child = child;
        m_args->qid = qid;
        m_args->links = links;
        m_args->mesh = mesh;

        pthread_create(&threads[child-1], NULL, MonitorChild_P, (void *)m_args);
    }

    for (int child = 1; child < nodes; child++) {
        pthread_join(threads[child-1], NULL);
    }

}

void *MonitorChild_P(void *args)
{
    MonitorArgs *m_args = (MonitorArgs *)args;

    Link *mesh = m_args->mesh;
    int qid = m_args->qid;
    int child = m_args->child;
    int links = m_args->links;

    MonitorChild(mesh, qid, child, links);

    free(args);

    pthread_exit(NULL);
}

void MonitorChild(Link *mesh, int qid, int child, int links)
{
    int fd_target = getPort(mesh, qid, child, READ_LINK, links);

    Log log;

    while (read(fd_target, &log, sizeof(log)) > 0)
    {
        switch (log.type)
        {
        case LOCAL:
            printf("State: Child #%d has value: %d\n", log.actor, log.state);
            break;

        case UPDATE:
            printf("Update: Child #%d has updated value to : %d\n", log.actor, log.state);
            break;

        case RECEIVE:
            printf("Transfer: Child #%d has RECEIVED (N=%d) from Child #%d\n", log.actor, log.state, log.patner);
            break;

        case SEND:
            printf("Transfer: Child #%d has SENT (N=%d) to Child #%d\n", log.actor, log.state, log.patner);
            break;

        default:
            break;
        }
    }
    
}