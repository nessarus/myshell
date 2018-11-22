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
#include <ctype.h>

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
void sumbytes(char *str, int bytes, char *address[], 
        int *byt, int *length, char *OUIs[], int *OUIs_num, int *ven_list) {
    char *string;
    if(OUIs != NULL) {
        str[8] = '\0';
        ven_list[*length] = existsIn(str,OUIs,*OUIs_num);
        if(ven_list[*length]!=-1){
            string = str;
        } else {
            string = "??:??:??";
        }
    } else {
        string = str;
    }
    int exist = existsIn(string, address, *length);
    if(exist == -1){
        strcpy(address[*length],string);
        byt[*length] = bytes;
        (*length)++;

    }
    if(exist == -2){
    }
    else {
        byt[exist] += bytes;
    }
}

/**
 * recordMAC opens the file and records the columns, col1
 * and col2, into MAC address and byte/vendor array, *address[] and 
 * *byt / *OUIs[].
 */
void recordMAC(char filename[], char *address[],int *nd, int *byt,  
        int col1,int col2, char *OUIs[], int *OUIs_num, int *ven_list) {
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
        char cell1[90];
        char cell2[90];
        int bytes=0;
        int length = strlen(line);
        int cell_num=0;
        int cell_start=0;

        //go through line looking for '\t'
        //and record relevent column between '\t'.
        for(int i=0; i<length+1; i++) {
            if(line[i]=='\t' || line[i]=='\0' || line[i]=='\n') {
                //store column 1
                if(cell_num==col1) {
                    //strncpy splits line and records into cell
                    memset(cell1, '\0', sizeof(cell1)); 
                    strncpy(cell1, &line[cell_start], i-cell_start);
                } 
                //store column 2
                else if(cell_num==col2) {
                    memset(cell2, '\0', sizeof(cell2));
                    strncpy(cell2, &line[cell_start], i-cell_start);
                    if(byt != NULL){
                        bytes  = atoi(cell2);
                    }
                }
                //progress to next cell ('\t') through line 
                cell_num++;
                cell_start = i+1;
            }
        }

        //store device and sum its bytes
        if(byt != NULL){
            sumbytes(cell1, bytes, address, byt, nd, OUIs, OUIs_num, ven_list);
        }
        //store OUIfile
        else {
            cell1[2]=':';
            cell1[5]=':';
            for(int i = 0; cell1[i]; i++){
                cell1[i] = tolower(cell1[i]);
            }
            strcpy(address[m],cell1);
            strcpy(OUIs[m],cell2);
            (*OUIs_num)++;
        }
        m++; //next line
    }
}


/**
 * sortNprint recieves the devices, bytes and number 
 * of devices, and sort them and print them in 
 * stdout.
 */
void sortNprint(char *dev[], int *byt, int num, char *ven[], int *ven_list)
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

            char *k;
            char *l = NULL;
            if(ven == NULL) {
                k = "-k2rn";
            } else {
                k = "-k3rn";
                l = "-k1";
            }
            char *newarg[]={"sort","-t\t",k,l,NULL};
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
                if(ven == NULL) {
                    printf("%s\t%d\n", dev[i], byt[i]);
                } else {
                    char *vendor_name;
                    if(ven_list[i]==-1) {
                        vendor_name = "UNKOWN-VENDOR";
                    } else {
                        vendor_name = ven[ven_list[i]];
                    }
                    printf("%s\t%s\t%d\n", dev[i], vendor_name, byt[i]);
                }
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

    int num_dev = 0;
    int bytes[500];
    char *devices[500];
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
                            recordMAC(argv[2],devices,&num_dev,
                                    bytes,1,3,NULL, NULL, NULL);
                            sortNprint(devices, bytes, num_dev, NULL, NULL);
                            break;
                        }

                        /* request for receivers */
                    case 'r':
                        {
                            recordMAC(argv[2],devices,&num_dev,
                                    bytes,2,3, NULL, NULL, NULL);
                            sortNprint(devices, bytes, num_dev,NULL, NULL);
                            break;
                        }
                }       
                break;
            }

            /* report on vendor that produced each device
             * i.e. prompt> ./wifistats what packets OUIfile */
        case 4:
            {
                int OUIs_num = 0;
                char *OUIs[25000];
                char *vendors[25000];
                for(int i=0; i<25000;i++) {
                    OUIs[i] = (char*)malloc(90*sizeof(char));
                    vendors[i] = (char*)malloc(90*sizeof(char));
                }
                int ven_list[500];

                switch (argv[1][0])
                {
                    /* request for transmitters */
                    case 't':
                        {
                            //store OUIfile
                            recordMAC(argv[3],OUIs,NULL,
                                    NULL,0,1,vendors, &OUIs_num,NULL);
                            //store packets
                            recordMAC(argv[2],devices,&num_dev,
                                    bytes,1,3,OUIs,&OUIs_num,ven_list);
                            sortNprint(devices, bytes, num_dev,vendors, ven_list);
                            break;
                        }

                        /* request for receivers */
                    case 'r':
                        {
                            //store OUIfile
                            recordMAC(argv[3],OUIs,NULL,
                                    NULL,0,1,vendors, &OUIs_num,NULL);
                            //store packets
                            recordMAC(argv[2],devices,&num_dev,
                                    bytes,2,3,OUIs,&OUIs_num,ven_list);
                            sortNprint(devices, bytes, num_dev,vendors, ven_list);
                            break;
                        }
                }
                break;
            }

    }

    return 0;
}

