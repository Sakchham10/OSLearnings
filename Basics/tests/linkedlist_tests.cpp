#include "../linkedlist.hpp"

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

namespace linkedlist_tests {

using basics::linkedlist;

template <typename T> bool contains(linkedlist<T> &list, const T &target) {
  return list.findIf([&target](const T &value) { return value == target; })
      .has_value();
}

void test_empty_list() {
  linkedlist<int> list;

  CHECK(list.getSize() == 0);
  CHECK(!contains(list, 20));
  CHECK(!list.findAndDelete(10));
  CHECK(list.getSize() == 0);
}

void test_append_and_findIf() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  CHECK(list.getSize() == 3);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(contains(list, 30));
  CHECK(!contains(list, 40));
}

void test_append_left_and_findIf() {
  linkedlist<int> list;

  list.appendLeft(30);
  list.appendLeft(20);
  list.appendLeft(10);

  CHECK(list.getSize() == 3);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(contains(list, 30));
  CHECK(!contains(list, 40));
}

void test_delete_head() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  CHECK(list.findAndDelete(10));
  CHECK(list.getSize() == 2);
  CHECK(!contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(contains(list, 30));
}

void test_delete_middle() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  CHECK(list.findAndDelete(20));
  CHECK(list.getSize() == 2);
  CHECK(contains(list, 10));
  CHECK(!contains(list, 20));
  CHECK(contains(list, 30));
}

void test_delete_tail() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  CHECK(list.findAndDelete(30));
  CHECK(list.getSize() == 2);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(!contains(list, 30));
}

void test_delete_only_node() {
  linkedlist<int> list;

  list.append(10);

  CHECK(list.findAndDelete(10));
  CHECK(list.getSize() == 0);
  CHECK(!contains(list, 10));
  CHECK(!list.findAndDelete(10));
}

void test_append_after_delete_only_node() {
  linkedlist<int> list;

  list.append(10);
  CHECK(list.findAndDelete(10));

  list.append(20);

  CHECK(list.getSize() == 1);
  CHECK(!contains(list, 10));
  CHECK(contains(list, 20));

  auto value = list.pop();
  CHECK(value.has_value());
  CHECK(value.value() == 20);
  CHECK(list.getSize() == 0);
}

void test_append_after_delete_tail() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);
  CHECK(list.findAndDelete(30));

  list.append(40);

  CHECK(list.getSize() == 3);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(!contains(list, 30));
  CHECK(contains(list, 40));

  auto value = list.pop();
  CHECK(value.has_value());
  CHECK(value.value() == 40);
}

void test_pop_after_delete_tail() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);
  CHECK(list.findAndDelete(30));

  auto value = list.pop();

  CHECK(value.has_value());
  CHECK(value.value() == 20);
  CHECK(list.getSize() == 1);
  CHECK(contains(list, 10));
  CHECK(!contains(list, 20));
}

void test_pop_left_after_delete_head() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);
  CHECK(list.findAndDelete(10));

  auto value = list.popLeft();

  CHECK(value.has_value());
  CHECK(value.value() == 20);
  CHECK(list.getSize() == 1);
  CHECK(!contains(list, 20));
  CHECK(contains(list, 30));
}

void test_delete_missing_preserves_list() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  CHECK(!list.findAndDelete(40));

  CHECK(list.getSize() == 3);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(contains(list, 30));
}

void test_pop_left_empty_list() {
  linkedlist<int> list;

  auto value = list.popLeft();

  CHECK(!value.has_value());
  CHECK(list.getSize() == 0);
}

void test_pop_left_only_node() {
  linkedlist<int> list;

  list.append(10);

  auto value = list.popLeft();

  CHECK(value.has_value());
  CHECK(value.value() == 10);
  CHECK(list.getSize() == 0);
  CHECK(!contains(list, 10));
}

void test_pop_left_multiple_nodes() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  auto value = list.popLeft();

  CHECK(value.has_value());
  CHECK(value.value() == 10);
  CHECK(list.getSize() == 2);
  CHECK(!contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(contains(list, 30));
}

void test_pop_empty_list() {
  linkedlist<int> list;

  auto value = list.pop();

  CHECK(!value.has_value());
  CHECK(list.getSize() == 0);
}

void test_pop_only_node() {
  linkedlist<int> list;

  list.append(10);

  auto value = list.pop();

  CHECK(value.has_value());
  CHECK(value.value() == 10);
  CHECK(list.getSize() == 0);
  CHECK(!contains(list, 10));
}

void test_pop_multiple_nodes() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  auto value = list.pop();

  CHECK(value.has_value());
  CHECK(value.value() == 30);
  CHECK(list.getSize() == 2);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(!contains(list, 30));
}

void test_pop_sequence_from_right() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  auto first = list.pop();
  auto second = list.pop();
  auto third = list.pop();
  auto fourth = list.pop();

  CHECK(first.has_value());
  CHECK(first.value() == 30);
  CHECK(second.has_value());
  CHECK(second.value() == 20);
  CHECK(third.has_value());
  CHECK(third.value() == 10);
  CHECK(!fourth.has_value());
  CHECK(list.getSize() == 0);
}

