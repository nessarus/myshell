#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void function(void) //char fileName[])
{
    char fileName[] = "s.txt";
    char *newargs[] = {
        "sort",
        "-o", 
        fileName, 
        "-k", 
        "2", 
        "sample-packets.txt",
        NULL
    };
    execv("/usr/bin/sort", newargs);
}


/**
 * sort_devices sorts a text file separated by /t (tab). 
 * It uses a child process and calls "/urs/bin/sort". 
 * set source=destination to save.
 * set source and destination to save as.
 * set k to the column you want to sort.
 */
void sort_devices(char input_txt[], char output_txt[], 
        char k[]) {
    char *newargs[] = {
        "sort",
        "-o",
        output_txt,
        "-k",
        k,
        input_txt, 
        NULL
    };
    execv("/usr/bin/sort",newargs);
}

void forktest(void)
{
    int pid; // some system define a pid_t
    char *newargs[] = {
        "sort",
        "-k", 
        "2", 
        NULL
    }; 
    switch (pid = fork()) {
        case -1:
            perror("fork()"); // process creation failed
            exit(0);
            break;

        case 0:

            execv( "usr/bin/sort", newargs);
            exit(EXIT_FAILURE);
            break;

        default:                  // original parent process
            sleep(1);
            /*
            printf("p:  value of pid=%i\n", pid);
            printf("p:  parent's pid=%i\n", getpid());
            printf("p:  parent's parent pid=%i\n", getppid());
            */
            /*
                FILE   *dict;
                char   line[BUFSIZ];

                dict = fopen("sample-packets.txt", "r");
                if(dict == NULL) {
                    printf( "cannot open dictionary '%s'\n", "sample-packets.txt");
                    exit(EXIT_FAILURE);
                }

            //  READ EACH LINE FROM THE FILE,
            //  CHECKING FOR END-OF-FILE OR AN ERROR
                while( fgets(line, sizeof line, dict) != NULL )    
                {
                    printf("%s",line);
                }
            //  AT END-OF-FILE (OR AN ERROR), CLOSE THE FILE
                fclose(dict);
            */
            break;
    }
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    //char input[] = "sample-packets.txt";
    //char output[] = "s.txt";
    //sort_devices(input, output, "2");
    function(); //argv[1]);
    //forktest();
    return 0;
}

