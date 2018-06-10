//
// Created by Haposik on 10/06/2018.
//

#ifndef MY_LIST_MYLIST_H
#define MY_LIST_MYLIST_H

#include <algorithm>
#include <cassert>

template <typename T>
class MyList;

template <typename T>
void swap(MyList<T>&, MyList<T>&) noexcept;

template <typename T>
class MyList {
 private:
  struct BaseNode {
    BaseNode* left;
    BaseNode* right;

    BaseNode() : left(nullptr), right(nullptr) {}
    BaseNode(BaseNode* left, BaseNode* right)
        : left(left), right(right)
    {}

    virtual ~BaseNode() = default;

    void FreeUntilEnd(BaseNode* end_);
  };
  struct Node : BaseNode {
    T data;

    Node(BaseNode* left, BaseNode* right, const T& data)
        : BaseNode(left, right), data(data) {}
  };

 public:
  MyList() noexcept;
  MyList(const MyList& other);
  MyList& operator=(MyList other);
  ~MyList();

  friend void swap<T>(MyList& lhs, MyList& rhs) noexcept;

  bool empty() const;
  void clear();

  void push_back(const T &value);
  void pop_back();
  T& back();
  const T& back() const;

  void push_front(const T &value);
  void pop_front();
  T& front();
  const T& front() const;

  // ---------------------------------------------------------------------------
  // -------------------------------- ITERATORS --------------------------------
  // ---------------------------------------------------------------------------

  template <typename U>
  class Iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = U;
    using pointer = U*;
    using reference = U&;
    using iterator_category = std::bidirectional_iterator_tag;

    friend class MyList;

    template <typename V>
    Iterator(const Iterator<V>& other,
             typename std::enable_if<std::is_same<U, const V>::value>::type* = nullptr);

    Iterator& operator++();
    const Iterator operator++(int);
    Iterator& operator--();
    const Iterator operator--(int);

    U& operator*() const;
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
      return lhs.ptr_ == rhs.ptr_;
    }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
      return lhs.ptr_ != rhs.ptr_;
    }

   private:
    explicit Iterator(BaseNode* ptr);

    BaseNode* ptr_;
  };
  using iterator = Iterator<T>;
  using const_iterator = Iterator<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

  iterator insert(const_iterator it, const T &value);
  iterator erase(const_iterator it);
  iterator erase(const_iterator first, const_iterator last);
  iterator splice(const_iterator pos, MyList&, const_iterator first, const_iterator last);

 private:


  BaseNode end_node_;
  BaseNode* end_ = &end_node_;
};

template <typename T>
void MyList<T>::BaseNode::FreeUntilEnd(BaseNode* end_) {
  BaseNode* ptr = right;
  BaseNode* next = right->right;
  while (ptr != end_) {
    delete ptr;
    ptr = next;
    next = next->right;
  }
}

template <typename T>
void swap(MyList<T>& lhs, MyList<T>& rhs) noexcept {
  std::swap(lhs.end_node_, rhs.end_node_);

  if (lhs.end_->left == rhs.end_) {
    lhs.end_->left = &lhs.end_node_;
    lhs.end_->right = &lhs.end_node_;
  }
  if (rhs.end_->left == lhs.end_) {
    rhs.end_->left = &rhs.end_node_;
    rhs.end_->right = &rhs.end_node_;
  }

  lhs.end_->left->right = &lhs.end_node_;
  lhs.end_->right->left = &lhs.end_node_;
  rhs.end_->left->right = &rhs.end_node_;
  rhs.end_->right->left = &rhs.end_node_;
}

template <typename T>
template <typename U>
MyList<T>::Iterator<U>::Iterator(MyList<T>::BaseNode* ptr) : ptr_(ptr) {}

template <typename T>
template <typename U>
template <typename V>
MyList<T>::Iterator<U>::Iterator(const MyList::Iterator<V> &other,
                                 typename std::enable_if<std::is_same<U, const V>::value>::type*)
    : ptr_(other.ptr_)
{}

template <typename T>
template <typename U>
typename MyList<T>::template Iterator<U>& MyList<T>::Iterator<U>::operator++() {
  ptr_ = ptr_->right;
  return *this;
}

template <typename T>
template <typename U>
const typename MyList<T>::template Iterator<U> MyList<T>::Iterator<U>::operator++(int) {
  Iterator tmp(ptr_);
  ++(*this);
  return tmp;
}

template <typename T>
template <typename U>
typename MyList<T>::template Iterator<U>& MyList<T>::Iterator<U>::operator--() {
  ptr_ = ptr_->left;
  return *this;
}

template <typename T>
template <typename U>
const typename MyList<T>::template Iterator<U> MyList<T>::Iterator<U>::operator--(int) {
  Iterator tmp(ptr_);
  --(*this);
  return tmp;
}

