CC      = gcc
FRAMES  = 128
CFLAGS  = -Wall -Wextra -std=c99 -pedantic -pthread -DNUM_FRAMES=$(FRAMES)
TARGET  = vm
OBJS    = main.o entrada.o memoria.o tlb.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c entrada.h memoria.h tlb.h
	$(CC) $(CFLAGS) -c main.c

entrada.o: entrada.c entrada.h
	$(CC) $(CFLAGS) -c entrada.c

memoria.o: memoria.c memoria.h entrada.h
	$(CC) $(CFLAGS) -c memoria.c

tlb.o: tlb.c tlb.h
	$(CC) $(CFLAGS) -c tlb.c

log:
	script -a evidencias.log -c "date; whoami; pwd; bash"

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean log