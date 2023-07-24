/**
 * wifistats reports the number of bytes either transmitted or received by devices using WIFI.
 * 
 * wifistats can be invoked in two ways:
 *  1.  prompt> ./wifistats what packets
 *      Prints report on individual devices to standard output.
 * 
 *      what is either the single character 't' or 'r' to request statistics about WiFi 
 *      transmitters, or receivers, respectively. For example, if the program is invoked as 
 *      ./wifistats t packets then the program should produce statistics about the data
 *      transmissions of each transmitting device present in the packets file.
 * 
 *      packets is the name of a text-file providing information about each WiFi packet, one 
 *      packet per line. Each line consists of 4 fields, providing the time that each packet was
 *      captured (in seconds and microseconds), the transmitter's and receiver's MAC addresses,
 *      and the length (in bytes) of the packet. Each of the fields will be separated by a single
 *      TAB ('\t') character.
 * 
 *  2.  prompt>  ./wifistats  what  packets  OUIfile
 *      Prints report on vendor that produced each device to standard output.
 * 
 *      OUIfile is the name of a text-file providing the OUI (Organizationally Unique Identifier) 
 *      of each vendor. If an OUIfile is provided, the program should report its statistics not on 
 *      individual device MAC addresses but on the vendor that produced each device. The 2 fields 
 *      will be separated by a single TAB ('\t') character, and the vendors' names may contain 
 *      spaces.
 * 
 * The report is sorted from largest to smallest bytes. The broadcast MAC address used to send a 
 * packet to all devices within range, ff:ff:ff:ff:ff:ff, is ignored in the program.
 * 
 * If OUIfile is provided then the report will be produced by vendor instead.
 * 
 * CITS2002 Project 1 2017
 * Name(s):             Joshua Ng, Benjamin Zhao 
 * Student number(s):   20163079, 21535307 
 * Date:                22/09/2017
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX_NUM_MAC_ADDRESSES   500
#define MAX_NUM_OUIS            25000
#define MAX_LEN_VENDOR_NAME     91
#define MAX_LEN_ADDRESS         17
#define MAX_LEN_VENDOR_ADDRESS  8
#define INDEX_NOT_FOUND         -1
#define BROADCAST_MAC_ADDRESS   "ff:ff:ff:ff:ff:ff"
#define DELIMITERS              "\t\n"
#define UNKNOWN_VENDOR_ADDRESS  "??:??:??"
#define UNKNOWN_VENDOR_NAME     "UNKNOWN-VENDOR"

/**
 * @brief Requested device categories for report.
 */
typedef enum Requested
{
    TRANSMITTERS,
    RECEIVERS
} Requested;

/**
 * @brief A data structure for the requested report query.
 */
typedef struct Request
{
    char *packets_filename;
    char *OUIs_filename;
    Requested requested;
    bool ouifile_provided;
} Request;

/**
 * @brief A data structure a mac device's packet meta data.
 */
typedef struct Mac
{
    char address[MAX_LEN_ADDRESS + 1];
    int length;
    bool broadcast;
} Mac;

/**
 * @brief A data structure for macs.
 */
typedef struct Macs
{
    Mac mac[MAX_NUM_MAC_ADDRESSES];
    int length;
} Macs;

/**
 * @brief A data structure for a line in the report.
 */
typedef struct Entry
{
    const char *address;
    const char *name;
    int     length;
} Entry;

/**
 * @brief A data structure for the report.
 */
typedef struct Report
{
    Entry   entries[MAX_NUM_MAC_ADDRESSES];
    int     length;
} Report;

/**
 * @brief A data structure for the vendor's oui data.
 * 
 */
typedef struct Vendor
{
    char address[MAX_LEN_ADDRESS + 1];
    char name[MAX_LEN_VENDOR_NAME];
} Vendor;

/**
 * @brief A data structure for the oui data.
 */
