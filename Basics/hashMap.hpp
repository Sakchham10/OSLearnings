//
// Created by Sakchham Sangroula on 7/23/26.
//

#ifndef BASICS_HASHMAP_HPP
#define BASICS_HASHMAP_HPP

#include "linkedlist.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <semaphore>
#include <utility>

namespace basics {
namespace detail {

template <typename T, typename V> struct Slot {
  explicit Slot(T key, V value)
      : key(std::move(key)), value(std::move(value)) {}
  T key;
  V value;
};

} // namespace detail

template <typename T, typename V> class hashMap {
  std::unique_ptr<linkedlist<detail::Slot<T, V>>[]> slots;
  size_t slotCount;
  size_t totalCount;
  size_t notResizeCount;
  std::binary_semaphore resizeSem;
  std::mutex countLock;
  std::mutex reCountLock;
  const size_t threshold = 1;
  static std::size_t hashKey(const T &key, std::size_t slotCount) {
    return std::hash<T>{}(key) % slotCount;
  }
  void enterNonResizeOperation() {
    std::unique_lock<std::mutex> resizeCountLock(reCountLock);
    notResizeCount++;
    if (notResizeCount == 1) {
      resizeSem.acquire();
    }
  }
  void exitNonResizeOperation() {
    std::unique_lock<std::mutex> resizeCountLock(reCountLock);
    notResizeCount--;
    if (notResizeCount == 0) {
      resizeSem.release();
    }
  }
  void resize() {
    resizeSem.acquire();
    auto newSlots =
        std::make_unique<linkedlist<detail::Slot<T, V>>[]>(slotCount * 2);
    for (auto i = 0; i < slotCount; i++) {
      auto &bucket = slots[i];
      while (auto slot = bucket.popLeft()) {
        if (!slot.has_value()) {
          break;
        }
        auto bucketKey = hashKey(slot->key, slotCount * 2);
        auto &newBucket = newSlots[bucketKey];
        newBucket.append(slot.value());
      }
    }
    slotCount = slotCount * 2;
    slots = std::move(newSlots);
    resizeSem.release();
  };

public:
  std::optional<V> get(T key) {
    enterNonResizeOperation();
    auto bucketKey = hashKey(key, slotCount);
    auto &bucket = slots[bucketKey];
    auto existingSlot =
        bucket.findIf(
            [&key](detail::Slot<T, V> &slot) { return slot.key == key; });
    if (!existingSlot.has_value()) {
      exitNonResizeOperation();
      return std::nullopt;
    }
    exitNonResizeOperation();
    return existingSlot->value;
  };
  void put(T key, V value) {
    enterNonResizeOperation();
    auto bucketKey = hashKey(key, slotCount);
    auto &bucket = slots[bucketKey];
    auto found =
        bucket.upsert(
            [&key](detail::Slot<T, V> &slot) { return slot.key == key; },
            detail::Slot(std::move(key), std::move(value)));

    exitNonResizeOperation();
    if (found) {
      return;
    }
    std::unique_lock<std::mutex> cLock(countLock);
    totalCount++;
    if (((totalCount) / slotCount) > threshold) {
      resize();
    }
  };
  void remove(T key) {
    enterNonResizeOperation();
    auto bucketKey = hashKey(key, slotCount);
    auto &bucket = slots[bucketKey];
    auto deleted = bucket.findIfAndDelete(
        [&key](detail::Slot<T, V> &slot) { return slot.key == key; });
    exitNonResizeOperation();
    if (!deleted)
      return;
    std::unique_lock<std::mutex> cLock(countLock);
    totalCount--;
  }

  hashMap(std::size_t initialCount = 10)
      : slots(std::make_unique<linkedlist<detail::Slot<T, V>>[]>(initialCount)),
        slotCount(initialCount), totalCount(0), notResizeCount(0),
        resizeSem(1) {};
};

} // namespace basics

#endif // BASICS_HASHMAP_HPP
