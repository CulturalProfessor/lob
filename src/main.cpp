#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <lob/order.hpp>

int main(int argc, char** argv) {
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

  while (std::getline(in, line)) {
    ++lineno;
    std::string_view msg{line};
    if (msg.empty() || msg.front() == '#') continue;
 
    std::cout << lineno << ": " << msg << '\n';
  }

  if (in.bad()) {
    std::cerr << "read error after line " << lineno << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
