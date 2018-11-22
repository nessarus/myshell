/*
 * CITS2002 Project 1 2017
 * Name(s):             Joshua Ng, Benjamin Zhao 
 * Student number(s):   20163079, 21535307 
 * Date:                date-of-submission
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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


/**
 * existsIn receives a string and checks if it is in 
 * the array. It returns the position in the array.
 */ 
int existsIn(char *string, char *array[], int length) {
    for(int i=0; i<length; i++) {
        if(strcmp(string, array[i])==0){
            return i;
        }
        else if(strcmp(string,"ff:ff:ff:ff:ff:ff")==0) {
            return -2;
        }
    }
    return -1;
}

// sumbytes should remove duplicate devices and 
// sum the bytes. It also should return the number
// of devices left.
void sumbytes(char *string, int bytes, 
        char *dev[], int *byt, int *length) {

    int exist = existsIn(string, dev, *length);
    if(exist == -1){
        strcpy(dev[*length],string);
        byt[*length] = bytes;
        (*length)++;

    }
    if(exist == -2){
        printf("ff\n");
    }
    else {
        byt[exist] += bytes;
    }
}

/**
 * recordMAC opens the file and records the columns, col1
 * and col2, into devices and byte array, *dev[] and *byt.
 */
void recordMAC(char filename[], char *dev[],int *nd, int *byt,  
        int col1,int col2) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("cannot open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[BUFSIZ];

    int m = 0;
    //open file and go line by line
    while( fgets(line, sizeof line, fp) != NULL)
    {
        char device[90];
        char bytes_s[90];
        int bytes=0;
        int length = strlen(line);
        int cell_num=0;
        int cell_start=0;

        //go through line looking for '\t'
        //and record relevnt column between '\t'.
        for(int i=0; i<length+1; i++) {
            if(line[i]=='\t' || line[i]=='\0') {
                //store column 1
                if(cell_num==col1) {
                    //strncpy splits line and records into cell
                    memset(device, '\0', sizeof(device)); 
                    strncpy(device, &line[cell_start], i-cell_start);
                } 
                //store column 2
                else if(cell_num==col2) {
                    memset(bytes_s, '\0', sizeof(bytes_s));
                    strncpy(bytes_s, &line[cell_start], i-cell_start);
                    bytes  = atoi(bytes_s);
                }

                //progress cells ('\t') through line
                cell_num++;
                cell_start = i+1;
            }
        }
        //store device and sum its bytes
        sumbytes(device, bytes, dev, byt, nd);
        m++; //next line
    }
}


/**
 * sortNprint recieves the devices, bytes and number 
 * of devices, and sort them and print them in 
 * stdout.
 */
void sortNprint(char *dev[], int *byt, int num)
{
    int     fd[2]; 
    int     pid;

    pipe(fd); //pipe

    switch (pid = fork()) //run fork, splits process
    { 
        case -1 :
            perror("fork");
            exit(1);

        case 0: //child
            close(fd[1]); //close piping back to parent
            dup2(fd[0],0); //connect pipe to child 

            char *newarg[] = {"sort", NULL};
            execv("/usr/bin/sort", newarg);
            exit(20);
            break;

        default: //parent
            sleep(1);
            close(fd[0]); //stop child piping back
            dup2(fd[1],1); //connect pipe to child

            for(int i=0; i<num; i++)
            {  
                // send line through the pipe
                printf("%s\t%d\n", dev[i], byt[i]);
            }
            break;
    }
}

int main(int argc, char *argv[])
{
    if(argc != 3 && argc != 4)  
    {
        perror("Unexpected arguements. ");
        exit(EXIT_FAILURE);
    }

    char *devices[500];
    int num_dev = 0;
    int bytes[500];

    for(int i=0; i<500;i++) {
        devices[i] = (char*)malloc(90*sizeof(char));
    }

    switch (argc) 
    {
        /* report on individual devices
         * i.e. prompt>  ./wifistats  what  packets */
        case 3:
            {
                switch (argv[1][0]) 
                {
                    /* request for transmitters */
                    case 't':
                        {
                            recordMAC(argv[2],devices,&num_dev,bytes,1,3);
                            sortNprint(devices, bytes, num_dev);
                            break;
                        }

                        /* request for receivers */
                    case 'r':
                        {
                            recordMAC(argv[2],devices,&num_dev,bytes,2,3);
                            sortNprint(devices, bytes, num_dev);
                            break;
                        }
                }       
                break;
            }

            /* report on vendor that produced each device
             * i.e. prompt> ./wifistats what packets OUIfile */
        case 4:
            {
                switch (argv[1][0])
                {
                    /* request for transmitters */
                    case 't':
                        {
                            break;
                        }

                        /* request for receivers */
                    case 'r':
                        {
                            // do something
                            break;
                        }
                }
                break;
            }

    }

    return 0;
}

