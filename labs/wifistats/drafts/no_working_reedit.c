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
 *      OUIfile is the name of a text-file providing the OUIs and names of Ethernet hardware 
 *      vendors. If an OUIfile is provided, the program should report its statistics not on 
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

#define MAX_NUM_MAC_ADDRESSES   500
#define MAX_NUM_OUIS            25000
#define MAX_LEN_VENDOR_NAME     91
#define MAX_LEN_ADDRESS         18
#define INDEX_NOT_FOUND         -1
#define BROADCAST_MAC_ADDRESS   "ff:ff:ff:ff:ff:ff"
#define DELIMITERS              "\t"

/**
 * @brief A data structure for the requested report query.
 */
typedef struct Request
{
    bool ouifile_provided;
    bool receivers_requested;
    bool transmitters_requested;
    char *packets_filename;
    char *OUIs_filename;
} Request;

/**
 * @brief A data structure for single entry on the report.
 */
typedef struct Entry
{
    char    address[MAX_LEN_ADDRESS];
    char    name[MAX_LEN_VENDOR_NAME];
    int     length;
    bool    broadcast;
} Entry;

/**
 * @brief A data structure for the requested report.
 */
typedef struct Report
{
    Entry   entries[MAX_NUM_MAC_ADDRESSES];
    int     length;
} Report;

typedef struct Vendor
{
    char address[MAX_NUM_MAC_ADDRESSES];
    char name[MAX_LEN_VENDOR_NAME];
} Vendor;

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
        fprintf(stderr, "Need input arguements.");
        exit(EXIT_FAILURE);
    }

    switch (argv[1][0])
    {
    case 'r':
        request.receivers_requested = true;
        break;

    case 't':
        request.transmitters_requested = true;
        break;

    default:
        fprintf(stderr, "Unexpected request: %c", argv[1][0]);
        exit(EXIT_FAILURE);
    }

    request.packets_filename = argv[2];
    return request;
}

/**
 * @brief Check if address is a broadcast. Boardcast are sent to all other address.
 * 
 * @param address   Address to be checked.
 * @return true -   Address is a broadcast.
 * @return false -  Address is not a broadcast.
 */
bool is_broadcast(char *address)
{
    return strcmp(address, BROADCAST_MAC_ADDRESS) == 0;
}

/**
 * @brief Parsing single packet entry in the packet file.
 * 
 * @param request   A data structure of the request data.
 * @param line      A line containing the packet meta data.
 * @return Entry - A data structure for an entry in the report.
 */
