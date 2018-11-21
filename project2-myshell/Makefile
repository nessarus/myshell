# A Makefile to build our 'myshell' project

PROJECT =  myshell
HEADERS =  $(PROJECT).h
OBJ     =  myshell.o globals.o execute.o parser.o


C99     =  cc -std=c99
CFLAGS  =  -Wall -pedantic -Werror 


$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm


%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<


clean:
	rm -f $(PROJECT) $(OBJ)
