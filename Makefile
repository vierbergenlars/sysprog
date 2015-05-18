UTIL_SRC=$(wildcard src/util/*.c)
SERVER_SRC=$(wildcard src/server/*.c) $(UTIL_SRC)
LOGGER_SRC=$(wildcard src/logger/*.c) $(UTIL_SRC)
CFLAGS+=-Wall -std=gnu99 -pthread -g -DSET_MIN_TEMP=17 -DSET_MAX_TEMP=24
LFLAGS+=-g -pthread

all: server logger
logger: $(patsubst %.c,%.o,$(LOGGER_SRC))
	$(CC) $(LFLAGS) $+ -o $@
server: $(patsubst %.c,%.o,$(SERVER_SRC))
	$(CC) $(LFLAGS) $(shell mysql_config --libs) $+ -o $@
src/server/sensor_db.o: src/server/sensor_db.c
	$(CC) $(CFLAGS) $(shell mysql_config --cflags) -c $< -o $@
src/server/storage_manager.o: src/server/storage_manager.c
	$(CC) $(CFLAGS) $(shell mysql_config --cflags) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f src/*/*.o
	rm -f server
	rm -f logger
.PHONY: clean all
