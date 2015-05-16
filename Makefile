UTIL_SRC=$(wildcard src/util/*.c)
SERVER_SRC=$(wildcard src/server/*.c) $(UTIL_SRC)
LOGGER_SRC=$(wildcard src/logger/*.c) $(UTIL_SRC)
CFLAGS+=-Wall -std=c99 -g
LFLAGS+=-g

all: server logger
logger: $(patsubst %.c,%.o,$(LOGGER_SRC))
	$(CC) $(LFLAGS) $+ -o $@
server: $(patsubst %.c,%.o,$(SERVER_SRC))
	$(CC) $(LFLAGS) $+ -o $@ -pthread
src/server/connection_manager.c: src/util/queue.c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f src/{util,server,logger}/*.o
	rm -f server
	rm -f logger
.PHONY: clean all
