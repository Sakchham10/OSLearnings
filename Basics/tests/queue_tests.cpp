#include "../queue.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#define CHECK(condition)                                                       \
  do {                                                                         \
    if (!(condition)) {                                                        \
      throw std::runtime_error("check failed: " #condition);                   \
    }                                                                          \
  } while (false)

namespace queue_tests {

using basics::queue;

void check_throws_empty_pop(auto pop) {
  try {
    pop();
  } catch (const std::runtime_error &error) {
    CHECK(std::string_view(error.what()) == "cannot pop from empty queue.");
    return;
  }

  throw std::runtime_error("check failed: expected empty queue pop to throw");
}

void test_empty_queue() {
  queue<int> values;

  CHECK(values.getSize() == 0);
  check_throws_empty_pop([&values]() { values.pop(); });
  check_throws_empty_pop([&values]() { values.popLeft(); });
  CHECK(values.getSize() == 0);
}

void test_append_and_pop() {
  queue<int> values;

  values.append(10);
  values.append(20);
  values.append(30);

  CHECK(values.getSize() == 3);
  CHECK(values.pop() == 30);
  CHECK(values.pop() == 20);
  CHECK(values.pop() == 10);
  CHECK(values.getSize() == 0);
}

void test_append_and_pop_left() {
  queue<int> values;

  values.append(10);
  values.append(20);
  values.append(30);

  CHECK(values.getSize() == 3);
  CHECK(values.popLeft() == 10);
  CHECK(values.popLeft() == 20);
  CHECK(values.popLeft() == 30);
  CHECK(values.getSize() == 0);
}

void test_append_left_and_pop_left() {
  queue<int> values;

  values.appendLeft(30);
  values.appendLeft(20);
  values.appendLeft(10);

  CHECK(values.getSize() == 3);
  CHECK(values.popLeft() == 10);
  CHECK(values.popLeft() == 20);
  CHECK(values.popLeft() == 30);
  CHECK(values.getSize() == 0);
}

void test_append_left_and_pop() {
  queue<int> values;

  values.appendLeft(30);
  values.appendLeft(20);
  values.appendLeft(10);

  CHECK(values.getSize() == 3);
  CHECK(values.pop() == 30);
  CHECK(values.pop() == 20);
  CHECK(values.pop() == 10);
  CHECK(values.getSize() == 0);
}

void test_mixed_append_and_pop_order() {
  queue<int> values;

  values.append(20);
  values.appendLeft(10);
  values.append(30);
  values.appendLeft(5);

  CHECK(values.getSize() == 4);
  CHECK(values.popLeft() == 5);
  CHECK(values.pop() == 30);
  CHECK(values.popLeft() == 10);
  CHECK(values.pop() == 20);
  CHECK(values.getSize() == 0);
}

void test_append_after_pop_to_empty() {
  queue<int> values;

  values.append(10);
  CHECK(values.pop() == 10);

  values.append(20);
  values.appendLeft(15);

  CHECK(values.getSize() == 2);
  CHECK(values.popLeft() == 15);
  CHECK(values.popLeft() == 20);
  CHECK(values.getSize() == 0);
}

void test_string_values() {
  queue<std::string> values;

  values.append("alpha");
  values.append("beta");
  values.appendLeft("zero");

  CHECK(values.getSize() == 3);
  CHECK(values.popLeft() == "zero");
  CHECK(values.pop() == "beta");
  CHECK(values.pop() == "alpha");
  CHECK(values.getSize() == 0);
}

int run() {
  const std::vector<std::pair<std::string_view, void (*)()>> tests = {
      {"empty queue", test_empty_queue},
      {"append and pop", test_append_and_pop},
      {"append and pop left", test_append_and_pop_left},
      {"append left and pop left", test_append_left_and_pop_left},
      {"append left and pop", test_append_left_and_pop},
      {"mixed append and pop order", test_mixed_append_and_pop_order},
      {"append after pop to empty", test_append_after_pop_to_empty},
      {"string values", test_string_values},
  };

  for (const auto &[name, test] : tests) {
    try {
      test();
      std::cout << "[PASS] " << name << '\n';
    } catch (const std::exception &error) {
      std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
      return 1;
    }
  }

  return 0;
}

} // namespace queue_tests

#ifndef BASICS_COMBINED_TEST_RUNNER
int main() { return queue_tests::run(); }
#endif
