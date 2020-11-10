#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef enum mode {
    READ_LINK,
    WRITE_LINK
} Mode;

typedef struct link {
    int actor;
    int patner;
    int fd;
    Mode mode;
} Link;

int CreateMesh(Link* mesh, int nodes);
void CloseRemoteLinks(Link * mesh, int actor, int links);
int getPort(Link *mesh, int actor, int patner, Mode mode, int links);
void CloseLinksMode(Link* mesh, Mode mode, int actor, int links);
void CloseParentLinks(Link * mesh, int actor, int links);


