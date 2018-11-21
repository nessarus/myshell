//  A sample solution to the CITS2002 1st project, 2017.
//  Written by Chris.McDonald@uwa.edu.au, September 2017.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#if     defined(__linux__)
#include <sys/wait.h>
#endif

//  DEFINE THE LIMITS ON PROGRAM'S DATA-STRUCTURES
#define MAX_MACs                500
#define MAX_OUIs                25000

#define LEN_MAC                 18              // includes the null-byte
#define LEN_OUI                 9               // includes the null-byte
#define LEN_VENDOR              91              // includes the null-byte

#define UNKNOWN_OUI_INDEX       0
#define UNKNOWN_OUI_OUI         "??:??:??"
#define UNKNOWN_OUI_VENDOR      "UNKNOWN-VENDOR"

#define BROADCAST_MAC           "ff:ff:ff:ff:ff:ff"

#define TMP_FILENAME            "/tmp/wifistats"

//  THE FULL PATHNAME OF THE sort PROGRAM
#if     defined(__APPLE__)
#define SORT_PATHNAME           "/usr/bin/sort"

#elif   defined(__linux__)
#define SORT_PATHNAME           "/bin/sort"
#endif

//  ARRAYS HOLDING EACH OBSERVED MAC ADDRESS AND ITS WIFI STATISTICS
char    packet_MACs[MAX_MACs][LEN_MAC];
int     packet_bytes[MAX_MACs];

int     nMACs           = 0;                    // will range 0..(MAX_MACs-1)

//  ARRAYS HOLDING EACH KNOWN OUI, VENDOR NAME, AND ITS WIFI STATISTICS
char    vendor_OUI[MAX_OUIs][LEN_OUI];
char    vendor_name[MAX_OUIs][LEN_VENDOR];
int     vendor_bytes[MAX_OUIs];

int     nvendors        = 0;                    // will range 0..(MAX_OUIs-1)

//  REMEMBER OUR PROGRAM'S NAME, USED WHEN PRINTING ERROR MESSAGES
char    *progname;

//  -----------------------------------------------------------------------

//  ACCUMULATE WIFI STATS FOR THE INDICATED MAC ADDRESS
void add_bytes_by_MAC(char MAC[], int nbytes)
{
    for(int m=0 ; m < nMACs ; m=m+1) {          // foreach observed MAC address
        if(strcmp(packet_MACs[m], MAC) == 0) {  // this MAC seen before?
            packet_bytes[m]     += nbytes;
            return;
        }
    }
    strcpy(packet_MACs[nMACs],  MAC);           // remember this new MAC
    packet_bytes[nMACs] = nbytes;
    nMACs = nMACs+1;
}

//  ACCUMULATE WIFI STATS FOR THE INDICATED MAC ADDRESS (-> OUI)
void add_bytes_by_OUI(char MAC[], int nbytes)
{
    char OUI[LEN_OUI];

    strncpy(OUI, MAC, LEN_OUI-1);               // save OUI part of this MAC
    OUI[LEN_OUI-1]      = '\0';

    for(int v=1 ; v < nvendors ; v=v+1) {       // foreach known vendor
        if(strcmp(vendor_OUI[v], OUI) == 0) {   // one of our known OUIs?
            vendor_bytes[v]     += nbytes;
            return;
        }
    }
    vendor_bytes[UNKNOWN_OUI_INDEX] += nbytes;  // it's an unknown OUI
}

//  -----------------------------------------------------------------------

//  WRITE WIFI STATISTICS FOR EACH OBSERVED MAC ADDRESS
void write_bytes_by_MAC(FILE *fp)
{
    for(int m=0 ; m < nMACs ; m=m+1) {          // foreach observed MAC address
        fprintf(fp, "%s\t%i\n", packet_MACs[m], packet_bytes[m]);
    }
}

//  WRITE (NON-ZERO) WIFI STATISTICS FOR OUI AND VENDOR
void write_bytes_by_OUI(FILE *fp)
{
    for(int v=0 ; v < nvendors ; v=v+1) {       // foreach vendor (and UNKNOWN)
        if(vendor_bytes[v] > 0) {
            fprintf(fp, "%s\t%s\t%i\n",
                        vendor_OUI[v], vendor_name[v], vendor_bytes[v]);
        }
    }
}

//  -----------------------------------------------------------------------

