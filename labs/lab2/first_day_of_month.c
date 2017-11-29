#include <time.h>
#include <string.h>

//  written by Chris.McDonald@uwa.edu.au

//  PROVIDED WITHOUT MUCH EXPLANATION YET!

int first_day_of_month(int month, int year)
{
    struct tm   tm;

//  SET ALL FIELDS OF tm TO ZERO TO MAKE SOME FIELDS 'UNKNOWN'
    memset(&tm, 0, sizeof(tm));

//  INITIALIZE THE FILEDS THAT WE ALREADY KNOW
    tm.tm_mday  = 1;
    tm.tm_mon   = month-1;              // Jan=0, Feb=1, ....
    tm.tm_year  = year-1900;

//  ASK THE C99 FUNCTION mktime TO DETERMINE THE UNKNOWN FIELDS
//  See: http://pubs.opengroup.org/onlinepubs/9699919799/
    mktime(&tm);

//  RETURN THE INTEGER MONTH VALUE
    return tm.tm_wday;                  // Sun=0, Mon=1, .....
}
