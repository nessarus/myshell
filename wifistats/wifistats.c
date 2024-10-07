/**
 * wifistats reports the number of bytes either transmitted or received by
 * devices using WIFI.
 *
 * wifistats can be invoked in two ways:
 *  1.  prompt> ./wifistats what packets
 *      Prints report on individual devices to standard output.
 *
 *      what is either the single character 't' or 'r' to request statistics
 *      about WiFi transmitters, or receivers, respectively. For example, if
 *      the program is invoked as ./wifistats t packets then the program
 *      should produce statistics about the data transmissions of each
 *      transmitting device present in the packets file.
 *
 *      packets is the name of a text-file providing information about each
 *      WiFi packet, one packet per line. Each line consists of 4 fields,
 *      providing the time that each packet was captured (in seconds and
 *      microseconds), the transmitter's and receiver's MAC addresses, and the
 *      length (in bytes) of the packet. Each of the fields will be separated by
 *      a single TAB ('\t') character.
 *
 *  2.  prompt>  ./wifistats  what  packets  OUIfile
 *      Prints report on vendor that produced each device to standard output.
 *
 *      OUIfile is the name of a text-file providing the OUI (Organizationally
 *      Unique Identifier) of each vendor. If an OUIfile is provided, the
 *      program should report its statistics not on individual device MAC
 *      addresses but on the vendor that produced each device. The 2 fields
 *      will be separated by a single TAB ('\t') character, and the vendors'
 *      names may contain spaces.
 *
 * The report is sorted from largest to smallest bytes. The broadcast MAC
 * address used to send a packet to all devices within range, ff:ff:ff:ff:ff:ff,
 * is ignored in the program.
 *
 * If OUIfile is provided then the report will be produced by vendor instead.
 *
 * CITS2002 Project 1 2017
 * Name(s):             Joshua Ng, Benjamin Zhao
 * Student number(s):   20163079, 21535307
 * Date:                22/09/2017
 */

#include "hashmap.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_NUM_MAC_ADDRESSES 500
#define MAX_NUM_OUIS 25000
#define MAX_LEN_VENDOR_NAME 91
#define MAX_LEN_ADDRESS 6
#define MAX_LEN_VENDOR_ADDRESS 3
#define UNKNOWN_VENDOR_ADDRESS "??:??:??"
#define UNKNOWN_VENDOR_NAME "UNKNOWN-VENDOR"

const uint64_t BROADCAST = 0xffffffffffff;

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
typedef union Mac
{
    uint8_t address[MAX_LEN_ADDRESS];
    uint64_t data;
} Mac;

typedef struct Packet
{
    Mac transmitter;
    Mac receiver;
    uint32_t bytes;
} Packet;

/**
 * @brief A data structure for macs.
 */
typedef struct Macs
{
    Mac entries[MAX_NUM_MAC_ADDRESSES];
    uint32_t bytes[MAX_NUM_MAC_ADDRESSES];
    NODE nodes[MAX_NUM_MAC_ADDRESSES];
    HASHMAP map;
} Macs;

/**
 * @brief A data structure for a line in the report.
 */
typedef union Entry
{
    uint8_t address[MAX_LEN_ADDRESS];
    uint64_t data;
} Entry;

/**
 * @brief A data structure for the report.
 */
typedef struct Report
{
    Entry entries[MAX_NUM_MAC_ADDRESSES];
    const char *names[MAX_NUM_MAC_ADDRESSES];
    uint32_t bytes[MAX_NUM_MAC_ADDRESSES];
    uint32_t unknown_bytes;
    int length;
} Report;

/**
 * @brief A data structure for the vendor's oui data.
 */
typedef union Vendor
{
    uint8_t address[MAX_LEN_VENDOR_ADDRESS];
    uint32_t data;
} Vendor;

/**
 * @brief Parsed vendor data.
 */
typedef struct VendorParse
{
    Vendor vendor;
    char *name;
} VendorParse;

/**
 * @brief A data structure for the oui data.
 */
typedef struct Vendors
{
    Vendor entries[MAX_NUM_OUIS];
    char names[MAX_NUM_OUIS][MAX_LEN_VENDOR_NAME];
    NODE nodes[MAX_NUM_OUIS];
    HASHMAP map;
} Vendors;

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
bool is_broadcast(uint64_t address)
{
    return address == BROADCAST;
}

/**
 * @brief Parses a single hex character. 
 * 
 * @param c 
 * @return int 
 */
int parse_hex_char(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }

    return -1;
}

/**
 * @brief Parses a hex byte.
 * 
 * @param line      The string to parse.
 * @return uint8_t  The parsed byte.
 */
