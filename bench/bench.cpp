#include <profiler/latency.hpp>

int main() {
  profiler::Profile p;
  for (int i = 0; i < 10000; i++) {
    profiler::ScopedTimer t(p);
  }
  p.report();
}