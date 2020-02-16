# Makefile Histogram
# Andrew Smith
# 2/9/20

PROG := hist

OBJ = main.o histogram.o

CFLAGS += -Wall -std=c99 -m32
LFLAGS += -lm -lpthread -m32

all: CFLAGS += -O0
all: $(PROG)

debug: CFLAGS += -O0 -g
debug: $(PROG)

$(PROG): $(OBJ)
	 $(CC) $^ $(LFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROG)
