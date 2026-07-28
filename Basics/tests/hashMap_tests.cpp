#include "../hashMap.hpp"

#include <iostream>
#include <optional>
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

struct TestKey {
  int id;
  std::string label;

  bool operator==(const TestKey &other) const {
    return id == other.id && label == other.label;
  }
};

template <> struct std::hash<TestKey> {
  std::size_t operator()(const TestKey &key) const {
    return std::hash<int>{}(key.id) ^ (std::hash<std::string>{}(key.label) << 1);
  }
};

namespace hashmap_tests {

using basics::hashMap;

void test_empty_map_returns_nullopt() {
  hashMap<int, std::string> values;

  auto value = values.get(10);

  CHECK(!value.has_value());
}

void test_put_and_get_value() {
  hashMap<int, std::string> values;

  values.put(10, "ten");

  auto value = values.get(10);
  CHECK(value.has_value());
  CHECK(value.value() == "ten");
}

void test_put_updates_existing_key() {
  hashMap<int, std::string> values;

  values.put(10, "ten");
  values.put(10, "updated");

  auto value = values.get(10);
  CHECK(value.has_value());
  CHECK(value.value() == "updated");
}

void test_collision_bucket_keeps_distinct_keys() {
  hashMap<int, std::string> values(1);

  values.put(10, "ten");
  values.put(20, "twenty");
  values.put(30, "thirty");

  CHECK(values.get(10).value() == "ten");
  CHECK(values.get(20).value() == "twenty");
  CHECK(values.get(30).value() == "thirty");
}

void test_values_survive_resize() {
  hashMap<int, std::string> values(2);

  for (int key = 0; key < 8; ++key) {
    values.put(key, "value-" + std::to_string(key));
  }

  for (int key = 0; key < 8; ++key) {
    auto value = values.get(key);
    CHECK(value.has_value());
    CHECK(value.value() == "value-" + std::to_string(key));
  }
}

void test_values_survive_multiple_resizes() {
  hashMap<int, int> values(1);

  for (int key = 0; key < 100; ++key) {
    values.put(key, key * 10);
  }

  for (int key = 0; key < 100; ++key) {
    auto value = values.get(key);
    CHECK(value.has_value());
    CHECK(value.value() == key * 10);
  }
}

void test_update_and_remove_after_resize() {
  hashMap<int, std::string> values(1);

  for (int key = 0; key < 20; ++key) {
    values.put(key, "original");
  }

  values.put(7, "updated");
  values.remove(12);

  CHECK(values.get(7).value() == "updated");
  CHECK(!values.get(12).has_value());
  CHECK(values.get(6).value() == "original");
  CHECK(values.get(13).value() == "original");
}

void test_remove_existing_key() {
  hashMap<int, std::string> values;

  values.put(10, "ten");
  values.put(20, "twenty");
  values.remove(10);

  CHECK(!values.get(10).has_value());
  CHECK(values.get(20).value() == "twenty");
}

void test_remove_missing_key_preserves_existing_values() {
  hashMap<int, std::string> values;

  values.put(10, "ten");
  values.remove(20);

  auto value = values.get(10);
  CHECK(value.has_value());
  CHECK(value.value() == "ten");
}

void test_string_keys() {
  hashMap<std::string, int> values;

  values.put("alpha", 1);
  values.put("beta", 2);

  CHECK(values.get("alpha").value() == 1);
  CHECK(values.get("beta").value() == 2);
  CHECK(!values.get("gamma").has_value());
}

void test_custom_key_type() {
  hashMap<TestKey, std::string> values;

  values.put(TestKey{1, "one"}, "first");
  values.put(TestKey{2, "two"}, "second");

  CHECK(values.get(TestKey{1, "one"}).value() == "first");
  CHECK(values.get(TestKey{2, "two"}).value() == "second");
  CHECK(!values.get(TestKey{3, "three"}).has_value());
}

int run() {
  const std::vector<std::pair<std::string_view, void (*)()>> tests = {
      {"empty map returns nullopt", test_empty_map_returns_nullopt},
      {"put and get value", test_put_and_get_value},
      {"put updates existing key", test_put_updates_existing_key},
      {"collision bucket keeps distinct keys",
       test_collision_bucket_keeps_distinct_keys},
      {"values survive resize", test_values_survive_resize},
      {"values survive multiple resizes", test_values_survive_multiple_resizes},
      {"update and remove after resize", test_update_and_remove_after_resize},
      {"remove existing key", test_remove_existing_key},
      {"remove missing key preserves existing values",
       test_remove_missing_key_preserves_existing_values},
      {"string keys", test_string_keys},
      {"custom key type", test_custom_key_type},
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

} // namespace hashmap_tests

#ifndef BASICS_COMBINED_TEST_RUNNER
int main() { return hashmap_tests::run(); }
#endif