typedef struct Oui
{
    Vendor vendors[MAX_NUM_OUIS];
    int length;
} Oui;

/**
 * @brief Parses input arguments into the wifi report request.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Request data structure for the requested report.
 */
Request parse_request(int argc, char *argv[])
{
    Request request = {0};

    switch (argc)
    {
    case 3:
        break;

    case 4:
        request.ouifile_provided = true;
        request.OUIs_filename = argv[3];
        break;

    default:
        fprintf(stderr, "Need input arguements.\n");
        exit(EXIT_FAILURE);
    }

    switch (argv[1][0])
    {
    case 'r':
        request.requested = RECEIVERS;
        break;

    case 't':
        request.requested = TRANSMITTERS;
        break;

    default:
        fprintf(stderr, "Unexpected request: %c\n", argv[1][0]);
        exit(EXIT_FAILURE);
    }

    request.packets_filename = argv[2];
    return request;
}

/**
 * @brief Check if address is a broadcast. Boardcast are sent to all other address.
 * 
 * @param address   Address to be checked.
 * @return true if the address is a broadcast.
 */
bool is_broadcast(char *address)
{
    return strcasecmp(address, BROADCAST_MAC_ADDRESS) == 0;
}

/**
 * @brief Parsing a line in the packet file.
 * 
 * @param request   A pointer to the request data.
 * @param line      A line containing the packet meta data.
 * @return Mac - A data structure for a mac's packet meta data.
 */
Mac parse_mac(const Request *request, char *line)
{
    Mac mac = {0};

    // Packet meta data are seperated by tabs.
    char *token = strtok(line, DELIMITERS);
    if (token != NULL)
    {
        // Parse date time (unused)
        token = strtok(NULL, DELIMITERS);
    }

    if (token != NULL)
    {
        if (request->requested == TRANSMITTERS)
        {
            // Parse transmitter address.
            strcpy(mac.address, token);
        }
        token = strtok(NULL, DELIMITERS);
    }

    if (token != NULL)
    {
        if (request->requested == RECEIVERS)
        {
            // Parse reciever address
            strcpy(mac.address, token);
        }
        mac.broadcast = is_broadcast(token);
        token = strtok(NULL, DELIMITERS);
    }

    if (request->ouifile_provided)
    {
        // Vendor only uses first 3 bytes (rest descarded).
        mac.address[MAX_LEN_VENDOR_ADDRESS] = '\0';
    }

    if ((token != NULL))
    {
        // Parse packet data.
        mac.length = atoi(token);
    }

    return mac;
}

/**
 * @brief Replaces all find characters in string with replace characters.
 * 
 * @param str       The string to be edited.
 * @param find      The character to be replaced.
 * @param replace   The character to replace with.
 * @return char*  - A pointer to the string.
 */
char* replace_char(char* str, char find, char replace)
{
    char *found = strchr(str, find);
    while (found)
    {
        *found = replace;
        found = strchr(found, find);
    }

    return str;
}

/**
 * @brief Parse line from the OUI file for vendor data.
 * 
 * @param line      A line from the OUI file.
 * @return Vendor - A data structure containing a vendor's data.
 */
Vendor parse_vendor(char *line)
{
    Vendor vendor = {0};
    char *token;

    // Vendor data are seperated by tabs.
    token = strtok(line, DELIMITERS);
    if (token != NULL)
    {
        // Parse vendor address.
        strcpy(vendor.address, token);
        replace_char(vendor.address, '-', ':');

        token = strtok(NULL, DELIMITERS);
    }

    if (token != NULL)
    {
        // Parse vendor name.
        strcpy(vendor.name, token);
    }

    return vendor;
}

/**
 * @brief Parse the OUI file for vendor data.
 * 
 * @param request   A pointer to the request data.
 * @return Oui -    A pointer to the oui data.
 */
