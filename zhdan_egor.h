#ifndef LIST_LIST_H
#define LIST_LIST_H

#include <algorithm>
#include <utility>
#include <cassert>

template<typename T>
class list {
public:
    list() noexcept;

    list(const list<T> &other);
    list &operator=(const list<T> &other);

    ~list();

    void push_back(const T &item);
    void push_front(const T &item);

    void pop_back();
    void pop_front();

    const T &front() const;
    const T &back() const;

    bool empty() const;
    void clear();

    template<typename E>
    friend void swap(list<E> &a, list<E> &b) noexcept;

private:
    struct node {
        T value;
        node *next;
        node *prev;

        node(const T& value, node *next, node *last);

        ~node();
    };

public:

    node *first;
    node *last;

    template<typename E>
    class __const_iterator;

    template<typename E>
    class __iterator {
    public:
        friend class list;

        __iterator() : parent(nullptr), cur(nullptr), reversed(false) {}

        __iterator(const list<E> *parent, node *cur, bool reversed) : parent(parent), cur(cur), reversed(reversed) {}
        __iterator(list<E> *parent, node *cur, bool reversed) : parent(parent), cur(cur), reversed(reversed) {}

        template<typename V>
        __iterator(const __iterator<V> &other) : parent(other.parent), cur(other.cur), reversed(other.reversed) {}
        template<typename V>
        __iterator(const __const_iterator<V> &other) : parent(other.parent), cur(other.cur), reversed(other.reversed) {}


        __iterator operator++() {
            if (end()) {
                if (reversed) {
                    cur = parent->first;
                    return *this;
                }
                return *this;
            }

            auto new_cur = !reversed ? cur->next : cur->prev;
            cur = new_cur;
            return *this;
        }

        __iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return *this;
        }

        __iterator operator--() {
            if (end()) {
                if (!reversed) {
                    cur = parent->last;
                    return *this;
                }
            }

            auto new_cur = reversed ? cur->next : cur->prev;
            cur = new_cur;
            return *this;
        }

        __iterator operator--(int) {
            iterator tmp(*this);
            --(*this);
            return *this;
        }

        const E &operator*() const {
            return cur->value;
        }

        E &operator*() {
            return cur->value;
        }

        bool operator==(const __iterator &other) const {
            return cur == other.cur;
        }

        bool operator!=(const __iterator &other) const {
            return cur != other.cur;
        }

        node *get() const {
            return cur;
        }

        typedef std::ptrdiff_t difference_type;
        typedef E value_type;
        typedef E *pointer;
        typedef E &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

    private:
        const list<E> *parent;
        node *cur;
        bool reversed;

        bool end() const {
            return !cur;
        }
    };

    template<typename E>
    class __const_iterator {
    public:
        friend class list;

        __const_iterator() : parent(nullptr), cur(nullptr), reversed(false) {}

        __const_iterator(const list<E> *parent, node *cur, bool reversed) : parent(parent), cur(cur), reversed(reversed) {}
        __const_iterator(list<E> *parent, node *cur, bool reversed) : parent(parent), cur(cur), reversed(reversed) {}

        template<typename V>
        __const_iterator(const __iterator<V> &other) : parent(other.parent), cur(other.cur), reversed(other.reversed) {}
        template<typename V>
        __const_iterator(const __const_iterator<V> &other) : parent(other.parent), cur(other.cur), reversed(other.reversed) {}

        __const_iterator operator++() {
            if (end()) {
                if (reversed) {
                    cur = parent->first;
                    return *this;
                }
                return *this;
            }

            auto new_cur = !reversed ? cur->next : cur->prev;
            cur = new_cur;
            return *this;
        }

        __const_iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return *this;
        }

        __const_iterator operator--() {
            if (end()) {
                if (!reversed) {
                    cur = parent->last;
                    return *this;
                }
            }

            auto new_cur = reversed ? cur->next : cur->prev;
            cur = new_cur;
            return *this;
        }

        __const_iterator operator--(int) {
            iterator tmp(*this);
            --(*this);
            return *this;
        }

        const E &operator*() const {
            return cur->value;
        }

        E &operator*() {
            return cur->value;
        }

        bool operator==(const __const_iterator &other) const {
            return cur == other.cur;
        }

        bool operator!=(const __const_iterator &other) const {
            return cur != other.cur;
        }

        node *get() const {
            return cur;
        }

        typedef std::ptrdiff_t difference_type;
        typedef E value_type;
        typedef E *pointer;
        typedef E &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

    private:
        const list<E> *parent;
        node *cur;
        bool reversed;

        bool end() const {
            return !cur;
        }
    };

    typedef __iterator<T> iterator;
    typedef __const_iterator<T> const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    iterator rbegin();
    iterator rend();
    const_iterator rbegin() const;
    const_iterator rend() const;

    iterator insert(const_iterator it, const T &value);
    iterator erase(const_iterator it);
    typename list<T>::iterator erase(list::const_iterator from, list::const_iterator to);
    void splice(const_iterator it, list &other, const_iterator from, const_iterator to);
};

template<typename E>
void swap(list<E> &a, list<E> &b) noexcept {
    std::swap(a.first, b.first);
    std::swap(a.last, b.last);
}


