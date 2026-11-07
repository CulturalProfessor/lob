#include <algorithm>
#include <cmath>
#include <iostream>
#include <profiler/latency.hpp>

namespace profiler {
void Profile::record(std::uint64_t ns) { storage_.push_back(ns); }

void Profile::report() {
  if (storage_.empty()) {
    std::cout << "No samples present, please initialize it.\n";
    return;
  }

  std::sort(storage_.begin(), storage_.end());
  std::uint64_t n = storage_.size();
  std::uint64_t min = storage_[0];
  std::uint64_t max = storage_[n - 1];

  std::uint64_t p99 = storage_[std::ceil(0.99 * n) - 1];
  std::uint64_t p50 = storage_[std::ceil(0.50 * n) - 1];

  std::cout << "Count" << '\t' << n << '\n'
            << "p99(ns)" << '\t' << p99 << '\n'
            << "p50(ns)" << '\t' << p50 << '\n'
            << "Min(ns)" << '\t' << min << '\n'
            << "Max(ns)" << '\t' << max << std::endl;
}
} // namespace profiler