uint8_t parse_hex_byte(char *line)
{
    // Check if the input string is exactly two characters
    if (line == NULL || line[0] == '\0' || line[1] == '\0' || line[2] != '\0')
    {
        fprintf(stderr, "Invalid input: String must be 2 characters long.\n");
        exit(EXIT_FAILURE);
    }

    int high = parse_hex_char(line[0]);
    int low = parse_hex_char(line[1]);

    if (high == -1 || low == -1)
    {
        fprintf(stderr, "Invalid hex string: %s\n", line);
        exit(EXIT_FAILURE);
    }

    return (uint8_t)((high << 4) | low);
}

/**
 * @brief Parses the mac address.
 * 
 * @param line          The line to parse.
 * @param delimiters    The address's delimiters
 * @return Mac          A mac address data structure.
 */
Mac parse_address(char *line, const char *delimiters)
{
    Mac address = {0};

    char *next;
    char *token = strtok_r(line, delimiters, &next);
    for (size_t i = 0; i < sizeof(address.address); i++)
    {
        if (token == NULL)
        {
            break;
        }

        address.address[i] = parse_hex_byte(token);
        token = strtok_r(NULL, delimiters, &next);
    }

    return address;
}

/**
 * @brief Parsing a line in the packet file.
 *
 * @param line      A line containing the packet meta data.
 * @return Mac - A data structure for a mac's packet meta data.
 */
Packet parse_packet(char *line)
{
    Packet packet = {0};

    // Packet meta data are seperated by tabs.
    char *next;
    char *token = strtok_r(line, "\t\n", &next);
    if (token != NULL)
    {
        // Parse date time (unused)
        token = strtok_r(NULL, "\t\n", &next);
    }

    if (token != NULL)
    {
        // Parse transmitter address.
        packet.transmitter = parse_address(token, ":");
        token = strtok_r(NULL, "\t\n", &next);
    }

    if (token != NULL)
    {
        // Parse reciever address
        packet.receiver = parse_address(token, ":");
        token = strtok_r(NULL, "\t\n", &next);
    }

    if ((token != NULL))
    {
        // Parse packet data.
        packet.bytes = atoi(token);
    }

    return packet;
}

/**
 * @brief Parse line from the OUI file for vendor data.
 *
 * @param line      A line from the OUI file.
 * @return Vendor - A data structure containing a vendor's data.
 */
VendorParse parse_vendor(char *line)
{
    VendorParse parse = {0};

    // Vendor data are seperated by tabs.
    char *next;
    char *token = strtok_r(line, "\t\n", &next);
    if (token != NULL)
    {
        // Parse vendor address.
        Mac address = parse_address(token, ":-");
        parse.vendor.data = address.data;
        token = strtok_r(NULL, "\t\n", &next);
    }

    if (token != NULL)
    {
        // Parse vendor name.
        parse.name = token;
    }

    return parse;
}

/**
 * @brief Parse the OUI file for vendor data.
 *
 * @param request   A pointer to the request data.
 * @return Vendors -    A pointer to the oui data.
 */
