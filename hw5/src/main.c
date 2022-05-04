#include <stdlib.h>
#include <unistd.h>

#include "pbx.h"
#include "server.h"
#include "debug.h"
#include "csapp.h"

static void terminate(int status);
void SIGHUP_handler(int sig);
volatile sig_atomic_t terminateFlag = 0;

/*
 * "PBX" telephone exchange simulation.
 *
 * Usage: pbx <port>
 */
int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    if (argc == 1){
        fprintf(stderr, "Usage: pbx -p <port>\n");
        exit(EXIT_SUCCESS);
    }

    char *port = NULL;    // store port num
    int opt;
    opterr = 0;         // prevent the error message from getopt 

    while((opt = getopt(argc, argv, "p:")) != -1){
      switch(opt){
        case 'p':                       // port command is given
          debug("p command is given");
          port = optarg;                // store given port
          break;
        default:
          fprintf(stderr, "Usage: pbx -p <port>\n");
          exit(EXIT_FAILURE);
      }
    }

    if (argc != 3 || port == NULL) {
        fprintf(stderr, "Usage: pbx -p <port>\n");
        exit(EXIT_FAILURE);
    }

    if ((atoi(port)) < 1024) {
        fprintf(stderr, "Wrong port format (should be greater than or equal to 1024)\n");
        exit(EXIT_FAILURE);
    }

    // Perform required initialization of the PBX module.
    debug("Initializing PBX... with port : %s", port);

    pbx = pbx_init();
    Signal(SIGHUP, SIGHUP_handler);
    
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    listenfd = Open_listenfd(port);
    while (terminateFlag == 0) {
      clientlen = sizeof(struct sockaddr_storage);
      connfdp = Malloc(sizeof(int));
      *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
      if (terminateFlag && errno == EINTR) {
        debug("Got EINTR");
        break;
      }
      Pthread_create(&tid, NULL, pbx_client_service, connfdp);
    }

    debug("\n\n main terminating ");
    terminate(EXIT_SUCCESS);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status) {
    debug("Shutting down PBX...");
    pbx_shutdown(pbx);
    debug("PBX server terminating");
    exit(status);
}

void SIGHUP_handler(int sig) {
  terminateFlag = 1;
  debug("Handler triggered");
}