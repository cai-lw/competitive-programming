#pragma once

#include <functional>
#include <string>
#include <vector>

namespace cplib {

namespace impl {

// Left-child right-sibling representation of the pairing heap
template <typename T>
struct PairingHeapNode {
  PairingHeapNode(const T& val) : _val(val), left(nullptr), right(nullptr), parent(nullptr) {}

  PairingHeapNode(T&& val) : _val(std::move(val)), left(nullptr), right(nullptr), parent(nullptr) {}

  template <typename... Args, typename = decltype(T(std::declval<Args>()...))>
  PairingHeapNode(Args&&... args) : _val(std::forward<Args>(args)...), left(nullptr), right(nullptr), parent(nullptr) {}

  // Add `child` as the youngest child (first in the sibling list). Both `this` and `child` must be root nodes.
  void adopt(PairingHeapNode* child) {
    if (!child) {
      return;
    }
    child->right = left;
    if (left) {
      left->parent = child;
    }
    left = child;
    child->parent = this;
  }

  // Deatch this node and its subtree from its parent, so that it becomes an independent root node.
  // Must only be called on non-root nodes.
  void detach() {
    if (parent->left == this) {
      parent->left = right;
    } else {
      parent->right = right;
    }
    if (right) {
      right->parent = parent;
    }
    right = nullptr;
    parent = nullptr;
  }

  T _val;
  PairingHeapNode *left, *right, *parent;
};

}  // namespace impl

/**
 * \brief Pairing heap, a pointer-based heap supporting efficient merge and decrease-key.
 * \ingroup order
 *
 * A min-heap with `std::priority_queue`-like interface and supports efficient merge(), decrease_key() and erase(). The
 * latter two take "iterators" returned by push(). "Iterators" are just read-only pointers and cannot be moved around.
 *
 * All operators except for pop() finishes in constant time, while pop() can take linear time in the worst case.
 * A certain amortized analysis gives that the amortized cost is \f$O(\log N)\f$ for pop(), \f$o(\log N)\f$ for
 * decrease_key(), and \f$O(1)\f$ for all other operations. See https://en.wikipedia.org/wiki/Pairing_heap for more.
 *
 * \tparam T Type of elements.
 * \tparam Comp Comparison function object. Note that this is a min-heap, as opposed to `std::priority_queue`, so the
 * element on the top is the smallest element ordered by the comparison function.
 */
template <typename T, typename Comp = std::less<T>>
struct PairingHeap {
 public:
  using size_type = std::size_t;
  using node_type = impl::PairingHeapNode<T>;

  PairingHeap() : root(nullptr), _size(0), comp() {}

  struct iterator {
    const T& operator*() { return node->_val; }

    const T* operator->() { return &node->_val; }

    bool operator==(const iterator& rhs) const { return node == rhs.node; }

    bool operator!=(const iterator& rhs) const { return node != rhs.node; }

   private:
    friend PairingHeap;
    node_type* node;

    iterator(node_type* node) : node(node) {}
  };

  /** \brief Returns the number of elements. */
  size_type size() const { return _size; }

  /** \brief Returns whether the heap is empty. */
  bool empty() const { return !root; }

  /** \brief Returns the top element. The heap must be non-empty. */
  const T& top() { return root->_val; }

  /** \brief Returns an iterator to the top element, or a null-pointer iterator if the heap is empty. */
  iterator begin() { return iterator(root); }

  /** \brief Returns a null-pointer iterator, intended for default value or edge case checking. */
  iterator end() { return iterator(nullptr); }

  /**
   * \brief Insert an element into the heap.
   *
   * \f$O(1)\f$ worst-case latency and \f$O(\log N)\f$ amortized cost.
   */
  iterator push(const T& t) {
    auto node = new node_type(t);
    _merge_with(node, 1);
    return iterator(node);
  }

  /** \copydoc push(const T&) */
  iterator push(T&& t) {
    auto node = new node_type(std::move(t));
    _merge_with(node, 1);
    return iterator(node);
  }

  /** \brief Construct a new element in place in the heap.
   * \copydetails push(const T&) */
  template <typename... Args>
  iterator emplace(Args&&... args) {
    auto node = new node_type(std::forward<Args>(args)...);
    _merge_with(node, 1);
    return iterator(node);
  }

  /**
   * \brief Remove the top element from the heap.
   *
   * The heap must be non-empty. The iterator pointing to the top element is invalidated.
   * \f$O(N)\f$ worse-case latency and \f$O(\log N)\f$ amortized cost.
   */
  void pop() {
    _size--;
    if (!root->left) {
      delete root;
      root = nullptr;
      return;
    }
    auto* curr = root->left;
    node_type* last = nullptr;
    delete root;
    // Merge in pairs, following the forward (`right`) linked list, and creating a backward (`parent`) linked list
    while (true) {
      auto next = curr->right;
      if (!next) {
        curr->parent = last;
        last = curr;
        break;
      }
      auto next_next = next->right;
      curr->parent = nullptr;
      curr->right = nullptr;
      next->parent = nullptr;
      next->right = nullptr;
      auto merged = _merge_node(curr, next);
      merged->parent = last;
      last = merged;
      if (!next_next) {
        break;
      }
      curr = next_next;
    }
    // Merge from the back, following the backward (`parent`) linked list just created.
    auto prev = last->parent;
    while (prev) {
      auto prev_prev = prev->parent;
      last->parent = nullptr;
      prev->parent = nullptr;
      last = _merge_node(last, prev);
      last->parent = prev_prev;
      prev = prev_prev;
    }
    root = last;
  }

  /**
   * \brief Merge this heap with another heap.
   *
   * The other heap is emptied and all of its elements are added to this heap. Iterators to elements in the other heap
   * remain valid. \f$O(1)\f$ worst-case latency and amortized cost.
   */
  void merge(PairingHeap&& other) {
    _merge_with(other.root, other.size());
    other._size = 0;
    other.root = nullptr;
  }

  /**
   * \brief Decrease the value of a given element.
   *
   * The new value must be no greater than the current value compared by `Comp`.
   * \f$O(1)\f$ worst-case latency and \f$o(\log N)\f$ amortized cost.
   */
  void decrease_key(iterator it, const T& new_key) {
    if (it.node != root) {
      it.node->detach();
      it.node->_val = new_key;
      _merge_with(it.node, 0);
    } else {
      root->_val = new_key;
    }
  }

  /**
   * \brief Remove an arbitrary element from the heap.
   *
   * Equivalent to decrease_key() the element to negative infininty, followed by pop(). The given iterator is
   * invalidated. \f$O(N)\f$ worse-case latency and \f$O(\log N)\f$ amortized cost.
   */
  void erase(iterator it) {
    if (it.node != root) {
      it.node->detach();
      it.node->adopt(root);
      root = it.node;
    }
    pop();
  }

 private:
  node_type* root;
  size_type _size;
  Comp comp;

  node_type* _merge_node(node_type* node1, node_type* node2) {
    if (!node1) {
      return node2;
    } else if (!node2) {
      return node1;
    }
    if (comp(node1->_val, node2->_val)) {
      std::swap(node1, node2);
    }
    node2->adopt(node1);
    return node2;
  }

  void _merge_with(node_type* other, size_type size_incr) {
    root = _merge_node(root, other);
    _size += size_incr;
  }
};

}  // namespace cplib