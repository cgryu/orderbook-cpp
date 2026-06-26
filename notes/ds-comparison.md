Setup: 
- Seed 42, 1M operations, 100K warmup, -O3 release, cancelProb = 0.3, marketProb = 0.1
- Core-pinned runs (taskset -c 2 ./build/bench)
- 5 runs per DS

Results:
- FlatBook: 9.8-10.3M ops/sec (throughput)
- MapBook: 7.9-9.0M ops/sec (throughput)
- 99th, 95th and 50th percentile tails were identical across both, around 300, 200 and 100ns respectively 
- Tail maxes can both be attributed to preemption
- Throughput differs, tail latency does not.

Conclusions:
- FlatBook appears to have a noticeable ~12-15% edge over MapBook. 
- The speed benefits of O(1) access from FlatBook should become even more prominent with at deeper book depths (untested). 