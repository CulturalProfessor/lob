#include "lob/order.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <lob/book.hpp>
#include <ostream>

namespace lob {
void Book::add(const Order &order) {

  if (order.side == Side::Buy) {
    bids_[order.price].push_back(order);
  } else if (order.side == Side::Sell) {
    asks_[order.price].push_back(order);
  }

  index_[order.id] = OrderLocation{order.price, order.side};
}

void Book::print() const {
  std::int64_t sumOfQuantity = 0;

  for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
    sumOfQuantity = 0;
    for (auto &orderIt : it->second) {
      sumOfQuantity += orderIt.qty;
    }
    std::cout << it->first << '\t' << sumOfQuantity << '\n';
  }
  std::cout << "\n---------------------------------------------------------\n";
  for (auto it = bids_.begin(); it != bids_.end(); ++it) {
    sumOfQuantity = 0;
    for (auto &orderIt : it->second) {
      sumOfQuantity += orderIt.qty;
    }
    std::cout << it->first << '\t' << sumOfQuantity << '\n';
  }
}
} // namespace lob