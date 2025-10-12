/**
 * @file    background.c
 * @author  Joshua Ng
 * @brief   Executes shell commands asynchronously.
 * @date    2023-09-08
 */

#include "background.h"
#include "globals.h"
#include "hashset.h"
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN_CAPACITY 16

static size_t hash_pid(const void *pid);
static bool pids_equals(const void *pid1, const void *pid2);
static void resize_pids_capacity(size_t capacity);
static void add_pid(pid_t pid);
static void remove_pid(pid_t pid);
static void parent_on_child_terminate(int signum);
static void child_terminate(int signum);

static HASHSET pids = {.interface = {.hash = hash_pid, .equals = pids_equals}};

/**
 * @brief Handler for when parent recieves a child has terminated signal.
 * @param signum The terminate signal enum.
 */
static void parent_on_child_terminate(int signum)
{
    pid_t pid = 0;
    int status;

    // Loop through all child processes that have changed their state.
    // WNOHANG: Return immediately if no child has changed its state.
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (!hashset_contains(&pids, &pid))
        {
            continue;
        }

        if (WIFEXITED(status))
        {
            uint8_t exitstatus = WEXITSTATUS(status);
            printf("Child %d exited with status %d\n", pid, exitstatus);
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child %d killed by signal %d\n", pid, WTERMSIG(status));
        }
        else
        {
            continue;
        }

        remove_pid(pid);

        if (pids.size == 0)
        {
            signal(SIGCHLD, SIG_DFL);
            break;
        }
    }
}

/**
 * @brief Handles child's response to receiving a terminate signal.
 * @param signum    The terminate signal enum.
 */
static void child_terminate(int signum)
{
    exit(EXIT_SUCCESS);
}

/**
 * @brief Executes a background shell command.
 * 
 * @param t     The shell command.
 * @return The exit status.
 */
int background_shellcmd(SHELLCMD *t)
{
    signal(SIGCHLD, parent_on_child_terminate);
    pid_t fpid = fork();
    check_error(fpid);

    if (fpid == 0)                  // Child process
    {
        signal(SIGCHLD, SIG_DFL);
        signal(SIGTERM, child_terminate);
        int exitstatus = EXIT_SUCCESS;

        if (t != NULL)
        {
            exitstatus = execute_shellcmd(t);
        }

        exit(exitstatus);
    }

    add_pid(fpid);
    return EXIT_SUCCESS;
}

/**
 * @brief Handles terminating all background processes.
 */
void background_exit(void)
{
    ITERATOR it = hashset_iterator(&pids);

    while (it.has_next(&it))
    {
        pid_t *pid = (pid_t *) it.next(&it);
        kill(*pid, SIGTERM);
        free(pid);
    }

    pids.size = 0;
    pids.capacity = 0;
    free(pids.elements);
    pids.elements = NULL;
}

/**
 * @brief Compute the hash of a pid
 * @param pid The pid to be hashed.
 * @return The hash of the pid.
 */
static size_t hash_pid(const void *pid)
{
    return (size_t) *(pid_t *)pid;
}

/**
 * @brief Check if two pids are equal.
 * @param pid1 The first pid.
 * @param pid2 The second pid.
 * @return True if the pids equal each other.
 */
static bool pids_equals(const void *pid1, const void *pid2)
{
    return *(pid_t *)pid1 == *(pid_t *)pid2;
}

/**
 * @brief Resize the pids set capacity.
 * @param capacity The new capacity.
 */
static void resize_pids_capacity(size_t capacity)
{
    void *elements = calloc(capacity, sizeof(void *));
    check_allocation(elements);
    if (pids.capacity == 0)
    {
        pids.elements = elements;
        pids.capacity = capacity;
        return;
    }

    void *old = hashset_resize(&pids, elements, capacity);
    if (old == NULL)
    {
        fprintf(stderr, "%s: unable to resize pids capacity\n", name0);
        exit(EXIT_FAILURE);
    }
    free(old);
}

/**
 * @brief Adds a pid to the hashset.
 * @param pid The pid to add.
 */
static void add_pid(pid_t pid)
{
    if (pids.capacity == 0)
    {
        resize_pids_capacity(MIN_CAPACITY);
    }
    else if (pids.capacity == pids.size)
    {
        resize_pids_capacity(pids.capacity * 2);
    }
    
    pid_t *new_pid = malloc(sizeof(pid_t));
    *new_pid = pid;
    HASHSET_RESULT result = hashset_insert(&pids, new_pid);
    
    if (result.success)
    {
        if (pids.size > pids.capacity / 2)
        {
            resize_pids_capacity(pids.capacity * 2);
        }
    }
    else
    {
        free(new_pid);
    }
}

/**
 * @brief Removes a pid from the hashset.
 * 
 * @param pid The pid to remove.
 */
static void remove_pid(pid_t pid)
{
    HASHSET_RESULT result = hashset_remove(&pids, &pid);
    if (!result.success)
        return;
    
    free(result.element);
    
    if (pids.size == 0)
    {
        free(pids.elements);
        pids.elements = NULL;
        pids.capacity = 0;
        return;
    }
    
    if ((pids.size < pids.capacity / 8) && (pids.capacity > MIN_CAPACITY))
    {
        size_t capacity = pids.capacity / 2;
        if (capacity < MIN_CAPACITY)
            capacity = MIN_CAPACITY;

        resize_pids_capacity(capacity);
    }
}