Entry parse_packet(const Request *request, char *line)
{
    Entry entry = {0};

    // Packet meta data are seperated by tabs.
    char *token = strtok(line, DELIMITERS);
    if (token != NULL)
    {
        // Parse date time (unused)
        token = strtok(NULL, DELIMITERS);
    }

    if (token != NULL)
    {
        if (request->transmitters_requested)
        {
            strcpy(entry.address, token);
        }
        token = strtok(NULL, DELIMITERS);
    }

    if (token != NULL)
    {
        if (request->receivers_requested)
        {
            strcpy(entry.address, token);
        }
        entry.broadcast = is_broadcast(token);
        token = strtok(NULL, DELIMITERS);
    }

    if (request->ouifile_provided)
    {
        entry.address[3] = '\0';
    }

    if ((token != NULL))
    {
        entry.length = atoi(strtok(NULL, DELIMITERS));
    }

    return entry;
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
 * @brief Parse an entry from the OUI file for vendor data.
 * 
 * @param entry     An entry from the OUI file.
 * @return Vendor - A data structure containing a vendor's data.
 */
Vendor parse_vendor(char *entry)
{
    Vendor vendor = {0};
    char *token;

    // Vendor data are seperated by tabs.
    token = strtok(entry, DELIMITERS);
    if (token != NULL)
    {
        strcpy(vendor.address, token);
        replace_char(vendor.address, '-', ':');
        token = strtok(NULL, DELIMITERS);
    }

    if (token != NULL)
    {
        strcpy(vendor.name, token);
    }

    return vendor;
}

/**
 * @brief Parse the OUI file for vendor data.
 * 
 * @param request A data structure of the request.
 * @return Oui - A data structure containing the vendors data.
 */
Oui parse_OUIfile(Request request)
{
    FILE *fp = fopen(request.OUIs_filename, "r");

    if(fp == NULL) 
    {
        fprintf(stderr, "Cannot open file '%s'\n", request.OUIs_filename);
        exit(EXIT_FAILURE);
    }
    
    Oui report = {0};
    char line[BUFSIZ];

    while (fgets(line, BUFSIZ, fp) != NULL)
    {
        report.vendors[report.length++] = parse_vendor(line);
    }

    fclose(fp);
    return report;
}

/**
 * @brief Finds the index of an entry with a specified value in the Oui.
 * 
 * @param Oui       A data structure for the Oui file.
 * @param value     The value to search for.
 * @return int -    The index of the entry in the Oui or INDEX_NOT_FOUND if not found.
 */
int find_index(const Oui *oui, char *value)
{
    for (int index = 0; index < oui->length; index++)
    {
        if (strcmp(oui->vendors[index].address, value) == 0)
        {
            return index; 
        }
    }

    return INDEX_NOT_FOUND;
}

/**
 * @brief Creates the report.
 * 
 * @param request A data structure of the request.
 * @return Packets - A data structure for the packets data.
 */
Report create_report(Request request)
{
    Report report = {0};
    Oui oui;
    char line[BUFSIZ];
    Entry *last_entry = report.entries;
    Entry unknown_vendor = {.address = "??:??:??", .name = "UNKNOWN-VENDOR"};

    if (request.ouifile_provided)
    {
        oui = parse_OUIfile(request);
    }

    while (fgets(line, BUFSIZ, stdin) != NULL)
    {
        Entry entry = parse_packet(&request, line);

        if (entry.broadcast)
        {
            // Broadcast messages (unused)
            continue;
        }

        if (request.ouifile_provided)
        {
            int index = find_index(&oui, entry.address);
            if (index == INDEX_NOT_FOUND)
            {
                unknown_vendor.length += entry.length;
                continue;
            }
            else
            {
                strcpy(entry.address, oui.vendors[index].name);
            }
        }

        if (strcmp(entry.address, last_entry->address) == 0)
        {
            last_entry->length += entry.length;
        }
        else
        {
            last_entry++;
            *last_entry = entry;
            report.length++;
        }
    }

    if (unknown_vendor.length > 0)
    {
        last_entry++;
        *last_entry = unknown_vendor;
        report.length++;
    }

    return report;
}

/**
 * @brief Using standard utility program /usr/bin/sort to sort the input packets.
 * Sort options: 
 * -t separator character, 
 * -k sorting argument (field number, 'r' reverse, 'n' numeric string, 'd' dictionary)
 *  
 * @param request   A data structure with the request data for the report.
 */
void sort_packets(Request request)
{
    char* args[2];

    if (request.transmitters_requested)
    {
        args[0] = "sort -t '\t' -k 2d";
    }
    else if (request.receivers_requested)
    {
        args[0] = "sort -t '\t' -k 3d";
    }
    else 
    {
        perror("Invalid request.");
        exit(EXIT_FAILURE);
    }

    args[1] = request.packets_filename;

    execv("/usr/bin/", args);
}

/**
 * @brief Using standard utility program /usr/bin/sort to sort the final results.
 * Sort options: 
 * -t separator character, 
 * -k sorting argument (field number, 'r' reverse, 'n' numeric string, 'd' dictionary)
 * @param request   A data structure with the request data for the report.
 */
void sort_report(const Request *request)
{
    char* args[1];

    *args = "sort -t '\t' -k 2rn -k 1d";

    if (request->ouifile_provided)
    {
        *args = "sort -t '\t' -k 3rn -k 1d";
    }

    execv("/usr/bin/", args);
}

/**
 * @brief Prints the report to the standard output.
 * 
 * @param report The report to be printed.
 */
void print_report(const Report *report)
{
    for(int i = 0; i < report->length; i++)
    {  
        printf("%s\t%d\n", report->entries[i].address, report->entries[i].length);
    }
}

/**
 * @brief Prints the vendor report to the standard output.
 * 
 * @param report The report to be printed.
 */
void print_vendor_report(const Report *report)
{
    for(int i = 0; i < report->length; i++)
    {
        char const *address = report->entries[i].address;
        char const *name = report->entries[i].name;
        int length = report->entries[i].length;

        printf("%s\t%s\t%d\n", address, name, length);
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

    int fd[2]; // fd[0] for using read end, fd[1] for using write end.
    int pid;

    pipe(fd);

    // Sorting packets alphabetically to avoid having to impliment a dictionary.
    switch (pid = fork())
    {
    case -1:
        perror("Fail to create child (new) process.");
        exit(1);
        break;
    case 0:
        close(fd[0]);
        sort_packets(request);
        close(fd[1]);
        exit(0);
        break;
    default:
        break;
    }

    close(fd[1]);
    Report report = create_report(request);
    close(fd[0]);

    //Forking program to sort report by highest packets.
    pipe(fd);
    switch (pid = fork())
    {
    case -1:
        perror("Fail to create child (new) process.");
        exit(1);
        break;
    case 0:
        close(fd[1]);
        sort_report(&request);
        close(fd[0]);
        break;
    default:
        close(fd[0]);
        request.ouifile_provided 
            ? print_vendor_report(&report)
            : print_report(&report);
        close(fd[1]);
        break;
    }

    return 0;
}
