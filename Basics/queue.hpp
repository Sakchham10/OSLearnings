#ifndef BASICS_QUEUE_HPP
#define BASICS_QUEUE_HPP


#include "linkedlist.hpp"
#include <optional>
#include <stdexcept>

namespace basics {

template <typename T> class queue {
  linkedlist<T> list;

public:
  void append(T value) { list.append(std::move(value)); }
  void appendLeft(T value) { list.appendLeft(std::move(value)); }
  T pop() {
    auto value = list.pop();
    if (value == std::nullopt) {
      throw std::runtime_error("cannot pop from empty queue.");
    }
    return std::move(value.value());
  }
  T popLeft() {
    auto value = list.popLeft();
    if (value == std::nullopt) {
      throw std::runtime_error("cannot pop from empty queue.");
    }
    return std::move(value.value());
  }

  int getSize() { return list.getSize(); }
  queue() = default;
  ~queue() = default;
};

} // namespace basics

#endif // BASICS_QUEUE_HPP