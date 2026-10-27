#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <lob/book.hpp>
#include <lob/order.hpp>
#include <sstream>
#include <string>
#include <string_view>

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

    book.add(lob::Order{order_id, order_quantity, order_price, order_side});
  }

  if (in.bad()) {
    std::cerr << "read error after line " << lineno << '\n';
    return EXIT_FAILURE;
  }

  book.print();

  return EXIT_SUCCESS;
}
