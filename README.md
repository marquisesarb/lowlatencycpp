# Low-Latency C++: Order Book with Out-of-Order Update Handling

## Overview

This project implements a basic limit order book in C++, focusing on efficient data structures, predictable operation costs, and handling updates that arrive out of sequence.

The exercise is inspired by a LinkedIn post.

### Main Requirements

* **Sequential update processing:** Updates must be applied in sequence-number order, even when they arrive out of order.
* **Out-of-order update caching:** Updates that arrive before their predecessors must be buffered until the missing sequence numbers are received.
* **Constant-time best-price access:** The best bid and best ask must be accessible in `O(1)`.
* **No `std::map` or `std::priority_queue`:** The core data structures must be implemented using alternative approaches.

The implementation uses a custom binary heap for the update cache and `std::set` to maintain the price levels of each side of the order book.

## Architecture

The application consists of three main components:

1. **`Heap<T>`** — A generic binary heap used to cache pending updates.
2. **`PriceQuantityMap`** — A price-level container based on `std::set`.
3. **`OrderBook`** — The main class that processes updates, maintains the bid and ask sides, and exposes the best prices and quantities.

---

## 1. Update Cache: Custom Binary Heap

The update cache is implemented in `Heap<T>`, using `std::vector` as its underlying storage.

The heap supports both min-heap and max-heap configurations. The order book instantiates it as a min-heap, so the update with the smallest sequence number is always at the root.

```cpp
Heap<Update> updateInQueue;
```

### Binary Heap Structure

A binary heap stores its elements in a contiguous array. For a node at index `i`, the parent and children are calculated directly:

```cpp
parent(i)    = (i - 1) / 2;
leftChild(i) = 2 * i + 1;
rightChild(i)= 2 * i + 2;
```

This avoids the need for explicit pointers between nodes.

### Supported Operations

| Operation | Complexity            |
| --------- | --------------------- |
| `top()`   | `O(1)`                |
| `size()`  | `O(1)`                |
| `push()`  | `O(log N)` worst case |
| `pop()`   | `O(log N)` worst case |

Insertion appends the new element to the vector and moves it toward the root until the heap property is restored.

Removal replaces the root with the last element and moves it downward as necessary.

The result is constant-time access to the smallest pending sequence number, with logarithmic insertion and removal.

### Example

```cpp
Heap<int> minHeap(false);

minHeap.push(5);
minHeap.push(10);
minHeap.push(2);
minHeap.push(15);

// The smallest element is at the root.
assert(minHeap.top() == 2);

minHeap.pop();

assert(minHeap.top() == 5);
```

The same implementation can be configured as a max-heap by passing `true` to the constructor.

---

## 2. Price-Quantity Map

The order book maintains two independent collections of price levels:

* **Bid side:** Buy-side price levels, ordered by ascending price.
* **Ask side:** Sell-side price levels, ordered by ascending price.

Each level is represented by a `PriceQuantityPair`:

```cpp
struct PriceQuantityPair {
    int price;
    int quantity;

    bool operator<(const PriceQuantityPair& other) const {
        return price < other.price;
    }
};
```

The comparison operator considers only the price. Consequently, each `std::set` can contain at most one entry per price level.

The `PriceQuantityMap` class wraps a `std::set<PriceQuantityPair>` and keeps track of whether it represents the bid or ask side.

```cpp
std::set<PriceQuantityPair> pairs_;
bool _isBid;
```

Although `std::map` is prohibited by the exercise, `std::set` provides an ordered tree structure that can implement the required price-level operations.

### Price-Level Updates

The `push()` method first searches for the requested price using `lower_bound()`.

If the price already exists, the incoming quantity is added to the existing quantity. Otherwise, a new price level is inserted, provided the resulting quantity is nonzero.

```cpp
void push(const PriceQuantityPair& pair);
```

The intended behavior is:

* **Existing price:** Aggregate the incoming quantity.
* **New price:** Create a price level.
* **Resulting quantity equal to zero:** Remove the level.

The `erase()` method removes a price level explicitly.

This design represents aggregated quantity at each price, rather than individual orders with unique order identifiers.

### Accessing the Best Price

The best price depends on the side of the book:

* **Best bid:** Maximum price.
* **Best ask:** Minimum price.

