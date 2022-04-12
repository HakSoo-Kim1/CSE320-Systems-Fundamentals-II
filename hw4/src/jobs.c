#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include "csapp.h"

#include "mush.h"
#include "debug.h"

/*
 * This is the "jobs" module for Mush.
 * It maintains a table of jobs in various stages of execution, and it
 * provides functions for manipulating jobs.
 * Each job contains a pipeline, which is used to initialize the processes,
 * pipelines, and redirections that make up the job.
 * Each job has a job ID, which is an integer value that is used to identify
 * that job when calling the various job manipulation functions.
 *
 * At any given time, a job will have one of the following status values:
 * "new", "running", "completed", "aborted", "canceled".
 * A newly created job starts out in with status "new".
 * It changes to status "running" when the processes that make up the pipeline
 * for that job have been created.
 * A running job becomes "completed" at such time as all the processes in its
 * pipeline have terminated successfully.
 * A running job becomes "aborted" if the last process in its pipeline terminates
 * with a signal that is not the result of the pipeline having been canceled.
 * A running job becomes "canceled" if the jobs_cancel() function was called
 * to cancel it and in addition the last process in the pipeline subsequently
 * terminated with signal SIGKILL.
 *
 * In general, there will be other state information stored for each job,
 * as required by the implementation of the various functions in this module.
 */

int jobIdCounter = 0;

typedef enum {
    NO_STATUS,
    NEW_STATUS,
    RUNNING_STATUS,
    COMPLETED_STATUS,
    ABORTED_STATUS,
    CANCELED_STATUS               
} JOB_STATUS;

typedef struct job_link {
    PIPELINE *pline;
    pid_t jobPGID;        
    int id;       
    JOB_STATUS status;
    struct job_link *prev;
    struct job_link *next;
} JOB_LINK;

// JOB_LINK jobHead = {NULL, -1, -1, -1, NO_STATUS, &jobHead, &jobHead};
JOB_LINK jobHead = {NULL, -1, -1, NO_STATUS, &jobHead, &jobHead};

int count_args(ARG *args);
void sigchldHandler(int s);
void insertNewJob(JOB_LINK *newJob);

/**
 * @brief  Initialize the jobs module.
 * @details  This function is used to initialize the jobs module.
 * It must be called exactly once, before any other functions of this
 * module are called.
 *
 * @return 0 if initialization is successful, otherwise -1.
 */
int jobs_init(void) {
    Signal(SIGCHLD, sigchldHandler);


    //
        // Signal(SIGCHLD, sigchldHandler); iosig

    // signal handler 

    // TO BE IMPLEMENTED
    // abort();
    return 0;
}

/**
 * @brief  Finalize the jobs module.
 * @details  This function is used to finalize the jobs module.
 * It must be called exactly once when job processing is to be terminated,
 * before the program exits.  It should cancel all jobs that have not
 * yet terminated, wait for jobs that have been cancelled to terminate,
 * and then expunge all jobs before returning.
 *
 * @return 0 if finalization is completely successful, otherwise -1.
 */
int jobs_fini(void) {
    // free malloc 

    // TO BE IMPLEMENTED
    // abort();
    return 0;
}

/**
 * @brief  Print the current jobs table.
 * @details  This function is used to print the current contents of the jobs
 * table to a specified output stream.  The output should consist of one line
 * per existing job.  Each line should have the following format:
 *
 *    <jobid>\t<pgid>\t<status>\t<pipeline>
 *
 * where <jobid> is the numeric job ID of the job, <status> is one of the
 * following strings: "new", "running", "completed", "aborted", or "canceled",
 * and <pipeline> is the job's pipeline, as printed by function show_pipeline()
 * in the syntax module.  The \t stand for TAB characters.
 *
 * @param file  The output stream to which the job table is to be printed.
 * @return 0  If the jobs table was successfully printed, -1 otherwise.
 */
