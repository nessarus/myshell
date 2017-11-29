/**
 * CITS2002 Project 1 2017
 * Name(s):             Joshua Ng, Benjamin Zhao 
 * Student number(s):   20163079, 21535307 
 * Date:                date-of-submission
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * wifistats reports the number of bytes either 
 * transmitted or received by devices using WIFI. 
 * Report is sorted from largest to smallest bytes.
 *
 * prompt> ./wifistats what packets OUIfile
 *
 * what: use 't' option for transmitters
 * what: use 'r' option for receivers
 * packets: name of text-file for each WIFI packet
 * OUIfile(optional): name of text-file for vendors
 * 
 * If OUIfile is provided then report will be produced
 * on the vendor that produced each device.
 */

// sample fork file
/*
   void function(void)
   {
   int pid; // some system define a pid_t

   switch (pid = fork()) {
   case -1:
   perror("fork()"); // process creation failed
   exit(0);
   break;

   case 0:
   printf("c:  value of pid=%i\n", pid);
   printf("c:  child's pid=%i\n", getpid());
   printf("c:  child's parent pid=%i\n", getppid());
   break;

   default:                  // original parent process
   sleep(1);
   printf("p:  value of pid=%i\n", pid);
   printf("p:  parent's pid=%i\n", getpid());
   printf("p:  parent's parent pid=%i\n", getppid());
   break;
   }
   fflush(stdout);
   }
   */

/*
void sort_devices(FILE *fileName) {
    int pid;
    char *newargs[] = {
        "sort " + fileName,
        NULL
    };
    char *environ[]={0};

    switch (pid = fork()) {
        case -1:
            perror("fork()"); // process creation failed
            exit(0);
            break;

        case 0:
            execve("usr/bin/sort", newargs, environ);
            exit(EXIT_FAILURE);
            break;
}
*/

int main(int argc, char *argv[])
{
    switch (argc) {
        /* report on individual devices
         * i.e. prompt>  ./wifistats  what  packets */
        case 3:
            switch (argv[1][0]) {
                /* request for transmitters */
                case 't':
                    // do something        
                    break;

                /* request for receivers */
                case 'r':
                    // do something
                    break;
            }        
            break;

        /* report on vendor that produced each device
         * i.e. prompt> ./wifistats what packets OUIfile */
        case 4:
            switch (argv[1][0]) {
                /* request for transmitters */
                case 't':
                    // do something        
                    break;

                /* request for receivers */
                case 'r':
                    // do something
                    break;
            }
            break;

        default:
            perror("unexpected arguements.");
            exit(EXIT_FAILURE);
            break;
    }

    //    function();
    return 0;
}

