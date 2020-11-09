#include "parent_child.h"

#define TRANSFERS 4
#define MAX_SLEEP 20000
#define P_SEND 5
#define PARENT_ID 0

typedef struct
{
    Link *mesh;
    int qid;
    int nodes;
    int links;
} TransferArgs;

typedef struct
{
    Link *mesh;
    int qid;
    int peer;
    int links;
} ListenArgs;

void BroadcastAndListen(int qid, Link *mesh, int nodes, int links);
void *ListenPeer_P(void *args);
void ListenPeer(Link *mesh, int qid, int peer, int links);
void *SendData_P(void *args);
void SendData(Link *mesh, int qid, int nodes, int links);

void ChildProcess(int qid, Link *mesh, int nodes, int links)
{
    CloseRemoteLinks(mesh, qid, nodes);

    BroadcastAndListen(qid, mesh, nodes, links);

    CloseParentLinks(mesh, qid, links);
}

void BroadcastAndListen(int qid, Link *mesh, int nodes, int links)
{
    pthread_t b_threads[TRANSFERS];
    TransferArgs *t_args;

    pthread_t l_threads[nodes - 1];
    ListenArgs *l_args;

    for (int transfer = 0; transfer < TRANSFERS; transfer++)
    {
        t_args = malloc(sizeof(TransferArgs));
        t_args->qid = qid;
        t_args->links = links;
        t_args->nodes = nodes;
        t_args->mesh = mesh;

        pthread_create(&b_threads[transfer], NULL, SendData_P, (void *)t_args);
    }

    for (int peer = 1; peer < nodes; peer++)
    {
        l_args = malloc(sizeof(ListenArgs));
        l_args->qid = qid;
        l_args->peer = peer;
        l_args->links = links;
        l_args->mesh = mesh;

        pthread_create(&l_threads[peer - 1], NULL, ListenPeer_P, (void *)l_args);
    }

    for (int transfer = 0; transfer < TRANSFERS; transfer++)
    {
        pthread_join(b_threads[transfer], NULL);
    }

    CloseLinksMode(mesh, WRITE_LINK, qid, links);

    for (int peer = 1; peer < nodes; peer++)
    {
        pthread_join(l_threads[peer - 1], NULL);
    }

    CloseLinksMode(mesh, READ_LINK, qid, links);
}

void *SendData_P(void *args)
{
    TransferArgs *t_args = (TransferArgs *)args;

    Link *mesh = t_args->mesh;
    int qid = t_args->qid;
    int nodes = t_args->nodes;
    int links = t_args->links;

    SendData(mesh, qid, nodes, links);

    printf("Broadcaster on %d exiting", qid);

    free(args);

    pthread_exit(NULL);
}

void *ListenPeer_P(void *args)
{
    ListenArgs *l_args = (ListenArgs *)args;

    Link *mesh = l_args->mesh;
    int qid = l_args->qid;
    int links = l_args->links;
    int peer = l_args->peer;

    if (qid != peer)
    {
        ListenPeer(mesh, qid, peer, links);
    }

    printf("Listener between %d and %d exiting", qid, peer);

    free(args);


    pthread_exit(NULL);
}

void SendData(Link *mesh, int qid, int nodes, int links)
{
    usleep(rand() % MAX_SLEEP);

    int counter = rand() % 20;

    int fd_parent = getPort(mesh, qid, PARENT_ID, WRITE_LINK, links);

    if (toss(P_SEND))
    {
        int target = get_target(nodes, qid);
        int fd_target = getPort(mesh, qid, target, WRITE_LINK, links);

        Data data = {.id = qid, .n = counter};
        write(fd_target, &data, sizeof(data));

        Log log = {.actor = qid, .state = counter, .type = SEND, .patner = target};
        write(fd_parent, &log, sizeof(log));
    }
    else
    {
        Log log = {.actor = qid, .state = counter, .type = LOCAL, .patner = PARENT_ID};
        write(fd_parent, &log, sizeof(log));
    }
}

void ListenPeer(Link *mesh, int qid, int peer, int links)
{
    int counter = 5000;

    int fd_parent = getPort(mesh, qid, PARENT_ID, WRITE_LINK, links);

    int fd_target = getPort(mesh, qid, peer, READ_LINK, links);

    Data data;

    while (read(fd_target, &data, sizeof(data)) > 0)
    {
        // counter = max(counter + 1, data.n);
        Log log = {.actor = qid, .state = data.n, .type = RECEIVE, .patner = data.id};
        write(fd_parent, &log, sizeof(log));
    }
}
