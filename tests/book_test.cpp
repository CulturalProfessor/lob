#include <cstdlib>
#include <iostream>
#include <lob/book.hpp>
#include <lob/order.hpp>
#include <vector>

static void check(bool ok, const char *what);
static int failures = 0;
static int testCount = 0;

int main() {

  lob::Book book;
  lob::Quantity sumOfQuantity = 0;

  std::vector<lob::Order> orders = {
      // resting book
      {1, 75, 1027, lob::Side::Sell},
      {2, 20, 1027, lob::Side::Sell},
      {3, 25, 1028, lob::Side::Sell},
      {4, 40, 1029, lob::Side::Sell},
      {5, 50, 1024, lob::Side::Buy},
      // crossing orders
      {6, 30, 1027, lob::Side::Buy},
      {7, 45, 1027, lob::Side::Buy},
      {8, 50, 1028, lob::Side::Buy},
      {9, 60, 1026, lob::Side::Buy},
      {10, 100, 1030, lob::Side::Buy},
      {11, 15, 1032, lob::Side::Sell},
  };

  std::vector<lob::Trade> out;
  for (const auto &it : orders) {
    book.add(it, out);
  }
  check(out.size() == 5, "Trade Count");

  for (const auto &it : out) {
    sumOfQuantity += it.qty;
  }
  check(sumOfQuantity == 160, "Traded Quantity");

  // data/cancels.txt - a fresh book, since the one above is full of resting
  // orders from cross_buy.
  lob::Book cancelBook;
  std::vector<lob::Order> cancelOrders = {
      {1, 100, 1025, lob::Side::Buy}, {2, 30, 1025, lob::Side::Buy},
      {3, 70, 1025, lob::Side::Buy},  {4, 50, 1024, lob::Side::Buy},
      {5, 80, 1027, lob::Side::Sell}, {6, 25, 1028, lob::Side::Sell},
      {7, 45, 1028, lob::Side::Sell},
  };

  std::vector<lob::Trade> cancelOut;
  for (const auto &order : cancelOrders) {
    cancelBook.add(order, cancelOut);
  }
  check(cancelOut.empty(), "nothing crosses in the cancel setup");
  check(cancelBook.cancel(2), "cancel middle of a busy level");
  check(cancelBook.cancel(4), "cancel the only order at 1024");
  check(cancelBook.cancel(5), "cancel the only order at the best ask");
  check(!cancelBook.cancel(99), "cancel an id that never existed");
  check(!cancelBook.cancel(2), "cancel an id already cancelled");
  check(cancelBook.cancel(6), "cancel the front of a busy level");

  std::cout << '\n'
            << testCount - failures << "/" << testCount << " Tests Passed\n";
  return failures == 0 ? 0 : 1;
}

static void check(bool ok, const char *what) {
  testCount += 1;
  if (ok) {
    std::cout << "OK    " << what << '\n';
  } else {
    std::cout << "FAIL  " << what << '\n';
    ++failures;
  }
}