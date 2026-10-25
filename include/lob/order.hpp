#pragma once

#include <cstdint>

namespace lob {

using OrderId = std::uint64_t;
using Quantity = std::int64_t;
using Price = std::int64_t;
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

} // namespace lob