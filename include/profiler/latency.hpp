#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

namespace profiler {

class Profile {
public:
  void record(std::uint64_t ns);
  void report();

private:
  std::vector<std::uint64_t> storage_;
};

class ScopedTimer {
public:
  ScopedTimer(Profile &p)
      : profile_(p), start_(std::chrono::steady_clock::now()) {}

  ~ScopedTimer() {
    auto end = std::chrono::steady_clock::now();
    auto elapsed = end - start_;

    std::uint64_t ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();

    profile_.record(ns);
  }

private:
  Profile &profile_;
  std::chrono::steady_clock::time_point start_;
};
} // namespace profiler