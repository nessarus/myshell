#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// testing reading from file


void function(char in_filename[], char out_filename[])
{
    FILE        *fp_in  = fopen(in_filename,  "r");
    FILE        *fp_out = fopen(out_filename, "w");

//  ENSURE THAT OPENING BOTH FILES HAS BEEN SUCCESSFUL
    if(fp_in != NULL && fp_out != NULL)
    {
        char    line[BUFSIZ];

        while( fgets(line, sizeof line, fp_in) != NULL)
        {  
            if(fputs(line, fp_out) == EOF)
            {
                printf("error copying file\n");
                exit(EXIT_FAILURE);
            }
        }

    }
//  ENSURE THAT WE ONLY CLOSE FILES THAT ARE OPEN
    if(fp_in != NULL)
        fclose(fp_in);
    if(fp_out != NULL)
        fclose(fp_out);
}

int main(int argc, char *argv[])
{
    function(argv[1],”s.txt”);
    return 0;
}