//  CONVERT UPPERCASE CHARS TO LOWERCASE, HYPHENS TO COLONS
void standardize_MAC(char MAC[])
{
    int m       = 0;

    while(MAC[m] != '\0') {                     // traverse the MAC string
        if(MAC[m] == '-') {                     // convert hyphen to colon
            MAC[m]      = ':';
        }
        else if(isupper(MAC[m])) {              // convert upper to lower
            MAC[m]      = tolower(MAC[m]);
        }
        m = m+1;
    }
}

//  -----------------------------------------------------------------------

//  REMOVE ANY TRAILING NEW-LINE OR CARRIAGE-RETURN (see Lecture 9)
void trim_line(char line[])
{
    int i = 0;

    while(line[i] != '\0') {                    // scan line until null-byte
        if(line[i] == '\r' || line[i] == '\n') {// found CR or NL ?
            line[i] = '\0';                     // replace CR/NL with null-byte
            break;
        }
        i = i+1;
    }
}

#define MAX_FIELDS      10
#define MAX_FIELD_LEN   100

char fields[MAX_FIELDS][MAX_FIELD_LEN];

//  BREAK A LINE OF TEXT INTO TAB-SEPARATED FIELDS
int find_fields(char line[])
{
    int i  = 0;
    int nf = 0;

    trim_line(line);            // remove end-of-line characters (from Lecture 9)
    while(line[i] != '\0') {
        int len = 0;

        while(line[i] != '\0' && line[i] != '\t') {
            fields[nf][len] = line[i];
            len = len+1;
            i   = i+1;
        }
        fields[nf][len] = '\0';
        nf = nf+1;
        i  = i+1;
    }
    return nf;
}

//  -----------------------------------------------------------------------

#define FIELD_PACKET_TRANSMITTER        1
#define FIELD_PACKET_RECEIVER           2
#define FIELD_PACKET_BYTES              3

//  READ PACKET FILE, EACH LINE CONTAINS: time, transmitter, receiver, bytes
void read_packets(char filename[], bool countTX, bool byOUI)
{
    FILE        *fp = fopen(filename, "r");     // open for reading
    char        line[BUFSIZ];
    int         wanted;                         // which field is wanted?

    if(fp == NULL) {
        printf("%s: unable to open packets file '%s'\n", progname, filename);
        exit(EXIT_FAILURE);
    }

    if(countTX) {
        wanted  = FIELD_PACKET_TRANSMITTER;
    }
    else {
        wanted  = FIELD_PACKET_RECEIVER;
    }

    while(fgets(line, sizeof line, fp) != NULL) { // read until the end-of-file
        find_fields(line);

//  WE IGNORE BROADCAST PACKETS (DESTINATION MAC IS ff:ff:ff:ff:ff:ff)
        standardize_MAC( fields[FIELD_PACKET_RECEIVER] );
        if(strcmp(fields[FIELD_PACKET_RECEIVER], BROADCAST_MAC) != 0) {
            int nbytes  = atoi(fields[FIELD_PACKET_BYTES]);

            if(wanted == FIELD_PACKET_TRANSMITTER) {
                standardize_MAC( fields[FIELD_PACKET_TRANSMITTER] );
            }
            if(byOUI) {
                add_bytes_by_OUI(fields[wanted], nbytes);
            }
            else {
                add_bytes_by_MAC(fields[wanted], nbytes);
            }
        }
    }
    fclose(fp);                                 // we opened it => we close it
}

//  SAVE INFORMATION ABOUT NEW OUI
void add_vendor(char OUI[], char name[])
{
    if(nvendors == 0) {                        // first time, initialize arrays
        strcpy(vendor_OUI [UNKNOWN_OUI_INDEX], UNKNOWN_OUI_OUI);
        strcpy(vendor_name[UNKNOWN_OUI_INDEX], UNKNOWN_OUI_VENDOR);
        vendor_bytes[UNKNOWN_OUI_INDEX] = 0;
        nvendors        = 1;
    }

    for(int v=1 ; v < nvendors ; v=v+1) {       // foreach known vendor
        if(strcmp(vendor_OUI[v], OUI) == 0) {   // duplicate OUI  (an error?)
            return;
        }
    }
    strcpy(vendor_OUI[nvendors],  OUI);         // save new OUI information
    strcpy(vendor_name[nvendors], name);
    vendor_bytes[nvendors]      = 0;            // initialize byte count
    nvendors = nvendors+1;                      // increase number of OUIs
}

