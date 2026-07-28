#include <iostream>

namespace linkedlist_tests {
int run();
}

namespace queue_tests {
int run();
}

namespace hashmap_tests {
int run();
}

int main() {
  std::cout << "[RUN] LinkedListTests\n";
  if (const int result = linkedlist_tests::run(); result != 0) {
    return result;
  }

  std::cout << "[RUN] QueueTests\n";
  if (const int result = queue_tests::run(); result != 0) {
    return result;
  }

  std::cout << "[RUN] HashMapTests\n";
  if (const int result = hashmap_tests::run(); result != 0) {
    return result;
  }

  std::cout << "[PASS] all tests\n";
  return 0;
}
