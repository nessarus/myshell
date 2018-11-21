#include <stdio.h>
#include <stdlib.h>

#include "first_day_of_month.c"

//  written by Chris.McDonald@uwa.edu.au
//
//  compile:  cc -std=c99 -Wall -Werror -o mycal mycal.c
//  run:      ./mycal [month year]

#define NROWS           6
#define NCOLS           7

int main(int argc, char *argv[])
{
//  DEFINE IMPORTANT VARIABLES (INITIALLY PRODUCING ONLY ONE CALENDAR)
    int year            = 2017;
    int month           = 8;            // August (Jan=1)
    int day1            = 2;            // August 1st is a Tuesday (Sun=0)

    int days_in_month;

//  IF EXACTLY 3 COMMAND-LINE ARGUMENTS ARE PROVIDED, USE THEM FOR THE
//  NEW VALUES OF month AND year  (WE SHOULD VALIDATE OUR INPUT!)
    if(argc == 3) {
        month           = atoi(argv[1]);
        year            = atoi(argv[2]);
    }

//  WE NEED A CONDITIONAL TEST, AND printf STATEMENT, FOR EACH MONTH
//  WE COULD USE A SEQUENCE OF  if..else..if..else..  BUT PREFER switch
//  NOTE THAT NOT ALL MONTHS ARE HERE!
    switch (month) {
    case 4:
        printf("         April %i\n", year);
        days_in_month = 30;
        break;
    case 8:
        printf("      August %i\n", year);
        days_in_month = 31;
        break;
    }

//  PRINT THE DAYS OF THE WEEK - EASY!
    printf("Su Mo Tu We Th Fr Sa\n");

    day1 = first_day_of_month(month, year);
    int day = 1 - day1;

//  WE OBSERVE THAT EACH CALENDAR (CAN) HAVE 6 ROWS OF 7 COLUMNS
    for(int row=0 ; row<NROWS ; row = row+1)        // row will be 0,1,2,3,4,5
    {
        for(int col=0 ; col<NCOLS ; col = col+1)    // col will be 0,1,2,3,4,5,6
        {
//  IF 'OUTSIDE' THE MONTH, JUST PRINT SPACES
            if(day < 1 || day > days_in_month)
            {
                printf("   ");
            }
//  IF 'WITHIN' THE MONTH, PRINT EACH DAY NUMBER
            else
            {
                printf("%2i ", day);    // print integers using 2 columns
            }
            day = day+1;                // increase the day after each cell
        }
//  PRINT NEW-LINE AT THE END OF EACH WEEK (after all 7 columns)
        printf("\n");
    }

//  TERMINATE, INDICATING SUCCESS TO THE OPERATING SYSTEM 
    exit(EXIT_SUCCESS);
}