#define FIELD_VENDOR_OUI                0
#define FIELD_VENDOR_NAME               1

//  READ OUI/VENDOR FILE, EACH LINE CONTAINS: OUI, vendor-name
void read_vendors(char filename[])
{
    FILE        *fp = fopen(filename, "r");     // open for reading
    char        line[BUFSIZ];

    if(fp == NULL) {
        printf("%s: unable to open OUIfile '%s'\n", progname, filename);
        exit(EXIT_FAILURE);
    }

    while(fgets(line, sizeof line, fp) != NULL) { // read until the end-of-file
        find_fields(line);
        standardize_MAC(fields[FIELD_VENDOR_OUI]);

        add_vendor(fields[FIELD_VENDOR_OUI], fields[FIELD_VENDOR_NAME]);
    }
    fclose(fp);                                 // we opened it => we close it
}

//  -----------------------------------------------------------------------

//  CREATE A NEW PROCESS TO RUN sort TO SORT THE FINAL RESULTS
void sort_bytes(bool byOUI)
{
    FILE        *fp = fopen(TMP_FILENAME, "w"); // open temp file for writing

    if(fp == NULL) {
        printf("%s: unable to create file '%s'\n", progname, TMP_FILENAME);
        exit(EXIT_FAILURE);
    }

//  WRITE WIFI STATISTICS TO THE TEMPORARY FILE
    if(byOUI) {
        write_bytes_by_OUI(fp);
    }
    else {
        write_bytes_by_MAC(fp);
    }
    fclose(fp);                                 // we opened it => we close it

//  SPAWN A NEW PROCESS, PARENT KEEPS RUNNING wifistats, CHILD WILL BECOME sort
    int pid = fork();

//  ENSURE THAT A NEW PROCESS WAS CREATED
    if(pid == -1) {                             // process creation failed
        printf("%s: fork() failed\n", progname);
        exit(EXIT_FAILURE);
    }
//  THE NEW CHILD PROCESS RUNS THE sort PROGRAM WHICH PRINTS FINAL OUTPUT
    else if(pid == 0) {
        char    *sort_arguments[20];            // should be enough(!)
        int     a=0;

        sort_arguments[a++]     = "sort";
        sort_arguments[a++]     = "-t\t";       // field delimiter is \t
        if(byOUI) {
            sort_arguments[a++] = "-k3,3nr";    // numeric, descending
            sort_arguments[a++] = "-k2,2";      // alphabetic, ascending
        }
        else {
            sort_arguments[a++] = "-k2,2nr";    // numeric, descending
        }
        sort_arguments[a++]     = TMP_FILENAME; // input filename
        sort_arguments[a  ]     = NULL;

        execv( SORT_PATHNAME, sort_arguments );

        printf("%s: execv() failed\n", progname);
        exit(EXIT_FAILURE);
    }
//  THE CONTINUING PARENT PROCESS WAITS FOR THE sort PROGRAM
    else {
        int     status;

        while( wait(&status) > 0) {     // capture (but ignore) sort's exit status
            /* nothing to do */ ;
        }
        unlink(TMP_FILENAME);           // parent removes the temporary file
    }
}

//  -----------------------------------------------------------------------

void usage(void)
{
    printf("Usage: %s t|r packets [OUIfile]\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    bool        countTX;

//  REMEMBER OUR PROGRAM'S NAME, USED WHEN PRINTING ERROR MESSAGES
    progname    = argv[0];

//  INCORRECT NUMBER OF ARGUMENTS PROVIDED?
    if(argc != 3 && argc != 4) {
        usage();
    }

//  RECORD WHETHER TO COUNT TRANSMITTED OR RECEIVED BYTES
    if(strcmp(argv[1], "t") == 0) {
        countTX = true;
    }
    else if(strcmp(argv[1], "r") == 0) {
        countTX = false;
    }
    else {
        usage();
    }

//  ENSURE THAT WE'VE BEEN GIVEN ALL REQUIRED FILENAMES
    if(argc == 3) {                     // ./wifistat what packets-file
        read_packets(argv[2], countTX, false);
        sort_bytes(false);
    }
    else {                              // ./wifistat what packets-file OUIfile
        read_vendors(argv[3]);
        read_packets(argv[2], countTX, true);
        sort_bytes(true);
    }

    exit(EXIT_SUCCESS);
    return 0;
}
