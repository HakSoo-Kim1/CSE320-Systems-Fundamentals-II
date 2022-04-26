/*
 * "PBX" server module.
 * Manages interaction with a client telephone unit (TU).
 */
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "pbx.h"
#include "server.h"
#include "csapp.h"



char *commandStrGenerator(char* command);

/*
 * Thread function for the thread that handles interaction with a client TU.
 * This is called after a network connection has been made via the main server
 * thread and a new thread has been created to handle the connection.
 */
void *pbx_client_service(void *arg) {

    char* pickupCommand = commandStrGenerator(tu_command_names[TU_PICKUP_CMD]); 
    char* hangupCommand = commandStrGenerator(tu_command_names[TU_HANGUP_CMD]); 
    char* dialCommand = commandStrGenerator(tu_command_names[TU_DIAL_CMD]); 
    char* chatCommand = commandStrGenerator(tu_command_names[TU_CHAT_CMD]); 

    int connfd = *((int *)arg);
    Free(arg);
    Pthread_detach(pthread_self());
    TU * newTu = tu_init(connfd);
    pbx_register(pbx, newTu, connfd);
    FILE *fp = Fdopen(connfd, "r");
    while(1){       // service loop
        int counter = 0;
        char *buf =  (char *) Malloc(MAXBUF);
        int bufSize = MAXBUF;
        char ch;
        while ((ch = fgetc(fp)) != EOF) {
            if ((sizeof(char) * (counter + 1)) >= bufSize){ // more buffer required
                bufSize *= 2;
                buf = Realloc(buf, bufSize);
            }
            buf[counter] = ch;
            if ((counter >= 1) && (ch == '\n') && (buf[counter - 1] == '\r')){   // command end
                buf[counter + 1] = '\0';
                break;
            }
            counter += 1;
        }
        if (ch == EOF){
            Free(buf);
            break;
        }
        
        debug("\tgiven string is : %s",buf);

        if (strcmp(buf, pickupCommand) == 0){
            tu_pickup(newTu);
        }   
        else if (strcmp(buf, hangupCommand) == 0){
            tu_hangup(newTu);
        }
        else{
            buf[counter - 1] = '\0';
            char *ptr;
            if ((ptr = strstr(buf, dialCommand))!= NULL){
                if (ptr == buf){
                    int dialStart = strlen(dialCommand);
                    if (buf[dialStart] == '\0'){
                        // debug("no dial given");         /// maybe not necessary
                        // pbx_dial(pbx, newTu, -1);       /// maybe not necessary
                    }
                    else{
                        debug("%s",&buf[dialStart]);
                        int dialNum = atoi(&buf[dialStart]);    // case 123a    prevent? 
                        debug("dialNum is %d",dialNum); 
                        if (dialNum >= 0){
                            pbx_dial(pbx, newTu, dialNum);
                        }
                    }
                }
            }
            else if ((ptr = strstr(buf, chatCommand)) != NULL){
                if (ptr == buf){
                    int chatStart = strlen(chatCommand);
                    if (buf[chatStart] == '\0'){
                        tu_chat(newTu, "");
                    }
                    else{
                        tu_chat(newTu, &buf[chatStart]);
                    }
                    
                }
            }
            else{
                debug("WRONG COMMAND : %s",buf);
            }
        }
        Free(buf);
    }
    

    Free(pickupCommand);
    Free(hangupCommand);
    Free(dialCommand);
    Free(chatCommand);
    pbx_unregister(pbx, newTu);
    Close(connfd);

    return NULL;
}

char *commandStrGenerator(char* command){
    int commandLen = strlen(command);
    char* commandStr;
    if ( strcmp(command, tu_command_names[TU_PICKUP_CMD]) == 0 || strcmp(command, tu_command_names[TU_HANGUP_CMD]) == 0){
        commandStr = Malloc(sizeof(char) * (commandLen + 3));
        strcpy(commandStr, command);
        commandStr[commandLen] = '\r';
        commandStr[commandLen + 1] = '\n';
        commandStr[commandLen + 2] = '\0';
        return commandStr;
    }
    else if(strcmp(command, tu_command_names[TU_DIAL_CMD]) == 0 || strcmp(command, tu_command_names[TU_CHAT_CMD]) == 0){
        commandStr = Malloc(sizeof(char) * (commandLen + 2));
        strcpy(commandStr, command);
        commandStr[commandLen] = ' ';
        commandStr[commandLen + 1] = '\0';
        return commandStr;
    }
    return NULL;
}

// gdb --args path/to/exe arg1 arg2 arg3
// 