int jobs_show(FILE *file) {
    debug("---- JOBS ----");
    JOB_LINK *head = &jobHead;
    JOB_LINK *node = jobHead.next;
    while(node != head){
        char *status = NULL;
        if (node -> status == NEW_STATUS){status = "new";}
        else if (node -> status == RUNNING_STATUS){status = "running";}
        else if (node -> status == COMPLETED_STATUS){status = "completed";}
        else if (node -> status == ABORTED_STATUS){status = "aborted";}
        else if (node -> status == CANCELED_STATUS){status = "canceled";}
        else{status = "NO_STATUS";}
        fprintf(file, "%d\t%d\t%s\t",node -> id,node -> jobPGID, status);
        show_pipeline(file, node ->pline);
        fprintf(file,"\n");
        node = node -> next;
    }
    return 0;
}

/**
 * @brief  Create a new job to run a pipeline.
 * @details  This function creates a new job and starts it running a specified
 * pipeline.  The pipeline will consist of a "leader" process, which is the direct
 * child of the process that calls this function, plus one child of the leader
 * process to run each command in the pipeline.  All processes in the pipeline
 * should have a process group ID that is equal to the process ID of the leader.
 * The leader process should wait for all of its children to terminate before
 * terminating itself.  The leader should return the exit status of the process
 * running the last command in the pipeline as its own exit status, if that
 * process terminated normally.  If the last process terminated with a signal,
 * then the leader should terminate via SIGABRT.
 *
 * If the "capture_output" flag is set for the pipeline, then the standard output
 * of the last process in the pipeline should be redirected to be the same as
 * the standard output of the pipeline leader, and this output should go via a
 * pipe to the main Mush process, where it should be read and saved in the data
 * store as the value of a variable, as described in the assignment handout.
 * If "capture_output" is not set for the pipeline, but "output_file" is non-NULL,
 * then the standard output of the last process in the pipeline should be redirected
 * to the specified output file.   If "input_file" is set for the pipeline, then
 * the standard input of the process running the first command in the pipeline should
 * be redirected from the specified input file.
 *
 * @param pline  The pipeline to be run.  The jobs module expects this object
 * to be valid for as long as it requires, and it expects to be able to free this
 * object when it is finished with it.  This means that the caller should not pass
 * a pipeline object that is shared with any other data structure, but rather should
 * make a copy to be passed to this function.
 * 
 * @return  -1 if the pipeline could not be initialized properly, otherwise the
 * value returned is the job ID assigned to the pipeline.
 */