template<typename T>
template<typename U>
U& MyList<T>::Iterator<U>::operator*() const {
  return static_cast<Node*>(ptr_)->data;
}

template<typename T>
MyList<T>::MyList() noexcept {
  end_node_.left = end_;
  end_node_.right = end_;
}

template<typename T>
MyList<T>::MyList(const MyList &other) : MyList() {
  BaseNode* ptr = other.end_->right;
  while (ptr != other.end_) {
    push_back(static_cast<Node *>(ptr)->data);
    ptr = ptr->right;
  }
}

template<typename T>
MyList<T>& MyList<T>::operator=(MyList other) {
  swap(*this, other);
  return *this;
}

template<typename T>
MyList<T>::~MyList() {
  end_->FreeUntilEnd(end_);
}

template<typename T>
bool MyList<T>::empty() const {
  return end_->left == end_;
}

template<typename T>
void MyList<T>::clear() {
  end_->FreeUntilEnd(end_);
  end_->left = end_;
  end_->right = end_;
}

template<typename T>
void MyList<T>::push_back(const T &value) {
  auto new_node = new Node(end_->left, end_, value);
  end_->left->right = new_node;
  end_->left = new_node;
}

template<typename T>
void MyList<T>::pop_back() {
  BaseNode* erased_node = end_->left;
  end_->left->left->right = end_;
  end_->left = end_->left->left;
  delete erased_node;
}

template<typename T>
T& MyList<T>::back() {
  assert(!empty());
  return static_cast<Node*>(end_->left)->data;
}

template<typename T>
const T& MyList<T>::back() const {
  assert(!empty());
  return static_cast<Node*>(end_->left)->data;
}

template<typename T>
void MyList<T>::push_front(const T &value) {
  auto new_node = new Node(end_, end_->right, value);
  end_->right->left = new_node;
  end_->right = new_node;
}

template<typename T>
void MyList<T>::pop_front() {
  assert(!empty());
  BaseNode* erased_node = end_->right;
  end_->right->right->left = end_;
  end_->right = end_->right->right;
  delete erased_node;
}

template<typename T>
T& MyList<T>::front() {
  assert(!empty());
  return static_cast<Node*>(end_->right)->data;
}

template<typename T>
const T& MyList<T>::front() const {
  assert(!empty());
  return static_cast<Node*>(end_->right)->data;
}

template<typename T>
typename MyList<T>::iterator MyList<T>::begin() {
  return iterator(end_->right);
}
template<typename T>
typename MyList<T>::const_iterator MyList<T>::begin() const {
  return const_iterator(end_->right);
}
template<typename T>
typename MyList<T>::iterator MyList<T>::end() {
  return iterator(end_);
}
template<typename T>
typename MyList<T>::const_iterator MyList<T>::end() const {
  return const_iterator(end_);
}
template<typename T>
typename MyList<T>::iterator MyList<T>::insert(MyList::const_iterator it, const T &value) {
  BaseNode* new_node = new Node(it.ptr_->left, it.ptr_, value);
  it.ptr_->left->right = new_node;
  it.ptr_->left = new_node;
  return iterator(new_node);
}
template<typename T>
typename MyList<T>::iterator MyList<T>::erase(MyList::const_iterator it) {
  iterator res = iterator(it.ptr_->right);
  it.ptr_->left->right = it.ptr_->right;
  it.ptr_->right->left = it.ptr_->left;
  delete it.ptr_;
  return res;
}

template<typename T>
typename MyList<T>::iterator MyList<T>::erase(MyList::const_iterator first, MyList::const_iterator last) {
  auto tmp_end = new BaseNode(last.ptr_->left, first.ptr_);
  first.ptr_->left->right = last.ptr_;
  last.ptr_->left->right = tmp_end;
  last.ptr_->left = first.ptr_->left;
  first.ptr_->left = tmp_end;
  tmp_end->FreeUntilEnd(tmp_end);
  delete tmp_end;
  return iterator(last.ptr_);
}

template<typename T>
typename MyList<T>::iterator MyList<T>::splice(
    MyList::const_iterator pos,
    MyList&,
    MyList::const_iterator first,
    MyList::const_iterator last)
{
  if (first == last) {
    return iterator(pos.ptr_);
  }
  BaseNode* tmp = last.ptr_;
  last.ptr_->left->right = pos.ptr_;
  pos.ptr_->left->right = first.ptr_;
  first.ptr_->left->right = tmp;
  tmp = pos.ptr_->left;
  pos.ptr_->left = last.ptr_->left;
  last.ptr_->left = first.ptr_->left;
  first.ptr_->left = tmp;
  return iterator(pos.ptr_);
}

#endif //MY_LIST_MYLIST_H
