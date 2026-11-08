#include <cstdlib>
#include <iostream>
#include <lob/book.hpp>
#include <lob/order.hpp>
#include <profiler/latency.hpp>
#include <vector>

int main() {
  profiler::Profile p;

  // Profle of timer itself
  // for (int i = 0; i < 10000; i++) {
  //   profiler::ScopedTimer t(p);
  // }
  // p.report();

  std::vector<lob::Order> orderList;
  lob::OrderId id = 1;
  for (int i = 0; i < 10000; i++) {
    id += 1;
    lob::Quantity qty = 100 + (i % 50);
    lob::Price price = 10000 + (i % 50);
    lob::Side side = i % 2 == 0 ? lob::Side::Buy : lob::Side::Sell;

    orderList.push_back({id, qty, price, side});
  }

  lob::Book book(10000);
  std::vector<lob::Trade> out;
  std::uint64_t tradeSize = 0;
  for (int i = 0; i < 10000; i++) {
    {
      profiler::ScopedTimer t(p);
      book.add(orderList[i], out);
    }

    tradeSize += out.size();
    out.clear();
  }
  p.report();
  std::cout << "Trade Size " << tradeSize << std::endl;
}