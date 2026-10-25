#pragma once

#include <deque>
#include <functional>
#include <lob/order.hpp>
#include <map>
#include <unordered_map>

namespace lob {

class Book {

public:
  void add(const Order &order);
  bool cancel(OrderId id);
  void print() const;

private:
  using Level = std::deque<Order>;
  std::map<Price, Level, std::greater<Price>> bids_;
  std::map<Price, Level, std::less<Price>> asks_;
  std::unordered_map<OrderId, OrderLocation> index_;
};

} // namespace lob
