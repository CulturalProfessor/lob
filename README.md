# lob

A limit order book and matching engine in C++20, with a latency harness.

Orders rest at price-time priority: better price first, and within a price,
earliest arrival first. An incoming order that crosses matches against the
opposite side, best price first and FIFO within each level, consuming resting
orders until it is filled or nothing crosses any more. Whatever quantity is
left rests in the book. Every fill produces a trade record with both order ids,
the price, the quantity and the aggressor side.

Trades execute at the resting order's price, not the incoming order's limit.

## Status

Done: add, cancel, matching on both sides, trade reporting, file replay, and a
latency harness with percentiles.

Not done: a real market data feed, and the allocation-free rewrite that the
benchmarks argue for.

## Layout

    include/lob/        Order, Trade, Book
    include/profiler/   Profile (samples and percentiles), ScopedTimer
    src/                implementations, and the replay driver in main.cpp
    bench/              benchmarks, compiled by hand, not part of the CMake build
    data/               replay files; each header states its expected output

## Build and run

    cmake -S . -B build
    cmake --build build -j$(nproc)
    ./build/lob data/simple.txt

Output is the trades produced, then the final ladder. Asks print above the
separator and bids below, both descending, so the best of each sit either side
of the line.

## Replay files

    simple.txt      8 orders, no crossing. Small enough to check by eye.
    busy.txt        240 orders, no crossing. Volume.
    cancels.txt     cancel cases: empty a level, unknown id, double cancel.
    cross_buy.txt   buy-side matching, with start and end ladders in the header.
    cross_sell.txt  sell-side matching.

Message format:

    A,<id>,<side B|S>,<price>,<qty>
    C,<id>

Prices are integer ticks. Lines starting with `#` are comments.

## Sanitizers

Run these after any change to `add` or `cancel`. The code holds references and
iterators into containers it also erases from, which is the bug class
AddressSanitizer catches.

    cmake -S . -B build-asan -DLOB_SANITIZE=ON
    cmake --build build-asan -j$(nproc)
    ./build-asan/lob data/cross_buy.txt

## Release

    cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
    cmake --build build-release -j$(nproc)

## Benchmarks

`add` on a crossing workload of 10,000 orders:

    price levels    p50      p99
    500             75ns     162ns
    50              108ns    1,040ns

The 50-level case is closer to a real book. The gap between the rows is
allocation churn from price levels being created and destroyed as they empty
and refill.

Measurement floor is 15-17ns and is included in both rows.

Method and caveats: [BENCHMARKS.md](BENCHMARKS.md).
