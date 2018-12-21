# myshell

myshell is a basic implementation of a unix shell using the C99 programming language.

## Instructions
Use a macOS or Linux terminal  
* Execute external commands (e.g. /usr/bin/cal -y)
* Search path (users do not need to provide full address)
e.g. prompt>> cal -y
* Execute internal commands: exit, cd, time
* Sequential execution (e.g. ";", "&&", "||")
e.g. ls; cal -y || asdfasd
* Sub-shell execution (e.g. >> (commands) )
e.g. prompt>> (exit)
* Stdin and stdout file (e.g. command < infile, command > outfile, command >> outfile (appends))
* Pipelines (e.g. command1 | commmand2)
* Shell scripts
* Background execution (e.g. "command1 & command2")

## How to run
To build run the Makefile file in the terminal:  
\>> make


To run the program:  
\>> ./myshell

## CITS2002 System Programming
myShell is a student project from the UWA course CITS2002 System Programming. Skeleton C99 source code files were provided by the University as assistance to develop this program. 
