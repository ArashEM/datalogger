CC = gcc
RM = rm -rf
SIZE = size
#-------------------------------------------#
CFLAGS  = -c -Wall -g -O2 -ansi
CFLAGS += -I.
LDFLAGS = 
#-------------------------------------------#
SRC = main.c log.c list.c
OBJ = main.o log.o list.o
EXE = datalogger
#-------------------------------------------#
all:	$(EXE)	$(OBJ)

$(EXE):	$(OBJ)
	$(CC)	$(LDFLAGS) $(OBJ) -o $@

%.o:%.c
	$(CC)	$(CFLAGS)  $<

clean:
	$(RM)	$(EXE)	$(OBJ)
