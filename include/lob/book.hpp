#pragma once

#include <deque>
#include <functional>
#include <lob/order.hpp>
#include <map>
#include <unordered_map>
#include <vector>

namespace lob {

class Book {

public:
  void add(const Order &order, std::vector<Trade> &out);
  bool cancel(OrderId id);
  void print() const;

  Book() {}
  Book(std::uint64_t indexSize) { index_.reserve(indexSize); }

private:
  using Level = std::deque<Order>;
  std::map<Price, Level, std::greater<Price>> bids_;
  std::map<Price, Level, std::less<Price>> asks_;
  std::unordered_map<OrderId, OrderLocation> index_;
  TradeId next_trade_id_ = 1;
};

} // namespace lob
