CC = gcc
CFLAGS = -g -w -o
TARGET = logParse
OBJS = logParse.o
.SUFFIXES: .c .o
all: logParse

%.o : %.c
	$(CC) -o %.o %.c

$(TARGET): logParse.c
	$(CC) $(CFLAGS) $(TARGET) logParse.c

.PHONY: clean
clean:
	rm -rf output.dat *.o $(TARGET)
