/*
 * PBX: simulates a Private Branch Exchange.
 */
#include <stdlib.h>

#include "pbx.h"
#include "debug.h"
#include "csapp.h"

typedef struct pbx
{
    struct tu_link *tuHead;
    sem_t pbxMutex;
    sem_t pbxZeroMutex;
}PBX;

typedef struct tu_link {
    TU *tu;
    struct tu_link *prev;
    struct tu_link *next;
} TU_LINK;

/*
 * Initialize a new PBX.
 *
 * @return the newly initialized PBX, or NULL if initialization fails.
 */
PBX *pbx_init() {
    debug("pbx init is called");
    TU_LINK *tuHead = Malloc(sizeof(TU_LINK)); 
    tuHead -> tu = NULL;
    tuHead -> prev = tuHead;
    tuHead -> next = tuHead;
    PBX *pbx = Malloc(sizeof(PBX)); 
    pbx -> tuHead = tuHead;
    sem_init(&(pbx -> pbxMutex), 0, 1);
    sem_init(&(pbx -> pbxZeroMutex), 0, 1);
    return pbx;
}

/*
 * Shut down a pbx, shutting down all network connections, waiting for all server
 * threads to terminate, and freeing all associated resources.
 * If there are any registered extensions, the associated network connections are
 * shut down, which will cause the server threads to terminate.
 * Once all the server threads have terminated, any remaining resources associated
 * with the PBX are freed.  The PBX object itself is freed, and should not be used again.
 *
 * @param pbx  The PBX to be shut down.
 */

void pbx_shutdown(PBX *pbx) {
    debug("\tSHUTTING DOWN");
    TU_LINK *tuHead = pbx -> tuHead;
    TU_LINK *node = tuHead -> next;

    P(&(pbx->pbxMutex));

    while(node != tuHead){
        debug("shutting down : %d",tu_fileno(node -> tu));
        if (shutdown(tu_fileno(node -> tu), SHUT_RDWR) != 0) {
            debug("error while shutting down");
        }
        node = node -> next;
    }

    V(&(pbx->pbxMutex));
    P(&pbx->pbxZeroMutex);
    Free(pbx -> tuHead);
    Free(pbx);
    debug("\t DONE SHUTTING DOWN");

}

/*
 * Register a telephone unit with a PBX at a specified extension number.
 * This amounts to "plugging a telephone unit into the PBX".
 * The TU is initialized to the TU_ON_HOOK state.
 * The reference count of the TU is increased and the PBX retains this reference
 *for as long as the TU remains registered.
 * A notification of the assigned extension number is sent to the underlying network
 * client.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU to be registered.
 * @param ext  The extension number on which the TU is to be registered.
 * @return 0 if registration succeeds, otherwise -1.
 */

int pbx_register(PBX *pbx, TU *tu, int ext) {
    debug("pbx_register is called");
    tu_set_extension(tu, ext);  // notify
    TU_LINK *tuLink = Malloc(sizeof(TU_LINK)); 
    tuLink -> tu = tu;
    TU_LINK *tuHead = pbx -> tuHead;

    P(&(pbx->pbxMutex));

    if ((tuHead == tuHead -> prev) && (tuHead == tuHead -> next)){
        P(&pbx->pbxZeroMutex);
    }

    TU_LINK *last =  tuHead -> prev;
    tuLink -> prev = last;
    tuLink -> next = tuHead;
    tuHead -> prev = tuLink;
    last -> next = tuLink;

    tu_ref(tu,"register!!");
    V(&(pbx->pbxMutex));

    return 0;

}


/*
 * Unregister a TU from a PBX.
 * This amounts to "unplugging a telephone unit from the PBX".
 * The TU is disassociated from its extension number.
 * Then a hangup operation is performed on the TU to cancel any
 * call that might be in progress.
 * Finally, the reference held by the PBX to the TU is released.
 *
 * @param pbx  The PBX.
 * @param tu  The TU to be unregistered.
 * @return 0 if unregistration succeeds, otherwise -1.
 */

int pbx_unregister(PBX *pbx, TU *tu) {
    debug("pbx_unregister is called");
    TU_LINK *tuHead = pbx -> tuHead;
    TU_LINK *node = tuHead -> next;
    TU_LINK *tuUnregister = NULL;

    while(node != tuHead){
        if (node -> tu == tu){
            tuUnregister = node;
            break;
        }
        node = node -> next;
    }
    if (!tuUnregister){return -1;}
    
    P(&(pbx->pbxMutex));

    if(tu_hangup(tuUnregister -> tu)){
        V(&(pbx->pbxMutex));
        return -1;
    };

    tuUnregister -> prev -> next = tuUnregister -> next;
    tuUnregister -> next -> prev = tuUnregister -> prev;
    tu_unref(tuUnregister -> tu, "unregister");
    Free(tuUnregister);

    V(&(pbx->pbxMutex));

    if ((tuHead == tuHead -> prev) && (tuHead == tuHead -> next)){
        debug("PBX IS EMPTY");
        V(&pbx->pbxZeroMutex);
    }
    return 0;


}

/*
 * Use the PBX to initiate a call from a specified TU to a specified extension.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU that is initiating the call.
 * @param ext  The extension number to be called.
 * @return 0 if dialing succeeds, otherwise -1.
 */
int pbx_dial(PBX *pbx, TU *tu, int ext) {
    debug("pbx_dial is called");
    TU_LINK *tuHead = pbx -> tuHead;
    TU_LINK *node = tuHead -> next;
    TU *tuReceiver = NULL;
    while(node != tuHead){
        if (tu_extension(node -> tu) == ext){
            tuReceiver = node -> tu;
            break;
        }
        node = node -> next;
    }
    return tu_dial(tu, tuReceiver);
}
