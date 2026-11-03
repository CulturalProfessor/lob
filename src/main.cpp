#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <lob/book.hpp>
#include <lob/order.hpp>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <replay-file>\n";
    return EXIT_FAILURE;
  }

  std::ifstream in(argv[1]);
  if (!in) {
    std::cerr << "cannot open " << argv[1] << '\n';
    return EXIT_FAILURE;
  }

  std::string line;
  std::size_t lineno = 0;
  lob::Book book;
  std::vector<lob::Trade> out;

  std::cout << '\n'
            << std::right << std::setw(6) << "TRADE" << std::setw(8) << "QTY"
            << std::setw(9) << "PRICE" << std::setw(9) << "BUY-ID"
            << std::setw(9) << "SELL-ID" << std::setw(7) << "AGGR"
            << std::setw(18) << "TIMESTAMP(ns)" << '\n';

  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    ++lineno;
    std::stringstream ss(line);
    std::string field;

    std::getline(ss, field, ',');
    std::string message_type = field;

    std::getline(ss, field, ',');
    std::uint64_t order_id = std::stoll(field);

    if (message_type == "C") {
      book.cancel(order_id);
      continue;
    }

    std::getline(ss, field, ',');
    lob::Side order_side;
    if (field == "B") {
      order_side = lob::Side::Buy;
    } else {
      order_side = lob::Side::Sell;
    }

    std::getline(ss, field, ',');
    std::int64_t order_price = std::stoll(field);

    std::getline(ss, field, ',');
    std::int64_t order_quantity = std::stoll(field);

    book.add(lob::Order{order_id, order_quantity, order_price, order_side},
             out);

    for (const lob::Trade &t : out) {
      std::cout << std::setw(6) << t.id << std::setw(8) << t.qty << std::setw(9)
                << t.price << std::setw(9) << t.buy_order_id << std::setw(9)
                << t.sell_order_id << std::setw(7)
                << (t.aggressor == lob::Side::Buy ? "Buy" : "Sell")
                << std::setw(18) << t.created_at.time_since_epoch().count()
                << '\n';
    }

    out.clear();
  }

  if (in.bad()) {
    std::cerr << "read error after line " << lineno << '\n';
    return EXIT_FAILURE;
  }

  book.print();

  return EXIT_SUCCESS;
}