Vendors parse_ouifile(Request *request)
{
    FILE *fp = fopen(request->OUIs_filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open file '%s'\n", request->OUIs_filename);
        exit(EXIT_FAILURE);
    }

    Vendors vendors = {0};
    vendors.map = (HASHMAP){.nodes = vendors.nodes, .capacity = MAX_NUM_OUIS};

    char line[BUFSIZ];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        const int ii = vendors.map.size;
        uint32_t *address = &vendors.entries[ii].data;
        char *name = vendors.names[ii];

        VendorParse parse = parse_vendor(line);
        *address = parse.vendor.data;
        strncpy(name, parse.name, MAX_LEN_VENDOR_NAME - 1);
        parse.name[MAX_LEN_VENDOR_NAME - 1] = '\0';

        const PAIR entry = {
            .first = (void *)address,
            .first_size = sizeof(*address),
            .second = (void *)name,
            .second_size = MAX_LEN_VENDOR_NAME};

        if (vendors.map.size >= MAX_NUM_MAC_ADDRESSES)
        {
            if (!hashmap_contains(&vendors.map, entry.first, entry.first_size))
            {
                fprintf(stderr, "Parsing too many unique vendor address.\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            hashmap_insert(&vendors.map, entry, NULL);
        }
    }

    fclose(fp);

    return vendors;
}

/**
 * @brief Parses the packet file for the macs' packet meta data.
 *
 * @param request   A pointer to the request data.
 * @return Packets - A pointer to the macs data.
 */
Macs parse_macs(const Request *request)
{
    FILE *fp = fopen(request->packets_filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open file '%s'\n", request->packets_filename);
        exit(EXIT_FAILURE);
    }

    Macs macs = {0};
    macs.map = (HASHMAP){.nodes = macs.nodes, .capacity = MAX_NUM_MAC_ADDRESSES};

    rewind(fp);

    char line[BUFSIZ];
    while (fgets(line, BUFSIZ, fp) != NULL)
    {
        Packet packet = parse_packet(line);

        if (is_broadcast(packet.receiver.data))
        {
            // Broadcast messages (unused)
            continue;
        }

        Mac mac = request->requested == TRANSMITTERS
                      ? packet.transmitter
                      : packet.receiver;

        if (request->ouifile_provided)
        {
            mac.data &= 0xffffff;
        }

        const int ii = macs.map.size;
        uint64_t *address = &macs.entries[ii].data;
        uint32_t *bytes = &macs.bytes[ii];

        *address = mac.data;
        *bytes = packet.bytes;

        PAIR entry = {
            .first = address,
            .first_size = sizeof(*address),
            .second = bytes,
            .second_size = sizeof(*bytes)};

        if (macs.map.size >= MAX_NUM_MAC_ADDRESSES)
        {
            if (!hashmap_contains(&macs.map, entry.first, entry.first_size))
            {
                fprintf(stderr, "Parsing too many unique mac address.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (!hashmap_insert(&macs.map, entry, &entry))
        {
            if (entry.second)
            {
                *((uint32_t *)entry.second) += packet.bytes;
            }
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
Report create_report(Macs *macs, Vendors *oui, Request *request)
{
    Report report = {0};

    if (request->ouifile_provided)
    {
        for (int i = 0; i < macs->map.size; ++i)
        {
            Mac *mac = &macs->entries[i];
            char *oui_name = (char *)hashmap_get(
                &oui->map,
                &mac->data,
                sizeof(mac->data),
                NULL);

            if (oui_name == NULL)
            {
                report.unknown_bytes += macs->bytes[i];
            }
            else
            {
                report.entries[report.length].data = macs->entries[i].data;
                report.names[report.length] = oui_name;
                report.bytes[report.length] = macs->bytes[i];
                report.length++;
            }
        }
    }
    else
    {
        for (int i = 0; i < macs->map.size; i++)
        {
            report.entries[report.length].data = macs->entries[i].data;
            report.bytes[report.length] = macs->bytes[i];
            report.length++;
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
    char *argv[] = {sort_path, "-t", separator, "-k", by_numeric, "-k", by_alphabet, terminator};

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
        for (int i = 0; i < report->length; i++)
        {
            const uint8_t *h = report->entries[i].address;
            const char *name = report->names[i];
            uint32_t bytes = report->bytes[i];

            printf("%02X:%02X:%02X\t%s\t%d\n", h[0], h[1], h[2], name, bytes);
        }

        const char *address = UNKNOWN_VENDOR_ADDRESS;
        const char *name = UNKNOWN_VENDOR_NAME;
        uint32_t bytes = report->unknown_bytes;
        printf("%s\t%s\t%d\n", address, name, bytes);
    }
    else
    {
        for (int i = 0; i < report->length; i++)
        {
            const uint8_t *h = report->entries[i].address;
            uint32_t bytes = report->bytes[i];
            printf(
                "%02X:%02X:%02X:%02X:%02X:%02X\t%d\n",
                h[0], h[1], h[2], h[3], h[4], h[5],
                bytes);
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
    clock_t start, end;
    double cpu_time_used;

    start = clock(); // Start the clock

    Request request = parse_request(argc, argv);

    Macs macs = parse_macs(&request);

    Vendors oui;
    if (request.ouifile_provided)
    {
        oui = parse_ouifile(&request);
    }

    Report report = create_report(&macs, &oui, &request);

    int pid;
    int fd[2]; // Array to hold pipe. 0 for read, 1 for write
    pipe(fd);  // Create pipe.

    switch (pid = fork()) // fork process
    {
    case -1:
        perror("Fail to create child (new) process.\n");
        exit(EXIT_FAILURE);
        break;
    case 0:                        // Child process
        close(fd[1]);              // Close pipe's write stream.
        dup2(fd[0], STDIN_FILENO); // Replace stdin stream with pipe's read stream.
        close(fd[0]);              // Close extra copy of pipe's read stream.
        sort_report(&request);     // Sort read stream until end-of-file received.
        exit(EXIT_FAILURE);        // Should only return on execv failure.
        break;
    default:          // Parent process
        close(fd[0]); // Close pipe's read stream.
        int fd_stdout = dup(STDOUT_FILENO);
        dup2(fd[1], STDOUT_FILENO);      // Replace stdout stream with pipe's write stream.
        close(fd[1]);                    // Close extra copy of pipe's write stream.
        print_report(&report, &request); // Print results to pipe's write stream.
        fflush(stdout);                  // Flush all results through.
        dup2(fd_stdout, STDOUT_FILENO);
        close(fd_stdout); // Close stdout (pipe's write stream) which sends end-of-file.
        wait(NULL);       // Wait for child to finish printing.
        break;
    }

    end = clock(); // End the clock

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC; // Calculate time in seconds
    printf("Time taken: %f seconds\n", cpu_time_used);

    return EXIT_SUCCESS;
}
