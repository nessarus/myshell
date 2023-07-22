#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE        *fp_in  = fopen(argv[1],  "r");
    FILE        *fp_out = fopen(argv[2], "w");

//  ENSURE THAT OPENING BOTH FILES HAS BEEN SUCCESSFUL
    if(fp_in != NULL && fp_out != NULL)
    {
        char    line[BUFSIZ];

        while( fgets(line, sizeof line, fp_in) != NULL)
        {  
            if(fputs(line, fp_out) == EOF)
            {
                printf("error copying file\n");
                return 0;
            }
        }

    }
//  ENSURE THAT WE ONLY CLOSE FILES THAT ARE OPEN
    if(fp_in != NULL)
        fclose(fp_in);
    if(fp_out != NULL)
        fclose(fp_out);

    return 0;
}
