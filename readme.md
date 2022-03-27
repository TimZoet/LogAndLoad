# Log And Load - Low Latency Logging

For building, exporting and installing please refer to the [build instructions](build_instructions.md). Everything else
can be found in the [manual](https://github.com/TimZoet/LogAndLoad-manual).

The Log And Load (`LAL`) library is a small C++ module for low latency logging. By combining asynchronous buffered
IO with postponed and compile-time computation, multi-threaded applications can efficiently generate lossless binary
logs. These logs can be converted to a human-readable format or analyzed through automated methods.

Consider the log file below that was generated by some hypothetical application:

```cpp
    INFO 2022/02/22 17:15:00.1131 Initializing
    INFO 2022/02/22 17:15:01.7594 Waiting for data
    INFO 2022/02/22 17:15:02.7544 Waiting for data
    INFO 2022/02/22 17:15:03.7490 Waiting for data
    INFO 2022/02/22 17:15:04.7426 Waiting for data
    INFO 2022/02/22 17:15:05.3478 Received 109.15MiB of data
    INFO 2022/02/22 17:15:08.4410 Starting data processing
    INFO 2022/02/22 17:15:08.5611 0/100%
    INFO 2022/02/22 17:15:12.1989 22/100%
    WARN 2022/02/22 17:15:13.9254 Degenerate data detected, discarding 124 elements
    INFO 2022/02/22 17:15:15.9725 55/100%
    INFO 2022/02/22 17:15:19.6207 78/100%
    INFO 2022/02/22 17:15:21.0613 100/100%
    INFO 2022/02/22 17:15:21.1001 Processing done
    INFO 2022/02/22 17:15:21.3199 Statistics:
    INFO 2022/02/22 17:15:21.3218 Time elapsed: 12.6591s
    INFO 2022/02/22 17:15:21.3218 Mean: 15.1431
    INFO 2022/02/22 17:15:21.3243 Median: 12.7834
    INFO 2022/02/22 17:15:21.3276 Min: 5.2796
    INFO 2022/02/22 17:15:21.3310 Max: 38.3742
```

These messages take up close to a kilobyte. The actual amount of information is much lower. This has two main causes.
Firstly, we convert binary data to a textual representation. Date and time become several times larger than necessary,
just like e.g. the floating point values. Secondly, there is a large amount of repeated information. The message
categories and most of the message strings are probably static. Referring to them by an identifier would be much more
efficient.

What's more, by converting binary data to text we are losing information. This has the potential to hide causes of bugs.
It also takes a lot of time to perform this conversion and the associated string formatting. We could postpone these
computations until after running the application. Besides, an automated analysis tool would probably need binary data
anyway. 

When logging a small message, doing so directly to a file on disk has huge performance penalties, in terms of both
latency and throughput. Disks are slow to respond and need larger amounts of data to be able to write at their
theoretical maximum writing speed. While some disk IO is unavoidable (unless your logs fit in main memory) these writes
should be done in an optimal manner and not interrupt your most performance-critical code paths.

The Log And Load library solves all these problems and subsequently offers the following:

* **Low latency**: Logging a single integer can be done in less than 10ns in the performance-critial code path.
* **High throughput**: There is little runtime computational overhead, making logging mostly disk bound (assuming you
  even generate that much logging data).
* **Thread-safe**: Log from an arbitrary number of threads while maintaining the exact order of all messages.
* **Macro free**: Need I say more?

One important thing to note, perhaps, is that this library is not intended to be used in safety critical applications.
This is mainly because messages can be lost if they are not flushed to disk yet when the application crashes.
