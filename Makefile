UTIL_SRC=$(wildcard src/util/*.c)
SERVER_SRC=$(wildcard src/server/*.c) $(UTIL_SRC)
LOGGER_SRC=$(wildcard src/logger/*.c) $(UTIL_SRC)
CFLAGS+=-Wall -std=gnu99 -pthread -g
LFLAGS+=-g -pthread

all: server logger
logger: $(patsubst %.c,%.o,$(LOGGER_SRC))
	$(CC) $(LFLAGS) $+ -o $@
server: $(patsubst %.c,%.o,$(SERVER_SRC))
	$(CC) $(LFLAGS) $+ -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f src/*/*.o
	rm -f server
	rm -f logger
.PHONY: clean all