Oui *parse_ouifile(Request *request)
{
    Oui *oui = malloc(sizeof(Oui));

    if (oui == NULL)
    {
        fprintf(stderr, "Failed to allocate memory when parsing oui\n.");
        exit(EXIT_FAILURE);
    }
    
    FILE *fp = fopen(request->OUIs_filename, "r");

    if(fp == NULL) 
    {
        fprintf(stderr, "Cannot open file '%s'\n", request->OUIs_filename);
        exit(EXIT_FAILURE);
    }
    
    char line[BUFSIZ];

    while (fgets(line, BUFSIZ, fp) != NULL)
    {
        if(oui->length >= MAX_NUM_OUIS) 
        {
            fprintf(stderr, "Parsing too many unique oui address.\n");
            exit(EXIT_FAILURE);
        }

        oui->vendors[oui->length++] = parse_vendor(line);
    }

    fclose(fp);

    return oui;
}

/**
 * @brief Finds the index of an entry with a specified value in the Oui.
 * 
 * @param Oui       A pointer to the oui data.
 * @param address   The string data.
 * @return int -    The index of the entry in the Oui or INDEX_NOT_FOUND if not found.
 */
int find_index_oui(const Oui *oui, const char *address)
{
    for (int index = 0; index < oui->length; index++)
    {
        if (strcasecmp(oui->vendors[index].address, address) == 0)
        {
            return index; 
        }
    }

    return INDEX_NOT_FOUND;
}

/**
 * @brief Finds the index of a mac with a particular address.
 * 
 * @param macs      A pointer to the macs data.
 * @param address   The address to search for.
 * @return int -    The index of the entry in the Oui or INDEX_NOT_FOUND if not found.
 */
int find_index_macs(const Macs *macs, const char *address)
{
    for (int index = 0; index < macs->length; index++)
    {
        if (strcasecmp(macs->mac[index].address, address) == 0)
        {
            return index; 
        }
    }

    return INDEX_NOT_FOUND;
}

/**
 * @brief Parses the packet file for the macs' packet meta data.
 * 
 * @param request   A pointer to the request data.
 * @return Packets - A pointer to the macs data.
 */