int jobs_run(PIPELINE *pline) {
    
    sigset_t mask_child, prev;
    Sigemptyset(&mask_child);
    Sigemptyset(&prev);
    Sigaddset(&mask_child, SIGCHLD);
    Sigprocmask(SIG_BLOCK, &mask_child, NULL);

    JOB_LINK *newJob = malloc(sizeof(JOB_LINK));
    newJob -> pline = copy_pipeline(pline);
    newJob -> id = jobIdCounter;
    newJob -> status = NEW_STATUS;
    insertNewJob(newJob);

    jobIdCounter += 1;

    pid_t leaderPID;
// mask 
    leaderPID = fork();
    if (leaderPID == 0){  /// leader process
        // unmask
        Sigprocmask(SIG_UNBLOCK, &mask_child, NULL);

        if (setpgid(getpid(), getpid()) == -1){exit(EXIT_FAILURE);}
        COMMAND *commandNode = pline -> commands;
        int fd[2];
        int temp;
        int firstFlag = 1;
        char *input_file = pline->input_file;
        char *output_file = pline->output_file;

        while(commandNode){
            if (pipe(fd) == -1){debug("PIPE ERROR"); exit(EXIT_FAILURE);}
            pid_t commandPID  = fork();
            if (commandPID == 0){
                if (setpgid(getpid(), getppid()) == -1){exit(EXIT_FAILURE);}
                if (firstFlag){
                    if (input_file != NULL){
                        int input;
                        if ((input = open(input_file,O_RDONLY)) == -1){exit(EXIT_FAILURE);}
                        if (dup2(input, STDIN_FILENO) == -1){exit(EXIT_FAILURE);}
                        if (close(input) == -1 ){exit(EXIT_FAILURE);}
                    }
                }
                else{
                    if(dup2(temp,STDIN_FILENO) == -1) {exit(EXIT_FAILURE);}
                    if(close(temp) == -1) {exit(EXIT_FAILURE);}
                }

                if (commandNode -> next == NULL){
                    if (output_file != NULL){
                        int output;
                        if ((output = open(output_file, O_CREAT | O_TRUNC | O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1){exit(EXIT_FAILURE);}
                        if (dup2(output,STDOUT_FILENO) == -1){exit(EXIT_FAILURE);}
                        if (close(output) == -1 ){exit(EXIT_FAILURE);}
                    }
                }
                else{
                    if (dup2(fd[1],STDOUT_FILENO) == -1)    {exit(EXIT_FAILURE);}             
                    if (close(fd[1]) == -1)    {exit(EXIT_FAILURE);}             
                }

                int argsNum = count_args(commandNode -> args);
                char *args[argsNum + 1]; 
                // debug("argsNum is %d", argsNum);
                ARG *currArg = commandNode -> args;
                for (int i = 0; i < argsNum ; i++){
                    args[i] = malloc(strlen(currArg -> expr -> members.variable) + 1);
                    strcpy(args[i], currArg -> expr -> members.variable);
                    // debug(" args[%d] = %s",i, currArg -> expr -> members.variable);
                    currArg = currArg -> next;
                }
                args[argsNum] = NULL;
                debug("EXECUTING %s",args[0]);
                int isError = execvp(args[0],args);
                //// have to free 
                debug("\t\t\t\t\t\t\t\tFAILED");

                debug("errorno is %d",errno);
                debug("\t\ta %d", isError);
                exit(EXIT_FAILURE);
            }
            firstFlag = 0;
            temp = fd[0];
            if (close(fd[1]) == -1) {exit(EXIT_FAILURE);}             
            commandNode = commandNode -> next;
        }
        pid_t pid;
        while ((pid = waitpid(-1, NULL, 0)) > 0) { 
        }
        debug("leader start WAITING");

        sleep(10);

        debug("leader exit");
        exit(EXIT_SUCCESS);
    }
// unblock 
    // sleep(10);
    // sicpomask(); mask sigchild
        newJob -> status = RUNNING_STATUS;
        newJob -> jobPGID = leaderPID;
        debug("leader pid is %d",leaderPID);
    Sigprocmask(SIG_UNBLOCK, &mask_child, NULL);

        // newJob -> jobPID = leaderPID;
    // newJob -> jobPGID = getpgrp();
        // debug()

    // unblock sigchild 

    // sleep(10);
    // else{  
    //     waitpid(leaderPID, &leaderStatus, 0);
    //     // int status = waitpid(leaderPID, &leaderStatus, 0);
    //     // debug("result from waitpid is : %d",status);
    //     if (!WIFEXITED(leaderStatus)){
    //         exit(EXIT_FAILURE);
    //     }
    // }
    return newJob -> id;

}

/**
 * @brief  Wait for a job to terminate.
 * @details  This function is used to wait for the job with a specified job ID
 * to terminate.  A job has terminated when it has entered the COMPLETED, ABORTED,
 * or CANCELED state.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(),
 * or -1 if any error occurs that makes it impossible to wait for the specified job.
 */
int jobs_wait(int jobid) {
    int leaderStatus;
    JOB_LINK *head = &jobHead;
    JOB_LINK *node = jobHead.next;
    JOB_LINK *job = NULL;
    while(node != head){
        if (node -> id == jobid){
            job = node;
            break;
        }
        node = node -> next;
    }

    if (!job){debug("ERROR OCCURED IN JOBS WAIT");return -1;}

    while(job -> status == RUNNING_STATUS){
        sigset_t mask;
        sigemptyset(&mask);
        debug("!!! WAITING IN JOBS WAIT");
        sigsuspend(&mask);
        debug("!!! WAITING IN JOBS DONE");
    }
    debug("leaderPID waiting");
    int result = waitpid(job -> jobPGID, &leaderStatus, 0);
    debug("leaderPID done %d",result);

    return result;
}

/**
 * @brief  Poll to find out if a job has terminated.
 * @details  This function is used to poll whether the job with the specified ID
 * has terminated.  This is similar to jobs_wait(), except that this function returns
 * immediately without waiting if the job has not yet terminated.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(), if the job
 * has terminated, or -1 if the job has not yet terminated or if any other error occurs.
 */
int jobs_poll(int jobid) {
    // TO BE IMPLEMENTED
    // abort();
    return 0;
}

/**
 * @brief  Expunge a terminated job from the jobs table.
 * @details  This function is used to expunge (remove) a job that has terminated from
 * the jobs table, so that space in the table can be used to start some new job.
 * In order to be expunged, a job must have terminated; if an attempt is made to expunge
 * a job that has not yet terminated, it is an error.  Any resources (exit status,
 * open pipes, captured output, etc.) that were being used by the job are finalized
 * and/or freed and will no longer be available.
 *
 * @param  jobid  The job ID of the job to expunge.
 * @return  0 if the job was successfully expunged, -1 if the job could not be expunged.
 */
int jobs_expunge(int jobid) {
    // TO BE IMPLEMENTED
    // abort();
    return 0;
}

/**
 * @brief  Attempt to cancel a job.
 * @details  This function is used to attempt to cancel a running job.
 * In order to be canceled, the job must not yet have terminated and there
 * must not have been any previous attempt to cancel the job.
 * Cancellation is attempted by sending SIGKILL to the process group associated
 * with the job.  Cancellation becomes successful, and the job actually enters the canceled
 * state, at such subsequent time as the job leader terminates as a result of SIGKILL.
 * If after attempting cancellation, the job leader terminates other than as a result
 * of SIGKILL, then cancellation is not successful and the state of the job is either
 * COMPLETED or ABORTED, depending on how the job leader terminated.
 *
 * @param  jobid  The job ID of the job to cancel.
 * @return  0 if cancellation was successfully initiated, -1 if the job was already
 * terminated, a previous attempt had been made to cancel the job, or any other
 * error occurred.
 */
int jobs_cancel(int jobid) {
    // TO BE IMPLEMENTED
    // abort();
    return 0;
}

/**
 * @brief  Get the captured output of a job.
 * @details  This function is used to retrieve output that was captured from a job
 * that has terminated, but that has not yet been expunged.  Output is captured for a job
 * when the "capture_output" flag is set for its pipeline.
 *
 * @param  jobid  The job ID of the job for which captured output is to be retrieved.
 * @return  The captured output, if the job has terminated and there is captured
 * output available, otherwise NULL.
 */
char *jobs_get_output(int jobid) {
    // TO BE IMPLEMENTED
    // abort();
    return 0;
}

/**
 * @brief  Pause waiting for a signal indicating a potential job status change.
 * @details  When this function is called it blocks until some signal has been
 * received, at which point the function returns.  It is used to wait for a
 * potential job status change without consuming excessive amounts of CPU time.
 *
 * @return -1 if any error occurred, 0 otherwise.
 */
int jobs_pause(void) {

/// sigsuspend? 

    // TO BE IMPLEMENTED
    // abort();
    return 0;
}


int count_args(ARG *args) {
    int counter = 0;
    while(args){
        counter += 1;
        args = args -> next ;
    }
    return counter;
}

void insertNewJob(JOB_LINK *newJob){
    JOB_LINK *last =  jobHead.prev;
    newJob -> prev = last;
    newJob -> next = &jobHead;
    jobHead.prev = newJob;
    last -> next = newJob;
}

void sigchldHandler(int s) {
    int leaderStatus; 
    pid_t jobPID;   
    debug("handler triggered");
    while ((jobPID = waitpid(-1, &leaderStatus, WNOHANG )) > 0){      
        debug("jobpid found in handlwe %d",jobPID);

        JOB_LINK *head = &jobHead;
        JOB_LINK *node = jobHead.next;
        JOB_LINK *job = NULL;
        while(node != head){
            if (node -> jobPGID == jobPID){
                job = node;
                break;
            }
            node = node -> next;
        }
        if (job == NULL){
            debug("\t\t\t not job id process found in signal");
            continue;
        }
                // what if cannot find? 
        if (WIFEXITED(leaderStatus)){
            debug("completed process found in handler %d",job -> jobPGID);
            node -> status = COMPLETED_STATUS;
        }
        else{
            debug("canceld process found in handler %d",job -> jobPGID);
            node -> status = CANCEL_STMT_CLASS;
        }
    }
    debug("EXITING HANDLER");
}
