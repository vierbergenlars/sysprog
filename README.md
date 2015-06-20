# Sensor monitoring system

## Configuration

* Credentials for the mysql database must be changed in `src/server/config.h`
* Minimum and maximum temperature configuration can be changed in `Makefile`

## Compiling

* Run `make` to build in the standard configuration
* To build in debug-mode: run `CFLAGS=-DDEBUG make`
* To build with gprof output: run `CFLAGS="-g -gp -fno-omit-frame-pointer" LFLAGS="-g -gp" make`

To build the gprof preload helper for profiling multithreaded applications, run `make gprof-helper.so`

## Running

There are three distinct executables: `server`, `logger` and `sensor_node`

The server and logger both have to be started at the same time, else the server will hang because there is no
reader on the `logFifo`

The `server` binary accepts one optional argument, the tcp port to listen on.
If no value is given, it will listen on `1234` by default.

The `logger` binary accepts no arguments, and must be started in the same working directory as the server.

The `sensor_node` binary requires 4 arguments: sensor id, time between measurements, server ip and server port.
The source code for this binary is downloaded from toledo and only serves as a testcase for the monitoring system.

## Profiling

To profile the `server`-binary effectively with `gprof`, add `gprof-helper.so` to the `LD_PRELOAD` environment variable.

```
LD_PRELOAD=./gprof-helper.so ./server
```

After running the application for some time, run `gprof server | less` to view profiling data.

For a nice call graph, use [`gprof2dot`](https://github.com/jrfonseca/gprof2dot): `gprof server | gprof2dot -n0 -e0 | dot -Tpng -o callgraph.png`

## Valgrind

Both the server and the logger have been checked for memory leaks with valgrind.

### Server

```
lars@lotte:~$ valgrind --leak-check=full --show-reachable=yes ./server
==2191== Memcheck, a memory error detector
==2191== Copyright (C) 2002-2011, and GNU GPL'd, by Julian Seward et al.
==2191== Using Valgrind-3.7.0 and LibVEX; rerun with -h for copyright info
==2191== Command: ./server
==2191==
^CShutting down operations
==2191==
==2191== HEAP SUMMARY:
==2191==     in use at exit: 56 bytes in 1 blocks
==2191==   total heap usage: 2,672 allocs, 2,671 frees, 305,463 bytes allocated
==2191==
==2191== 56 bytes in 1 blocks are still reachable in loss record 1 of 1
==2191==    at 0x4C28BED: malloc (vg_replace_malloc.c:263)
==2191==    by 0x400C4CC: _dl_map_object_deps (dl-deps.c:506)
==2191==    by 0x4012167: dl_open_worker (dl-open.c:263)
==2191==    by 0x400DBD5: _dl_catch_error (dl-error.c:178)
==2191==    by 0x4011B49: _dl_open (dl-open.c:633)
==2191==    by 0x5F4BEAF: do_dlopen (dl-libc.c:86)
==2191==    by 0x400DBD5: _dl_catch_error (dl-error.c:178)
==2191==    by 0x5F4BF4E: dlerror_run (dl-libc.c:47)
==2191==    by 0x5F4C046: __libc_dlopen_mode (dl-libc.c:160)
==2191==    by 0x538779B: pthread_cancel_init (unwind-forcedunwind.c:53)
==2191==    by 0x53878FB: _Unwind_ForcedUnwind (unwind-forcedunwind.c:126)
==2191==    by 0x53859EF: __pthread_unwind (unwind.c:130)
==2191==
==2191== LEAK SUMMARY:
==2191==    definitely lost: 0 bytes in 0 blocks
==2191==    indirectly lost: 0 bytes in 0 blocks
==2191==      possibly lost: 0 bytes in 0 blocks
==2191==    still reachable: 56 bytes in 1 blocks
==2191==         suppressed: 0 bytes in 0 blocks
==2191==
==2191== For counts of detected and suppressed errors, rerun with: -v
==2191== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 6 from 6)
lars@lotte:~$
```

The still reachable block of memory is caused by the pthread library.

### Logger

```
==11110== Memcheck, a memory error detector
==11110== Copyright (C) 2002-2011, and GNU GPL'd, by Julian Seward et al.
==11110== Using Valgrind-3.7.0 and LibVEX; rerun with -h for copyright info
==11110== Command: ./logger
==11110==
==11110==
==11110== HEAP SUMMARY:
==11110==     in use at exit: 0 bytes in 0 blocks
==11110==   total heap usage: 2 allocs, 2 frees, 1,136 bytes allocated
==11110==
==11110== All heap blocks were freed -- no leaks are possible
==11110==
==11110== For counts of detected and suppressed errors, rerun with: -v
==11110== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 4 from 4)
```
