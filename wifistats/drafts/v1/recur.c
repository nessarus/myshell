#include <stdio.h>

void function(int i, int j, int *k)
{
    char a = i;
    a = j *  a;
    if(k==NULL) {
        printf("woah\n");
    }
}

int main(int argc, char *argv[])
{
    function(1 , 2, NULL);
    return 0;
}
