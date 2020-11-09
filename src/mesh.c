#include "mesh.h"
#include "utils.h"

int CreateMesh(Link *mesh, int nodes)
{
    int count = 0;
    for (int i = 0; i < nodes; i++)
    {
        for (int j = 0; j < nodes; j++)
        {
            if (i == j)
                continue;

            int fds[2];
            pipe(fds);

            Link link = {.fd = fds[0], .actor = i, .patner = j, .mode = READ_LINK};
            Link link2 = {.fd = fds[1], .actor = j, .patner = i, .mode = WRITE_LINK};

            mesh[count] = link;
            mesh[count + 1] = link2;

            count += 2;
        }
    }

    return count;
}

void CloseRemoteLinks(Link *mesh, int actor, int links)
{
    Link link;

    for (int i = 0; i < links; i++)
    {
        link = mesh[i];

        if (link.actor != actor)
        {
            close(link.fd);
        }
    }
}

int getPort(Link *mesh, int actor, int patner, Mode mode, int links)
{
    Link link;

    for (int i = 0; i < links; i++)
    {
        link = mesh[i];

        if (link.actor == actor && link.patner == patner && link.mode == mode)
        {
            return link.fd;
        }
    }
}

void CloseLinksMode(Link *mesh, Mode mode, int actor, int links)
{
    Link link;

    for (int i = 0; i < links; i++)
    {
        link = mesh[i];

        if (link.patner == 0 && actor != 0)
            continue;

        if (link.mode == mode && link.actor == actor)
        {
            close(link.fd);
        }
    }
}

void CloseParentLinks(Link *mesh, int actor, int links)
{
    Link link;

    for (int i = 0; i < links; i++)
    {
        link = mesh[i];

        if (link.actor == actor && link.patner == 0)
        {
            close(link.fd);
        }
    }
}
