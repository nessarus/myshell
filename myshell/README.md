# myshell

myshell is a basic implementation of a unix shell using the C99 programming language.

## Instructions
Use a macOS or Linux terminal  
* Execute external commands (e.g. /usr/bin/cal -y)
![Alt Text](https://media.giphy.com/media/WgO9PwZ1qvt1CoV6FW/giphy.gif)

* Search path (users do not need to provide full address)
e.g. prompt>> cal -y
![Alt Text](https://media.giphy.com/media/t8LoBPg52gGxBhG6C4/giphy.gif)

* Execute internal commands: exit, cd, time
* Sequential execution (e.g. ";", "&&", "||")
e.g. ls; cal -y || asdfasd
![Alt Text](https://media.giphy.com/media/23ex1hrWYOef7MNU1r/giphy.gif)

* Sub-shell execution (e.g. >> (commands) )
e.g. prompt>> (exit)
* Stdin and stdout file (e.g. command < infile, command > outfile, command >> outfile (appends))
* Pipelines (e.g. command1 | commmand2)
* Shell scripts
* Background execution (e.g. "command1 & command2")

## How to run
To build run the Makefile file in the terminal:  
\>> make

![Alt Text](https://media.giphy.com/media/WgO9PwZ1qvt1CoV6FW/giphy.gif)

To run the program:  
\>> ./myshell

![Alt Text](https://media.giphy.com/media/48NvlVthTh2nhPC2uj/giphy.gif)

## CITS2002 System Programming
myShell is a student project from the UWA course CITS2002 System Programming. Skeleton C99 source code files were provided by the University as assistance to develop this program. 
