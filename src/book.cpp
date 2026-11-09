#include "lob/order.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <lob/book.hpp>
#include <ostream>
#include <vector>

namespace lob {
void Book::add(const Order &order, std::vector<Trade> &out) {

  Time createdAt = std::chrono::steady_clock::now();

  // managing trade
  if (order.side == Side::Buy) {
    Quantity remaining = order.qty;

    while (remaining > 0 && !asks_.empty()) {
      auto best = asks_.begin();
      if (best->first > order.price) {
        break;
      }
      Level &level = best->second;
      Order &resting = level.front();

      Quantity traded = std::min(remaining, resting.qty);
      remaining -= traded;
      resting.qty -= traded;

      out.push_back({next_trade_id_, traded, order.side, best->first, createdAt,
                     order.id, resting.id});
      next_trade_id_ += 1;

      if (resting.qty == 0) {
        index_.erase(resting.id);
        level.pop_front();
      }

      if (level.empty()) {
        asks_.erase(best);
      }
    }

    if (remaining > 0) {
      bids_[order.price].push_back(
          {order.id, remaining, order.price, order.side});
      index_[order.id] = OrderLocation{order.price, order.side};
    }

  } else if (order.side == Side::Sell) {
    Quantity remaining = order.qty;

    while (remaining > 0 && !bids_.empty()) {
      auto best = bids_.begin();
      if (best->first < order.price) {
        break;
      }
      Level &level = best->second;
      Order &resting = level.front();

      Quantity traded = std::min(remaining, resting.qty);
      remaining -= traded;
      resting.qty -= traded;

      out.push_back({next_trade_id_, traded, order.side, best->first, createdAt,
                     resting.id, order.id});
      next_trade_id_ += 1;

      if (resting.qty == 0) {
        index_.erase(resting.id);
        level.pop_front();
      }

      if (level.empty()) {
        bids_.erase(best);
      }
    }

    if (remaining > 0) {
      asks_[order.price].push_back(
          {order.id, remaining, order.price, order.side});
      index_[order.id] = OrderLocation{order.price, order.side};
    }
  }
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
  lob::Quantity sumOfQuantity = 0;
  std::cout << '\n' << '\t' << "Asks (sellers)\n";
  std::cout << "Price" << '\t' << "Quantity" << '\n';

  for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
    sumOfQuantity = 0;
    for (auto &orderIt : it->second) {
      sumOfQuantity += orderIt.qty;
    }
    std::cout << it->first << '\t' << sumOfQuantity << '\n';
  }
  std::cout << "---------------------------------"
               "------------------------\n";

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