template<typename T>
list<T>::node::node(const T& value, list::node *next, list::node *last) : value(value), next(next), prev(last) {
}

template<typename T>
list<T>::node::~node() {
}

template<typename T>
list<T>::list() noexcept : first(nullptr), last(nullptr) {}

template<typename T>
bool list<T>::empty() const {
    return !first && !last;
}

template<typename T>
void list<T>::clear() {
    if (empty()) return;

    node *next = first;
    first = last = nullptr;
    while (next) {
        node *cur = next;
        next = cur->next;

        delete cur;
    }
}

template<typename T>
list<T>::~list() {
    clear();
}

template<typename T>
void list<T>::push_back(const T &item) {
    auto *next = new node(item, nullptr, last);
    if (last) {
        last->next = next;
        last = next;
    } else {
        first = last = next;
    }
}

template<typename T>
void list<T>::push_front(const T &item) {
    auto *prev = new node(item, first, nullptr);
    if (first) {
        first->prev = prev;
        first = prev;
    } else {
        first = last = prev;
    }
}

template<typename T>
void list<T>::pop_back() {
    assert(!empty());

    auto *prev = last;
    auto *new_last = prev->prev;
    if (new_last) new_last->next = nullptr;
    delete prev;
    last = new_last;
    if (!last) first = nullptr;
}

template<typename T>
void list<T>::pop_front() {
    assert(!empty());

    auto *next = first;
    auto *new_first = next->next;
    if (new_first) new_first->prev = nullptr;
    delete next;
    first = new_first;
    if (!first) last = nullptr;
}

template<typename T>
const T &list<T>::front() const {
    assert(!empty());

    return first->value;
}

template<typename T>
const T &list<T>::back() const {
    assert(!empty());

    return last->value;
}


template<typename T>
typename list<T>::iterator list<T>::begin() {
    return iterator(this, first, false);
}

template<typename T>
typename list<T>::iterator list<T>::end() {
    return ++iterator(this, last, false);
}

template<typename T>
typename list<T>::const_iterator list<T>::begin() const {
    return const_iterator(this, first, false);
}

template<typename T>
typename list<T>::const_iterator list<T>::end() const {
    return ++const_iterator(this, last, false);
}

template<typename T>
typename list<T>::iterator list<T>::rbegin() {
    return iterator(this, last, true);
}

template<typename T>
typename list<T>::iterator list<T>::rend() {
    return --iterator(this, first, true);
}

template<typename T>
typename list<T>::const_iterator list<T>::rbegin() const {
    return const_iterator(this, last, true);
}

template<typename T>
typename list<T>::const_iterator list<T>::rend() const {
    return --const_iterator(this, first, true);
}

template<typename T>
typename list<T>::iterator list<T>::insert(list<T>::const_iterator it, const T &value) {
    if (it == end()) {
        push_back(value);
        return it;
    }
    auto cur = it.get();

    auto new_node = new node(value, cur, cur->prev);
    if (cur->prev) cur->prev->next = new_node;
    else first = new_node;
    cur->prev = new_node;
    return it;
}

template<typename T>
typename list<T>::iterator list<T>::erase(list<T>::const_iterator it) {
    auto cur = it.get();
    iterator new_it = it;
    ++new_it;

    if (cur->prev)
        cur->prev->next = cur->next;
    else first = cur->next;

    if (cur->next)
        cur->next->prev = cur->prev;
    else last = cur->prev;

    (*cur).~node();
    delete cur;

    return new_it;
}

template<typename T>
typename list<T>::iterator list<T>::erase(list::const_iterator from, list::const_iterator to) {
    iterator new_it = from;
    if (new_it.get()->prev)
        --new_it;
    else new_it = end();

    auto left = from.get();
    auto right = to.get();

    if (left->prev)
        left->prev->next = right;
    else first = right;

    if (right)
        right->prev = left->prev;
    else last = left->prev;

    while (left != right) {
        auto cur = left;
        left = left->next;

        (*cur).~node();
        delete cur;
    }

    return new_it;
}

template<typename T>
void list<T>::splice(list::const_iterator it, list &other, list::const_iterator from, list::const_iterator to) {
    auto *left = from.get();
    auto *right = to.get();
    if (left == right) return;
    auto *cur = it.get();
    auto *cur_next = (cur) ? cur->next : nullptr;

    auto *prev_left = left->prev;
    auto *next_right = right;

    if (cur)
        cur->next = left;
    else first = left;
    left->prev = cur;

    if (right) {
        if (cur_next)
            cur_next->prev = right->prev;
        else last = right->prev;

        right->prev->next = cur_next;
    } else last = other.last;

    if (prev_left)
        prev_left->next = next_right;
    else other.first = next_right;
    if (next_right)
        next_right->prev = prev_left;
    else other.last = prev_left;
}

template<typename T>
list<T>::list(const list<T> &other) : list() {
    for (const auto &it : other) push_back(it);
}

template<typename T>
list<T> &list<T>::operator=(const list<T> &other) {
    list<T> tmp(other);
    swap(*this, tmp);
    return *this;
}


#endif //LIST_LIST_H
