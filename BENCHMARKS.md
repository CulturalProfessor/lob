# Benchmarks

Measurements of `Book::add` on synthetic workloads. Everything here is from
one machine, so only the comparisons between runs mean much.

## Running

    g++ -std=c++20 -O2 -Iinclude src/latency.cpp src/book.cpp bench/bench.cpp -o /tmp/bench
    taskset -c 2 /tmp/bench

`-O2` is required; a Debug build is several times slower and tells you nothing.
`taskset` pins the process to one core, which steadies the tail.

## Setup

- 10,000 orders, built in memory before timing starts
- one `ScopedTimer` around each `add` call, nothing else inside it
- harness floor is 15-17ns and is included in every figure below
- p50 and p99 repeat to within a few percent across runs; `max` does not, and
  swings between 3us and 30us depending on what the kernel is doing

## Experiment 1: the order index

`index_` is a `std::unordered_map<OrderId, OrderLocation>` written on every
resting order. Commenting out that single line:

| | p50 | p99 |
|---|---|---|
| with index | 60ns | 900ns |
| without | 45ns | 122ns |

So the line costs 15ns at the median and 780ns at p99, which is 87% of the
tail. Calling `reserve()` on the bucket array removed a reproducible 40us
spike, but left p99 where it was: `unordered_map` still allocates a node per
element, so 10,000 inserts are 10,000 calls to malloc.

## Experiment 2: price range and level churn

Alternating buys and sells over the same price range, so most orders cross.
A narrower range concentrates activity into fewer levels, which are then
created and destroyed repeatedly as they empty and refill.

| prices | trades | p50 | p99 | p99/p50 |
|---|---|---|---|---|
| 500 | 9,881 | 75ns | 162ns | 2.2x |
| 50 | 9,530 | 108ns | 1,040ns | 9.6x |

Trade counts are within 4% of each other, so both runs do the same amount of
matching. The difference is allocation churn from levels being destroyed and
recreated.

The 50-price run is the realistic one. Live books concentrate nearly all
their activity in a few levels around the best bid and ask.

## Where this points

Both results come down to allocation in the hot path, and `reserve` has gone
as far as it can. What is left is structural: a fixed array of levels indexed
by tick, so levels go empty and refill in place, and an order index that does
not allocate per entry.
