# myshell

myshell is a basic implementation of a unix shell using the C99 programming language.  
![Alt Text](https://media.giphy.com/media/5QXkxncWSfkOvhwc4F/giphy.gif)  

## Features
* macOS and Linux supported 
* Execute external commands (e.g. /usr/bin/cal -y)  
* Search path (users do not need to provide full address)
* Execute internal commands: exit, cd, time
* Sequential execution (e.g. ";", "&&", "||")  
* Sub-shell execution (e.g. >> (commands) )
* Stdin and stdout file (e.g. command < infile, command > outfile, command >> outfile (appends))
* Pipelines (e.g. command1 | commmand2)
* Shell scripts
* Background execution (e.g. "command1 & command2")

## How to run
To build run the Makefile file in the terminal:  
```
\>> make
```  
  
To run the program:  
```
\>> ./myshell
```  

## CITS2002 System Programming
myShell is a student project from the University of Western Australia course CITS2002 System Programming. Skeleton C99 source code files were provided by the University as assistance to develop this program. 

## Preparation and Labs
Preparation work from C99 techniques labs have also been included.
