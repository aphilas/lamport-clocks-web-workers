#include "mesh.h"
#include "utils.h"

void CreateMesh(Link *mesh, int nodes)
{
    int count = 0;
    for (int i = 0; i < nodes; i++)
    {
        for (int j = 0; j < nodes; j++) {
            if (i == j) continue;

            int fds[2];
            pipe(fds);

            Link link = { .fd= fds[0], .source= i, .target= j };
            Link link2 = { .fd= fds[1], .source= j, .target= i };

            mesh[count] = link;
            mesh[++count] = link2;
        }
    }
}

void CloseRemoteLinks(Link *mesh, int qid, int nodes)
{
    int links = square(nodes) - nodes;

    for (int i = 0; i < links; i++)
    {
        Link link = mesh[i];

        if (link.source != qid && link.target != qid) {
            close(link.fd);
        }
    }


}

int getPort(Link *mesh, int qid, Channel channel, int nodes)
{
    int links = square(nodes) - nodes;

    for (int i = 0; i < links; i++)
    {
        Link link = mesh[i];

        if (channel.mode == READ_LINK && link.target == qid && link.source == channel.target) {
            return link.fd;
        }

        if (channel.mode == WRITE_LINK && link.source == qid && link.target == channel.target) {
            return link.fd;
        }
    }
}

void CloseLinksMode(Link *mesh, Mode mode, int qid, int nodes)
{
    int links = square(nodes) - nodes;

    for (int i = 0; i < links; i++)
    {
        Link link = mesh[i];

        if ((link.source == 0 || link.target == 0) && qid != 0) continue;

        if (mode == READ_LINK && link.source == qid) {
            close(link.fd);
        }

        if (mode == WRITE_LINK && link.target == qid) {
            close(link.fd);
        }
    }
}

void CloseParentLinks(Link *mesh, int qid, int nodes)
{
    int links = square(nodes) - nodes;

    for (int i = 0; i < links; i++)
    {
        Link link = mesh[i];

        if ((link.target == qid && link.source == 0) || (link.source == qid && link.target == 0)) {
            close(link.fd);
        }
    }
}