Macs *parse_macs(const Request *request)
{
    Macs *macs = malloc(sizeof(Macs));

    if (macs == NULL)
    {
        fprintf(stderr, "Failed to allocate memory when parsing macs.");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(request->packets_filename, "r");

    if(fp == NULL) 
    {
        fprintf(stderr, "Cannot open file '%s'\n", request->packets_filename);
        exit(EXIT_FAILURE);
    }

    char line[BUFSIZ];

    while (fgets(line, BUFSIZ, fp) != NULL)
    {
        Mac mac = parse_mac(request, line);

        if (mac.broadcast)
        {
            // Broadcast messages (unused)
            continue;
        }

        int index = find_index_macs(macs, mac.address);

        if (index == INDEX_NOT_FOUND)
        {
            if(macs->length >= MAX_NUM_MAC_ADDRESSES) 
            {
                fprintf(stderr, "Parsing too many unique mac address.\n");
                exit(EXIT_FAILURE);
            }

            macs->mac[macs->length++] = mac;
        }
        else
        {
            macs->mac[index].length += mac.length;
        }
    }

    fclose(fp);
    
    return macs;
}

/**
 * @brief Creates a report.
 * 
 * @param macs      A pointer to the macs data.
 * @param oui       A pointer to the oui data.
 * @param request   A pointer to the request data.
 * @return Report   A data structure for the report.
 */
Report create_report(const Macs *macs, const Oui *oui, const Request *request)
{
    Report report = {0};

    if (request->ouifile_provided)
    {
        int unknown_packets = 0;

        for (int i = 0; i < macs->length; ++i)
        {
            int index = find_index_oui(oui, macs->mac[i].address);
            if (index == INDEX_NOT_FOUND)
            {
                unknown_packets += macs->mac[i].length;
            }
            else
            {
                Entry *entry = &report.entries[report.length++];
                entry->address = macs->mac[i].address;
                entry->name = oui->vendors[index].name;
                entry->length = macs->mac[i].length;
            }
        }

        if (unknown_packets > 0)
        {
            Entry *entry = &report.entries[report.length++];
            entry->address = UNKNOWN_VENDOR_ADDRESS;
            entry->name = UNKNOWN_VENDOR_NAME;
            entry->length = unknown_packets;
        }
    }
    else
    {
        for (int i = 0; i < macs->length; i++)
        {
            Entry *entry = &report.entries[report.length++];
            entry->address = macs->mac[i].address;
            entry->length = macs->mac[i].length;
        }
    }

    return report;
}

/**
 * @brief Runs standard utility program /usr/bin/sort to sort the final results.
 * Sort options: 
 * -t separator character, 
 * -k sorting argument (field number, 'r' reverse, 'n' numeric string, 'd' dictionary)
 * @param request   A data structure with the request data for the report.
 */
void sort_report(const Request *request)
{
    char *sort_path = "/usr/bin/sort";
    char *separator = "\t";
    char *by_numeric = "2rn";
    char *by_alphabet = "1d";
    char *terminator = NULL; // must include!

    if (request->ouifile_provided)
    {
        by_numeric = "3rn";
    }

    // By convension, execv arguement array must include execution path with its arguments.
    // The arguement array MUST also be terminated by a NULL pointer. 
    char* argv[] = {sort_path, "-t", separator, "-k", by_numeric, "-k", by_alphabet, terminator};

    execv(sort_path, argv);
}

/**
 * @brief Prints the report to the standard output.
 * 
 * @param report The report to be printed.
 */
void print_report(const Report *report, const Request *request)
{
    if (request->ouifile_provided)
    {
        for(int i = 0; i < report->length; i++)
        {
            char const *address = report->entries[i].address;
            char const *name = report->entries[i].name;
            int length = report->entries[i].length;

            printf("%s\t%s\t%d\n", address, name, length);
        }
    }
    else
    {
        for(int i = 0; i < report->length; i++)
        {  
            printf("%s\t%d\n", report->entries[i].address, report->entries[i].length);
        }
    }
}

/**
 * @brief Program entry.
 * @param argc  Argument count.
 * @param argv  Argument vector.
 * @return int  Program exit status.
 */
int main(int argc, char *argv[])
{
    Request request = parse_request(argc, argv);

    Macs *macs = parse_macs(&request);

    Oui *oui = NULL;
    if (request.ouifile_provided)
    {
        oui = parse_ouifile(&request);
    }

    Report report = create_report(macs, oui, &request);

    int pid;
    int fd[2];  // Array to hold pipe. 0 for read, 1 for write
    pipe(fd);   // Create pipe.

    switch (pid = fork())   // fork process
    {
    case -1:
        perror("Fail to create child (new) process.\n");
        exit(EXIT_FAILURE);
        break;
    case 0:                     // Child process
        close(fd[1]);           // Close pipe's write stream.
        dup2(fd[0], 0);         // Replace stdin stream with pipe's read stream.
        close(fd[0]);           // Close extra copy of pipe's read stream.
        sort_report(&request);  // Sort read stream until end-of-file received. 
        exit(EXIT_FAILURE);     // Should only return on execv failure.
        break;
    default:                    // Parent process
        close(fd[0]);           // Close pipe's read stream.
        dup2(fd[1], 1);         // Replace stdout stream with pipe's write stream.
        close(fd[1]);           // Close extra copy of pipe's write stream.
        print_report(&report, &request);    // Print results to pipe's write stream.
        fflush(stdout);         // Flush all results through.
        close(1);               // Close stdout (pipe's write stream) which sends end-of-file.
        wait(NULL);             // Wait for child to finish printing.
        break;
    }

    free(macs);
    free(oui);

    return EXIT_SUCCESS;
}
