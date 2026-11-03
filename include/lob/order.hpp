#pragma once

#include <chrono>
#include <cstdint>
namespace lob {

using OrderId = std::uint64_t;
using Quantity = std::int64_t;
using Price = std::int64_t;
using Time = std::chrono::steady_clock::time_point;
using TradeId = std::uint64_t;

enum class Side { Buy, Sell };

struct Order {
  OrderId id;
  Quantity qty;
  Price price;
  Side side;
};

struct OrderLocation {
  Price price;
  Side side;
};

struct Trade {
  TradeId id;
  Quantity qty;
  Side aggressor;
  Price price;
  Time created_at;
  OrderId buy_order_id;
  OrderId sell_order_id;
};

} // namespace lob