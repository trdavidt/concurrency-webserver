# Concurrent Web Server

## Overview

This project implements a multi-threaded concurrent web server based on the single-threaded server from Arpaci-Dusseau's repo for [*Operating Systems: Three Easy Pieces*](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master). Incoming HTTP requests for HTML files are handled by a worker thread pool. The scheduler manages the worker thread pool with either a first in first out (FIFO) or shortest file first (SFF) policy. The worker threads are the consumers in a producer-consumer relationship with the main thread, waiting for the main thread to add new requests to the scheduler. The client creates a thread to request the server for each of one or multiple files.

## Server

```
wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]
```

The server serves files from the `basedir` directory and runs on the specified `port` on `localhost`. The server creates `threads` number of worker threads and will accept up to `buffers` number of request connections at once. The scheduling policy provided in `schedalg` can either be `SFF` or `FIFO`. Defaults are provided in the [common header](https://github.com/trdavidt/concurrency-webserver/blob/master/src/common.h).

## Client

```
wclient <host> <port> <file path(s)>
```

The client makes requests for the listed files to the provided `host` and `port`. The file paths should be relative to the `basedir` used by the server.


## Load Testing

to be continued...

