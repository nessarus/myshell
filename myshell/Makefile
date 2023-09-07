# A Makefile to build our 'myshell' project

PROJECT =  	myshell
HEADERS =  	$(PROJECT).h
OBJ     =  	myshell.o globals.o external.o parser.o filepaths.o searchpath.o \
			simplemap.o internal.o subshell.o redirection.o pipeline.o shellscript.o \
			set.o background.o


COMPILE =  clang -std=c99 -g
CFLAGS  =  -Wall -pedantic -Werror -fcolor-diagnostics -fansi-escape-codes


$(PROJECT) : $(OBJ)
	$(COMPILE) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm


%.o : %.c $(HEADERS)
	$(COMPILE) $(CFLAGS) -c $<


clean:
	rm -f $(PROJECT) $(OBJ)
