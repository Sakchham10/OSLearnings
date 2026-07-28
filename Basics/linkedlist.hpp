#ifndef BASICS_LINKEDLIST_HPP
#define BASICS_LINKEDLIST_HPP
#include <memory>
#include <mutex>
#include <optional>
#include <semaphore>
#include <utility>

namespace basics {
namespace detail {

template <typename T> struct node {
  T value;
  std::unique_ptr<node<T>> next = nullptr;
  node<T> *prev = nullptr;
  explicit node(T value) : value(std::move(value)) {}
};

} // namespace detail

template <typename T> class linkedlist {
  std::mutex lock;
  std::binary_semaphore readerSem;
  int size;
  int readerCount;
  std::unique_ptr<detail::node<T>> head;
  detail::node<T> *tail;

  void appendUnlocked(T value) {
    auto newNode = std::make_unique<detail::node<T>>(std::move(value));
    if (head.get() == nullptr) {
      head = std::move(newNode);
      tail = head.get();
      size++;
      return;
    }
    if (head.get() == tail) {
      newNode->prev = head.get();
      head->next = std::move(newNode);
      tail = head->next.get();
      size++;
      return;
    }
    auto temp = tail;
    newNode->prev = temp;
    temp->next = std::move(newNode);
    tail = temp->next.get();
    size++;
  }

public:
  linkedlist()
      : size(0), readerSem(1), readerCount(0), head(nullptr), tail(nullptr) {}
  ~linkedlist() = default;
  void append(T value) {
    readerSem.acquire();
    appendUnlocked(std::move(value));
    readerSem.release();
  };
  void appendLeft(T value) {
    auto newNode = std::make_unique<detail::node<T>>(std::move(value));
    readerSem.acquire();
    if (head == nullptr) {
      head = std::move(newNode);
      tail = head.get();
      size++;
      readerSem.release();
      return;
    }
    if (head.get() == tail) {
      auto temp = head.get();
      temp->prev = newNode.get();
      newNode->next = std::move(head);
      head = std::move(newNode);
      tail = temp;
      size++;
      readerSem.release();
      return;
    }
    head->prev = newNode.get();
    newNode->next = std::move(head);
    head = std::move(newNode);
    size++;
    readerSem.release();
  };
  std::optional<T> pop() {
    readerSem.acquire();
    if (head == nullptr) {
      readerSem.release();
      return std::nullopt;
    }
    if (head.get() == tail) {
      auto val = std::move(head->value);
      head = nullptr;
      tail = nullptr;
      size--;
      readerSem.release();
      return val;
    }
    auto temp = tail;
    tail = temp->prev;
    auto val = std::move(temp->value);
    tail->next = nullptr;
    size--;
    readerSem.release();
    return val;
  };
  std::optional<T> popLeft() {
    readerSem.acquire();
    if (head == nullptr) {
      readerSem.release();
      return std::nullopt;
    }
    if (head.get() == tail) {
      auto val = std::move(head->value);
      head = nullptr;
      tail = nullptr;
      size--;
      readerSem.release();
      return val;
    }
    auto temp = head.get();
    auto value = std::move(temp->value);
    head = std::move(head->next);
    head->prev = nullptr;
    size--;
    readerSem.release();
    return value;
  };
  bool findAndDelete(T value) {
    readerSem.acquire();
    auto curr = head.get();
    detail::node<T> *prev = nullptr;
    while (curr != nullptr && curr->value != value) {
      prev = curr;
      curr = curr->next.get();
    }
    if (curr == nullptr) {
      readerSem.release();
      return false;
    }
    if (prev == nullptr) {
      head = std::move(head->next);
      if (head != nullptr) {
        head->prev = nullptr;
      } else {
        tail = nullptr;
      }
      size--;
      readerSem.release();
      return true;
    }
    prev->next = std::move(curr->next);
    if (prev->next != nullptr) {
      prev->next->prev = prev;
    } else {
      tail = prev;
    }
    size--;
    readerSem.release();
    return true;
  }
  template <typename Predicate> std::optional<T> findIf(Predicate pred) {
    lock.lock();
    readerCount++;
    if (readerCount == 1) {
      readerSem.acquire();
    }
    lock.unlock();
    auto curr = head.get();
    while (curr && !pred(curr->value)) {
      curr = curr->next.get();
    }
    lock.lock();
    readerCount--;
    if (readerCount == 0) {
      readerSem.release();
    }
    lock.unlock();
    if (curr == nullptr) {
      return std::nullopt;
    }
    // TODO: only works with copyable value.
    return curr->value;
  };
  template <typename Predicate> bool upsert(Predicate pred, T value) {
    readerSem.acquire();
    auto curr = head.get();
    while (curr && !pred(curr->value)) {
      curr = curr->next.get();
    }
    if (curr == nullptr) {
      appendUnlocked(std::move(value));
      readerSem.release();
      return false;
    }
    curr->value = std::move(value);
    readerSem.release();
    return true;
  };
  template <typename Predicate> bool findIfAndDelete(Predicate pred) {
    readerSem.acquire();
    auto curr = head.get();
    detail::node<T> *prev = nullptr;
    while (curr && !pred(curr->value)) {
      prev = curr;
      curr = curr->next.get();
    }
    if (curr == nullptr) {
      readerSem.release();
      return false;
    }
    if (prev == nullptr) {
      head = std::move(head->next);
      if (head != nullptr) {
        head->prev = nullptr;
      } else {
        tail = nullptr;
      }
      size--;
      readerSem.release();
      return true;
    }
    prev->next = std::move(curr->next);
    if (prev->next != nullptr) {
      prev->next->prev = prev;
    } else {
      tail = prev;
    }
    size--;
    readerSem.release();
    return true;
  };
  int getSize() {
    readerSem.acquire();
    auto currentSize = size;
    readerSem.release();
    return currentSize;
  }
};

} // namespace basics

#endif // BASICS_LINKEDLIST_HPP
