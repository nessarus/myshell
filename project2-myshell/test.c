#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{
    char *a;
    char *b;
    
    a="/Users/minruilu/cits2002/Project2/myshell";
    b=strrchr(a,'/');
    char c[strlen(a)-strlen(b)+1];
    strncpy(c,a,strlen(a)-strlen(b));
    c[strlen(a)-strlen(b)]='\0';
    //printf("something\n");
    //strcat(c,NULL);
    printf("a is %s\n",a);
    printf("b is %s\n",b);
    printf("c is %s\n",c);
    return 0;
    
}
