UTIL_SRC=$(wildcard src/util/*.c)
SERVER_SRC=$(wildcard src/server/*.c) $(UTIL_SRC)
LOGGER_SRC=$(wildcard src/logger/*.c) $(UTIL_SRC)
CFLAGS+=-Wall -std=gnu99 -pthread -DSET_MIN_TEMP=17 -DSET_MAX_TEMP=24 
LFLAGS+=-pthread 

all: server logger sensor_node
logger: $(patsubst %.c,%.o,$(LOGGER_SRC))
	$(CC) $(LFLAGS) $+ -o $@
server: $(patsubst %.c,%.o,$(SERVER_SRC))
	$(CC) $(LFLAGS) $(shell mysql_config --libs) $+ -o $@
sensor_node: src/sensornode/sensor_node.o src/util/_tcpsocket.o
	$(CC) $(LFLAGS) $+ -o $@
src/server/sensor_db.o: src/server/sensor_db.c
	$(CC) $(CFLAGS) $(shell mysql_config --cflags) -c $< -o $@
src/server/storage_manager.o: src/server/storage_manager.c
	$(CC) $(CFLAGS) $(shell mysql_config --cflags) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
gprof-helper.so: gprof-helper.c
	gcc -shared -fPIC $< -o $@ -lpthread -ldl
clean:
	rm -f src/*/*.o
	rm -f server
	rm -f logger
.PHONY: clean all