Since `std::set` keeps its elements ordered, the implementation uses `rbegin()` for the best bid and `begin()` for the best ask.

```cpp
int bestPrice() const {
    return pairs_.empty()
        ? -1
        : _isBid
            ? pairs_.rbegin()->price
            : pairs_.begin()->price;
}
```

The same approach is used to retrieve the quantity at the best price.

### Operation Complexity

| Operation                          | Complexity            |
| ---------------------------------- | --------------------- |
| Best bid or ask price              | `O(1)`                |
| Quantity at the best price         | `O(1)`                |
| Book depth                         | `O(1)`                |
| Find a price using `lower_bound()` | `O(log N)`            |
| Insert a price level               | `O(log N)` worst case |
| Erase a price level by key         | `O(log N)`            |
| Retrieve the first `K` levels      | `O(K)`                |

The `std::set::insert(iterator, value)` overload can provide amortized constant-time insertion when the supplied hint is sufficiently close to the correct insertion position. However, the overall update operation still includes the logarithmic cost of locating the price.

The `top(n)` method returns up to `n` price levels in book-priority order:

* **Bids:** Highest to lowest.
* **Asks:** Lowest to highest.

---

## 3. OrderBook

The `OrderBook` class coordinates the update cache and the two price-level collections.

### Main Members

```cpp
uint64_t sequence_;
Heap<Update> updateInQueue;
PriceQuantityMap bidSide;
PriceQuantityMap askSide;
int maximumInQueue;
```

* `sequence_` stores the last sequence number processed.
* `updateInQueue` buffers updates that cannot yet be processed.
* `bidSide` maintains the buy-side price levels.
* `askSide` maintains the sell-side price levels.
* `maximumInQueue` limits the number of cached updates.

The constructor accepts an initial sequence number and a maximum cache size:

```cpp
OrderBook(uint64_t initialSequence = 0, int K = 100);
```

By default, the book starts at sequence `0` and can hold up to `100` pending updates.

### Sequential Update Processing

The central method is:

```cpp
void onUpdate(const Update& update);
```

An update is immediately eligible for processing only when its sequence number is exactly one greater than the current sequence number.

Conceptually, the method follows this logic:

1. Compare the incoming sequence number with `sequence_ + 1`.
2. If it matches, advance the sequence and validate the update.
3. Apply the update to the appropriate side of the book.
4. If cached updates exist, inspect the smallest cached sequence number and attempt to process it.
5. If the incoming sequence number does not match the expected sequence, store the update in the heap.
6. If the cache is full, throw a `std::runtime_error`.

This allows the book to recover from out-of-order arrivals without applying updates out of sequence.

### Example

Suppose the current processed sequence is `10`, and updates arrive in the following order:

| Arrival Order | Update Sequence | Behavior                               |
| ------------- | --------------: | -------------------------------------- |
| 1             |              12 | Cached; sequence 11 is missing         |
| 2             |              11 | Processed immediately                  |
| 3             |              12 | Retrieved from the cache and processed |

After update `11` is processed, the order book examines the heap and finds update `12`. Since it is now the expected sequence, it can be processed as well.

This example illustrates the difference between **arrival order** and **processing order**.

### Cache Capacity

The constructor parameter `K` controls the maximum number of cached updates.

```cpp
OrderBook book(0, 100);
```

If another out-of-sequence update arrives when the cache has already reached its configured capacity, the implementation throws:

```cpp
throw std::runtime_error("Update cache is full");
```

The exact exception message depends on the implementation.

---

## Summary

The project combines a custom binary heap with ordered price-level containers to implement a basic limit order book that supports out-of-order updates.

The key design choices are:

* A **custom binary min-heap** provides efficient buffering and retrieval of pending updates by sequence number.
* **`std::set`** maintains ordered bid and ask price levels without using `std::map` or `std::priority_queue`.
* **Constant-time best-price access** is achieved through the set's `begin()` and `rbegin()` iterators.
* **Sequential processing** ensures updates are applied in sequence-number order, while the cache holds updates whose predecessors have not yet arrived.
* **Bounded caching** limits memory growth by restricting the number of pending updates.

Together, these components provide a straightforward foundation for exploring data-structure choices, update sequencing, and performance trade-offs in C++ order-book implementations.
