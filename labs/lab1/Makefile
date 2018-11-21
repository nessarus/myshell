# A Makefile to build our 'myshell' project

PROJECT =  rot
HEADERS =  $(PROJECT).h
OBJ     =  rot.o


C99     =  gcc -std=c99
CFLAGS  =  -Wall -pedantic -Werror

ifdef OS
   RM = del /Q /S
   EXE = $(PROJECT).exe
else
   ifeq ($(shell uname), Linux)
      RM = rm -f
	  EXE = $(PROJECT)
   endif
endif

ifneq ("$(wildcard $(HEADERS))","")
$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm

%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<
else
$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm

%.o : %.c
	$(C99) $(CFLAGS) -c $<
endif



clean: 
	$(RM) $(EXE) $(OBJ)