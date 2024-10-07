# A Makefile to build our 'wifistats' project

PROJECT =  	wifistats
HEADERS =  	$(wildcard *.h)
SRC     =  	$(wildcard *.c)
OBJ     =  	$(SRC:.c=.o)


COMPILE =  clang -std=c99 -g
CFLAGS  =  -Wall -pedantic -Werror -fcolor-diagnostics -fansi-escape-codes


$(PROJECT) : $(OBJ)
	$(COMPILE) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm


%.o : %.c $(HEADERS)
	$(COMPILE) $(CFLAGS) -c $<


clean:
	rm -f $(PROJECT) $(OBJ)
