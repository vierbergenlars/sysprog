UTIL_SRC=$(wildcard src/util/*.c)
SERVER_SRC=$(wildcard src/server/*.c) 
LOGGER_SRC=$(wildcard src/logger/*.c) 
CFLAGS+=-Wall -std=gnu99 -pthread -g -DSET_MIN_TEMP=17 -DSET_MAX_TEMP=24
CDFLAGS=$(CFLAGS) -fPIC
LFLAGS+=-g -pthread -L./lib -Wl,-rpath=./lib 
LDFLAGS=-shared -pthread

all: server logger
logger: $(patsubst %.c,%.o,$(LOGGER_SRC)) lib/liblogger.so
	$(CC) $(LFLAGS) $+ -o $@ -llogger
server: $(patsubst %.c,%.o,$(SERVER_SRC)) lib/liblogger.so lib/liblist.so lib/libshared_queue.so lib/libtcp_socket.so
	$(CC) $(LFLAGS) $(shell mysql_config --libs) $+ -o $@ -llogger -llist -lshared_queue -ltcp_socket
src/server/sensor_db.o: src/server/sensor_db.c
	$(CC) $(CFLAGS) $(shell mysql_config --cflags) -c $< -o $@
src/server/storage_manager.o: src/server/storage_manager.c
	$(CC) $(CFLAGS) $(shell mysql_config --cflags) -c $< -o $@
src/util/%.o: src/util/%.c
	$(CC) $(CDFLAGS) -c $< -o $@
lib/libtcp_socket.so: src/util/tcp_socket.o src/util/_tcpsocket.o
	$(CC) $(LDFLAGS) $+ -o $@
lib/libshared_queue.so: src/util/queue.o src/util/shared_queue.o 
	$(CC) $(LDFLAGS) $+ -o $@
lib/lib%.so: src/util/%.o
	$(CC) $(LDFLAGS) $+ -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f src/*/*.o lib/*.so
	rm -f server
	rm -f logger
.PHONY: clean all
