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

bool Book::cancel(OrderId id) {
  auto idx = index_.find(id);
  if (idx == index_.end()) {
    return false;
  }

  const Price p = idx->second.price;
  const Side s = idx->second.side;

  Level &l = s == Side::Buy ? bids_.find(p)->second : asks_.find(p)->second;

  auto oit = l.begin();
  while (oit != l.end() && oit->id != id) {
    ++oit;
  }
  if (oit == l.end()) {
    return false;
  }

  l.erase(oit);

  if (l.empty()) {
    if (s == Side::Buy) {
      bids_.erase(p);
    } else {
      asks_.erase(p);
    }
  }

  index_.erase(id);
  return true;
}

void Book::print() const {
  std::int64_t sumOfQuantity = 0;
  std::cout << '\n' << '\t' << "Asks (sellers)\n";
  std::cout << "Price" << '\t' << "Quantity" << '\n';

  for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
    sumOfQuantity = 0;
    for (auto &orderIt : it->second) {
      sumOfQuantity += orderIt.qty;
    }
    std::cout << it->first << '\t' << sumOfQuantity << '\n';
  }
  std::cout << "---------------------------------------------------------\n";

  for (auto it = bids_.begin(); it != bids_.end(); ++it) {
    sumOfQuantity = 0;
    for (auto &orderIt : it->second) {
      sumOfQuantity += orderIt.qty;
    }
    std::cout << it->first << '\t' << sumOfQuantity << '\n';
  }
  std::cout << '\t' << "Bids (buyers)\n";
}
} // namespace lob