#include <stdbool.h>
#include <unistd.h>


typedef struct link {
    int source;
    int target;
    int fd;
} Link;

typedef enum mode {
    READ_LINK,
    WRITE_LINK
} Mode;

typedef struct channel {
    int target;
    Mode mode;
} Channel;



void CreateMesh(Link* mesh, int nodes);
void CloseRemoteLinks(Link * mesh, int qid, int nodes);
int getPort(Link* mesh, int qid, Channel channel, int nodes);
void CloseLinksMode(Link* mesh, Mode mode, int qid, int nodes);
void CloseParentLinks(Link * mesh, int qid, int nodes);