void test_pop_sequence_from_left() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  auto first = list.popLeft();
  auto second = list.popLeft();
  auto third = list.popLeft();
  auto fourth = list.popLeft();

  CHECK(first.has_value());
  CHECK(first.value() == 10);
  CHECK(second.has_value());
  CHECK(second.value() == 20);
  CHECK(third.has_value());
  CHECK(third.value() == 30);
  CHECK(!fourth.has_value());
  CHECK(list.getSize() == 0);
}

void test_mixed_append_and_pop_order() {
  linkedlist<int> list;

  list.append(20);
  list.appendLeft(10);
  list.append(30);
  list.appendLeft(5);

  CHECK(list.getSize() == 4);

  auto left = list.popLeft();
  auto right = list.pop();

  CHECK(left.has_value());
  CHECK(left.value() == 5);
  CHECK(right.has_value());
  CHECK(right.value() == 30);
  CHECK(list.getSize() == 2);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
}

void test_append_left_after_pop_to_empty() {
  linkedlist<int> list;

  list.append(10);
  CHECK(list.pop().has_value());

  list.appendLeft(20);

  CHECK(list.getSize() == 1);
  CHECK(contains(list, 20));
  auto value = list.popLeft();
  CHECK(value.has_value());
  CHECK(value.value() == 20);
  CHECK(list.getSize() == 0);
}

void test_append_after_pop_left_to_empty() {
  linkedlist<int> list;

  list.appendLeft(10);
  CHECK(list.popLeft().has_value());

  list.append(20);

  CHECK(list.getSize() == 1);
  CHECK(contains(list, 20));
  auto value = list.pop();
  CHECK(value.has_value());
  CHECK(value.value() == 20);
  CHECK(list.getSize() == 0);
}

void test_string_values() {
  linkedlist<std::string> list;

  list.append("alpha");
  list.append("beta");
  list.append("gamma");

  CHECK(contains<std::string>(list, "alpha"));
  CHECK(contains<std::string>(list, "beta"));
  CHECK(contains<std::string>(list, "gamma"));
  CHECK(list.findAndDelete("beta"));
  CHECK(!contains<std::string>(list, "beta"));
  CHECK(list.getSize() == 2);
}

void test_upsert_appends_missing_value() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);

  list.upsert([](int value) { return value == 30; }, 30);

  CHECK(list.getSize() == 3);
  CHECK(contains(list, 10));
  CHECK(contains(list, 20));
  CHECK(contains(list, 30));
}

void test_upsert_updates_existing_value() {
  linkedlist<int> list;

  list.append(10);
  list.append(20);
  list.append(30);

  list.upsert([](int value) { return value == 20; }, 25);

  CHECK(list.getSize() == 3);
  CHECK(contains(list, 10));
  CHECK(!contains(list, 20));
  CHECK(contains(list, 25));
  CHECK(contains(list, 30));
}

void test_upsert_updates_string_value() {
  linkedlist<std::string> list;

  list.append("alpha");
  list.append("beta");
  list.append("gamma");

  list.upsert([](const std::string &value) { return value == "beta"; },
              std::string("delta"));

  CHECK(list.getSize() == 3);
  CHECK(contains<std::string>(list, "alpha"));
  CHECK(!contains<std::string>(list, "beta"));
  CHECK(contains<std::string>(list, "delta"));
  CHECK(contains<std::string>(list, "gamma"));
}

int run() {
  const std::vector<std::pair<std::string_view, void (*)()>> tests = {
      {"empty list", test_empty_list},
      {"append and findIf", test_append_and_findIf},
      {"append left and findIf", test_append_left_and_findIf},
      {"delete head", test_delete_head},
      {"delete middle", test_delete_middle},
      {"delete tail", test_delete_tail},
      {"delete only node", test_delete_only_node},
      {"append after delete only node", test_append_after_delete_only_node},
      {"append after delete tail", test_append_after_delete_tail},
      {"pop after delete tail", test_pop_after_delete_tail},
      {"pop left after delete head", test_pop_left_after_delete_head},
      {"delete missing preserves list", test_delete_missing_preserves_list},
      {"pop left empty list", test_pop_left_empty_list},
      {"pop left only node", test_pop_left_only_node},
      {"pop left multiple nodes", test_pop_left_multiple_nodes},
      {"pop empty list", test_pop_empty_list},
      {"pop only node", test_pop_only_node},
      {"pop multiple nodes", test_pop_multiple_nodes},
      {"pop sequence from right", test_pop_sequence_from_right},
      {"pop sequence from left", test_pop_sequence_from_left},
      {"mixed append and pop order", test_mixed_append_and_pop_order},
      {"append left after pop to empty", test_append_left_after_pop_to_empty},
      {"append after pop left to empty", test_append_after_pop_left_to_empty},
      {"string values", test_string_values},
      {"upsert appends missing value", test_upsert_appends_missing_value},
      {"upsert updates existing value", test_upsert_updates_existing_value},
      {"upsert updates string value", test_upsert_updates_string_value},
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

} // namespace linkedlist_tests

#ifndef BASICS_COMBINED_TEST_RUNNER
int main() { return linkedlist_tests::run(); }
#endif
