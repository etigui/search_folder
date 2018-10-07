#############################
# makefile searchfolder #
#############################

#http://randu.org/tutorials/c/make.php

#Source, executable, includes
INCL=main.h parser.h daemon.h files.h linkedlist.h function.h
SRC=main.c parser.c daemon.c files.c linkedlist.c function.c
OBJS=$(SRC:.c=.o)
EXE=searchfolder
FILES=daemon.dm

#Compiler, linker defines, library defines
CFLAGS=-Wall
LIBS=
CC=gcc
RM=rm -rf
DATE=$(shell date +'%d.%m.%Y at %H:%M:%S')

#All target
all : $(EXE)

#Compile and assemble C source files into object files
%.o: %.c
	@echo '//Building file: $<'
	$(CC) $(CFLAGS) -c $*.c
	@echo '//Finished building: $<'
	@echo ' '

# Link all object files with external libraries into binaries
$(EXE): $(OBJS)
	@echo '//Building target: $@'
	$(CC) -o $(EXE) $(OBJS) $(LIBS)
	@echo '//Finished building target: $@ the $(DATE)'

#Objects depend on these libraries
$(OBJS): $(INCL)

#Clean up objects, exectuables
clean:
	$(RM) $(OBJS) $(EXE) $(FILES)
	