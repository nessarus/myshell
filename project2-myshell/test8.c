#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct _l {
    int       pid;
    struct _l   *next;
} CHILDPIDS;

CHILDPIDS   *cpids = NULL;

void push_pid( int newpid)
{
    CHILDPIDS *new = malloc( sizeof(CHILDPIDS) );

    if(new == NULL) {
        perror( __func__ );
        exit(EXIT_FAILURE);
    }

    new->pid    = newpid;
    new->next   = cpids;
    cpids       = new;
}

void pop_pid(int pid)
{
    if(cpids == NULL) {
        fprintf(stderr, "attempt to pop from an empty stack\n");
        exit(EXIT_FAILURE);
    }
    
    CHILDPIDS *old = cpids;
    if(old->pid == pid)
    {
        cpids = cpids->next;
        free(old);
        return;
    }

    CHILDPIDS *before_old = old;
    while(before_old->next->pid != pid) {
        before_old = before_old->next;
        if(before_old == NULL)
            return;
    }
    old = before_old->next;
    before_old->next = before_old->next->next;
    free(old);
}


int main(int argc, char*argv[])
{
    push_pid(1234);
    push_pid(1489);
    push_pid(1484);
    push_pid(6549);
    pop_pid(1234);
    pop_pid(1489);

    CHILDPIDS *c = cpids;
    while(c !=NULL) {
        printf("%d\n",c->pid);
        c = c->next;
    }   
    return 0;
}
